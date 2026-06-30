# TFLM Porting Notes for HPMicro SDK

## Upstream Baseline
- **Repository**: https://github.com/tensorflow/tflite-micro
- **Old baseline commit**: `1a0287fc5fa81fa6aa1dcfb0c5b2e01f74164393`
- **New baseline commit**: `f2b2b3f51cdf97fe190ef8bd6d1839013c9c179e`

## Directory Layout Mapping

| SDK Path | Upstream Path | Notes |
|----------|--------------|-------|
| `middleware/tflm/tf_core/tensorflow/` | `tensorflow/` | Core TFLM source |
| `middleware/tflm/third_party/flatbuffers/` | Downloaded via make | Not in upstream repo directly |
| `middleware/tflm/third_party/gemmlowp/` | Downloaded via make | Not in upstream repo directly |
| `middleware/tflm/third_party/kissfft/` | `third_party/kissfft/` | |
| `middleware/tflm/third_party/ruy/` | `third_party/ruy/` | |

## HPM Modifications to Upstream Files

### 1. Platform Compatibility (`__HPMICRO__` macro)

**Files**: `kernels/internal/cppmath.h`, `kernels/internal/min.h`, `kernels/internal/max.h`

**What**: Adds `|| defined(__HPMICRO__)` alongside `__ZEPHYR__` in preprocessor guards. This ensures that on the HPMicro platform (which lacks a full hosted C++ stdlib), TFLM uses global-scope `min`/`max`/cmath functions instead of `std::` prefixed versions.

**Why**: The Andes RISC-V toolchain's C++ standard library does not always provide `std::min`/`std::max`/`std::round` etc. in the expected way. This is the same workaround used by Zephyr.

### 2. Debug Log Output

**File**: `micro/debug_log.cc`

**What**: Replaces `vfprintf(stderr, format, args)` with `vprintf(format, args)`.

**Why**: MCU environments typically don't have a separate stderr stream. Using `vprintf` routes debug output through the SDK's retargeted stdout (typically UART console).

### 3. CountLeadingSignBits Workaround (`common.h`)

**File**: `kernels/internal/common.h`

**What**: Added `!defined(__HPMICRO__)` to the `CountLeadingSignBits` preprocessor guard, so the Andes RISC-V GCC toolchain uses the portable fallback instead of `__builtin_clrsb`.

**Why**: The Andes GCC fork (used in SEGGER Embedded Studio) defines `__GNUC__` but does not define `__nds__`, causing it to enter the `__builtin_clrsb` code path. However, `__builtin_clrsb` in the Andes GCC fork produces incorrect results on certain inputs. Using `__HPMICRO__` (already defined by all TFLM sample CMakeLists) is consistent with the platform guards in `cppmath.h`, `min.h`, and `max.h`.

### 4. C++ Standard Library Header Compatibility (micro_profiler.cc)

**File**: `micro/micro_profiler.cc`

**What**: Changed `#include <cinttypes>` to `#include <inttypes.h>`.

**Why**: SEGGER Embedded Studio's libc++ implementation has an incomplete `<cinttypes>` header. The C++ standard requires `<cinttypes>` to declare symbols like `imaxdiv_t`, `strtoimax`, etc. in the `std::` namespace, but SEGGER's implementation does not properly import these symbols from the underlying C library.

**Evidence**:
- SEGGER Forum discussion: https://forum.segger.com/index.php/Thread/4579-SOLVED-inttypes-h-according-to-standard/
- StackOverflow discussion: https://stackoverflow.com/questions/74891743/using-declarations-not-defined-when-including-cinttypes
- C++ LWG Issue 1449: https://cplusplus.github.io/LWG/issue1449

**Alternative**: The C-style header `<inttypes.h>` works correctly because it declares symbols in the global namespace, which is what the TFLM code actually uses. Note that other files in the upstream TFLM codebase (e.g., `cortex_m_corstone_300/system_setup.cc`) also use `<inttypes.h>` for similar reasons.

## HPM Additions (SDK-Only Files)

### Build System

- **`middleware/tflm/CMakeLists.txt`**: Top-level CMake integration, sets up include paths and subdirectories.
- **`middleware/tflm/tf_core/CMakeLists.txt`**: Include path for `tf_core/`.
- **`middleware/tflm/tf_core/tensorflow/lite/CMakeLists.txt`**: Master source list with `CONFIG_HPM_MATH` conditional compilation:
  - When `CONFIG_HPM_MATH` is **not** set: compiles upstream `micro/kernels/*.cc` (reference implementations).
  - When `CONFIG_HPM_MATH` **is** set: compiles `micro/kernels/libnn/*.cc` (Andes NN library accelerated implementations).

### Accelerated Kernels (`micro/kernels/libnn/`)

Seven kernel files that replace reference implementations with calls to `hpm_math.h` (which wraps `riscv_nn_*` from Andes libnn):

| File | Accelerated Operations |
|------|----------------------|
| `libnn/add.cc` | Element-wise add (int8) |
| `libnn/conv.cc` | Conv2D (int8, multiple strategies: 1x1, 1xN, general) |
| `libnn/depthwise_conv.cc` | Depthwise Conv2D (int8) |
| `libnn/fully_connected.cc` | Fully connected / dense (int8) |
| `libnn/pooling.cc` | Average/Max pooling (int8) |
| `libnn/reshape.cc` | Reshape (passthrough, no acceleration needed) |
| `libnn/softmax.cc` | Softmax (int8) |

**Design pattern**: Each libnn kernel file follows the same structure:
1. Includes `hpm_math.h` instead of reference headers.
2. Extends `OpData` struct with a `buffer_idx` for scratch memory.
3. In `Prepare()`: calculates required scratch buffer size via `hpm_nn_*_get_buffer_size()`.
4. In `Eval()`: checks tensor parameters to select optimized path or falls back to reference implementation (e.g., when dilation != 1 for conv).

### Model Data

- `micro/models/person_detect_model_data.cc` / `.h`: Pre-baked model for the person detection sample.

## Integration Points

- **`middleware/CMakeLists.txt`**: `add_subdirectory_ifdef(CONFIG_TFLM tflm)` enables the middleware.
- **`middleware/hpm_math/CMakeLists.txt`**: Links `libnn_rvp32.a` and sets up NN library includes when `CONFIG_HPM_MATH_NN_*` is set.
- **Sample CMakeLists**: Set `CONFIG_TFLM=1`, `CONFIG_HPM_MATH=1`, and define `__HPMICRO__`.
