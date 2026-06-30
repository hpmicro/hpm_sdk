MicroQuickJS
============

## 简介

MicroQuickJS（又称 MQuickJS）是一款专为嵌入式系统设计的 JavaScript 引擎。它仅需 **10 kB RAM** 即可编译和运行 JavaScript 程序。整个引擎仅需约 **100 kB ROM**（ARM Thumb-2 代码），包含 C 标准库。其运行速度与 QuickJS 相当。

MQuickJS 仅支持接近 ES5 的 [JavaScript 子集](#javascript-子集参考)。它实现了一种**更严格模式**，禁用了一些容易出错或低效的 JavaScript 构造。

虽然 MQuickJS 与 QuickJS 共享大量代码，但其内部实现不同，以减少内存消耗。特别是，它依赖于追踪式垃圾回收器，虚拟机不使用 CPU 栈，字符串以 UTF-8 格式存储。

## REPL

REPL 程序为 `mqjs`。用法：

```
usage: mqjs [options] [file [args]]
-h  --help            列出选项
-e  --eval EXPR       执行表达式 EXPR
-i  --interactive     进入交互模式
-I  --include file    包含额外文件
-d  --dump            输出内存使用统计
    --memory-limit n  限制内存使用为 'n' 字节
--no-column           调试信息中不包含列号
-o FILE               保存字节码到 FILE
-m32                  强制输出 32 位字节码（与 -o 配合使用）
-b  --allow-bytecode  允许输入文件为字节码
```

使用 10 kB RAM 编译运行程序：

```sh
./mqjs --memory-limit 10k tests/mandelbrot.js
```

除了正常的脚本执行，`mqjs` 还可以将编译后的字节码输出到持久存储（文件或 ROM）：

```sh
./mqjs -o mandelbrot.bin tests/mandelbrot.js
```

然后可以像普通脚本一样运行编译后的字节码：

```sh
./mqjs -b mandelbrot.bin
```

字节码格式取决于 CPU 的字节序和字长（32 位或 64 位）。在 64 位 CPU 上，可以使用 `-m32` 选项生成可在嵌入式 32 位系统上运行的 32 位字节码。

使用 `--no-column` 选项移除列号调试信息（仅保留行号），以节省存储空间。

## 更严格模式

MQuickJS 仅支持 JavaScript 的一个子集（主要是 ES5）。它始终处于**更严格模式**，禁用了一些容易出错的 JavaScript 特性。其核心理念是更严格模式是 JavaScript 的子集，因此在其他 JavaScript 引擎中仍能正常工作。主要特点如下：

- 只允许**严格模式**构造，因此不允许使用 `with` 关键字，全局变量必须使用 `var` 关键字声明。

- 数组不能有空洞。在数组末尾之后写入元素是不允许的：
```js
    a = []
    a[0] = 1; // 可以扩展数组长度
    a[10] = 2; // TypeError
```
  如果需要一个有空洞的类数组对象，请使用普通对象：
```js
    a = {}
    a[0] = 1;
    a[10] = 2;
```
  `new Array(len)` 仍按预期工作，但数组元素初始化为 `undefined`。
  带有空洞的数组字面量是语法错误：
```js
    [ 1, , 3 ] // SyntaxError
```

- 只支持全局 `eval`，因此它不能访问或修改局部变量：
```js
    eval('1 + 2'); // 禁止
    (1, eval)('1 + 2'); // 可以
```

- 没有值装箱：不支持 `new Number(1)`，而且永远不需要。

## JavaScript 子集参考

- 只支持严格模式，强调 ES5 兼容性。

- `Array` 对象：
    - 没有空洞。
    
    - 数值属性始终由数组对象处理，不会转发到其原型。
  
    - 越界写入是错误，除非在数组末尾。
      
    - `length` 属性是数组原型中的 getter/setter。

- 所有属性都是可写、可枚举和可配置的。

- `for in` 只迭代对象自身属性。应使用以下常见模式以保持与标准 JavaScript 的一致行为：
  
```js
    for(var prop in obj) {
        if (obj.hasOwnProperty(prop)) {
            ...
        }
    }
```    
建议使用 `for of`，它支持数组：

```js
    for(var prop of Object.keys(obj)) {
        ...
    }
```

- `prototype`、`length` 和 `name` 是函数对象中的 getter/setter。

- C 函数不能有自己的属性（但 C 构造函数行为正常）。

- 支持全局对象，但不鼓励使用。它不能包含 getter/setter，直接在其中创建的属性在执行脚本中不会作为全局变量可见。

- 与 `catch` 关键字关联的变量是普通变量。

- 不支持直接 `eval`。只支持间接（=全局）`eval`。

- 没有值装箱（例如不支持 `new Number(1)`）

- 正则表达式：

    - 大小写折叠仅适用于 ASCII 字符。

    - 匹配是 unicode 模式，即 `/./` 匹配一个 unicode 码点而不是 UTF-16 字符（如同使用 `u` 标志）。

- 字符串：`toLowerCase` / `toUpperCase` 仅处理 ASCII 字符。

- Date：只支持 `Date.now()`。

ES5 扩展：
  
- 支持 `for of`，但只迭代数组。不支持自定义迭代器（暂不支持）。

- 类型化数组（Typed arrays）。

- 字符串字面量中接受 `\u{hex}`

- Math 函数：`imul`、`clz32`、`fround`、`trunc`、`log2`、`log10`。

- 指数运算符

- 正则表达式：接受 dotall（`s`）、sticky（`y`）和 unicode（`u`）标志。在 unicode 模式下，不支持 unicode 属性。

- 字符串函数：`codePointAt`、`replaceAll`、`trimStart`、`trimEnd`。

- `globalThis` 全局属性。

## C API

### 引擎初始化

MQuickJS 几乎不依赖 C 标准库。特别是它不使用 `malloc()`、`free()` 或 `printf()`。创建 MQuickJS 上下文时，必须提供内存缓冲区。引擎只在该缓冲区中分配内存：
```c
    JSContext *ctx;
    uint8_t mem_buf[8192];
    ctx = JS_NewContext(mem_buf, sizeof(mem_buf), &js_stdlib);
    ...
    JS_FreeContext(ctx);
```
`JS_FreeContext(ctx)` 仅在需要调用用户对象的终结器时使用，因为引擎不会分配系统内存。

### 内存处理

C API 与 QuickJS 非常相似（见 `mquickjs.h`）。但由于使用了压缩垃圾回收器，有重要区别：

1. 不需要显式释放值（没有 `JS_FreeValue()`）。

2. 每次调用 JS 分配时，对象的地址可能会移动。一般规则是避免在 C 中使用 `JSValue` 类型的变量。它们只能在 MQuickJS API 调用之间临时使用。其他情况下，始终使用指向 `JSValue` 的指针。`JS_PushGCRef()` 返回指向存储在 `JSGCRef` 变量中的临时不透明 `JSValue` 的指针。`JS_PopGCRef()` 用于释放临时引用。当对象移动时，`JSGCRef` 中的不透明值会自动更新。示例：

```c
JSValue my_js_func(JSContext *ctx, JSValue *this_val, int argc, JSValue *argv)
{
        JSGCRef obj1_ref, obj2_ref;
        JSValue *obj1, *obj2, ret;

        ret = JS_EXCEPTION;
        obj1 = JS_PushGCRef(ctx, &obj1_ref);
        obj2 = JS_PushGCRef(ctx, &obj2_ref);
        *obj1 = JS_NewObject(ctx);
        if (JS_IsException(*obj1))
            goto fail;
        *obj2 = JS_NewObject(ctx); // obj1 可能移动
        if (JS_IsException(*obj2))
            goto fail;
        JS_SetPropertyStr(ctx, *obj1, "x", *obj2);  // obj1 和 obj2 可能移动
        ret = *obj1;
     fail:
        PopGCRef(ctx, &obj2_ref);
        PopGCRef(ctx, &obj1_ref);
        return ret;
}
```

在 PC 上运行时，可以使用 `DEBUG_GC` 宏强制 JS 分配器在每次分配时都移动对象。这是检查是否使用了无效 JSValue 的好方法。

### 标准库

标准库由自定义工具（`mquickjs_build.c`）编译为可驻留在 ROM 中的 C 结构。因此，标准库实例化非常快，几乎不需要 RAM。`mqjs_stdlib.c` 提供了 `mqjs` 的标准库示例，编译结果为 `mqjs_stdlib.h`。

`example.c` 是一个使用 MQuickJS C API 的完整示例。

### 持久化字节码

`mqjs` 生成的字节码可从 ROM 执行。在这种情况下，在烧录到 ROM 之前必须重定位（见 `JS_RelocateBytecode()`）。然后使用 `JS_LoadBytecode()` 实例化，并像普通脚本一样使用 `JS_Run()` 运行（见 `mqjs.c`）。

与 QuickJS 一样，字节码级别不保证向后兼容。此外，字节码在执行前不进行验证。只运行来自可信源的 JavaScript 字节码。

### 数学库和浮点仿真

MQuickJS 包含自己的小型数学库（在 `libm.c` 中）。此外，如果 CPU 没有浮点支持，它包含自己的浮点仿真器，可能比 GCC 工具链提供的更小。

## 内部实现及与 QuickJS 的比较

### 垃圾回收

使用追踪式压缩垃圾回收器代替引用计数。这使得对象更小。GC 为每个分配的内存块增加少量位数的开销。此外，避免了内存碎片。

引擎有自己的内存分配器，不依赖 C 库的 malloc。

### 值和对象表示

值的大小与 CPU 字长相同（因此在 32 位 CPU 上为 32 位）。一个值可以包含：

  - 一个 31 位整数（1 位标签）

  - 单个 unicode 码点（即一个或两个 16 位代码单元的字符串）

  - 在 64 位 CPU 字长下，具有较小指数的 64 位浮点数

  - 指向内存块的指针。内存块中存储有标签。

JavaScript 对象至少需要 3 个 CPU 字（因此在 32 位 CPU 上为 12 字节）。根据对象类可能会分配额外数据。属性存储在哈希表中。每个属性至少需要 3 个 CPU 字。标准库对象的属性可以驻留在 ROM 中。

属性键是 JSValue，不同于 QuickJS 中它们有特定类型。它们可以是字符串或正 31 位整数。字符串属性键是内部化的（唯一的）。

字符串内部以 WTF-8（UTF-8 + 未配对代理项）存储，而 QuickJS 中是 8 或 16 位数组。代理项对不会显式存储，但在 JavaScript 中迭代 16 位代码单元时仍然可见。因此保持了与 JavaScript 和 UTF-8 的完全兼容性。

C 函数可以存储为单个值以减少开销。在这种情况下，不能添加额外属性。大多数标准库函数都以这种方式存储。

### 标准库

整个标准库驻留在 ROM 中。它在编译时生成。只有少数对象在 RAM 中创建。因此引擎实例化时间非常短。

### 字节码

它是基于栈的字节码（类似于 QuickJS）。但是，字节码通过间接表引用原子。

行号和列号信息使用 [指数 Golomb 编码](https://en.wikipedia.org/wiki/Exponential-Golomb_coding) 压缩。

### 编译

解析器与 QuickJS 非常接近，但避免递归，因此 C 栈使用是有界的。没有抽象语法树。字节码在单遍中生成，并有多种优化技巧（QuickJS 有多遍优化）。

## 测试和基准测试

运行基本测试：
```sh
make test
```

运行 QuickJS 微基准测试：
```sh
make microbench
```

可以在[这里](https://bellard.org/mquickjs/mquickjs-extras.tar.xz)下载额外测试和适配更严格模式的 Octane 基准测试补丁版本：

运行 V8 octane 基准测试：
```sh
make octane
```

## 许可证

MQuickJS 以 MIT 许可证发布。

除非另有说明，MQuickJS 源代码版权归 Fabrice Bellard 和 Charlie Gordon 所有。
