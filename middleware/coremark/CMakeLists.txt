# Copyright (c) 2021,2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(./)
sdk_src(core_list_join.c)
sdk_src(core_matrix.c)
sdk_src(core_state.c)
sdk_src(core_util.c)
sdk_src(core_main.c)

set(opt_string "-O3 -funroll-all-loops -finline-limit=600 -fno-code-hoisting -fselective-scheduling -fno-forward-propagate -fno-tree-ter -fipa-pta -fno-tree-vrp")
separate_arguments(opt_list UNIX_COMMAND ${opt_string})

if("${TOOLCHAIN_VARIANT}" STREQUAL "zcc")
    set(zcc_opt_string "-O3 -flto -falign-functions=4 -flate-loop-unroll -malign-branch")
    separate_arguments(zcc_opt_list UNIX_COMMAND ${zcc_opt_string})
    foreach(opt ${zcc_opt_list})
        sdk_zcc_compile_options(${opt})
    endforeach()
    sdk_zcc_ld_options(-O3)
    sdk_zcc_ld_options("-fuse-size-lib")
    sdk_zcc_ld_options(-Wl,-mllvm,--riscv-misched-load-clustering=true)
    sdk_zcc_ld_options(-Wl,-mllvm,--misfetch-cost=8)
    sdk_zcc_ld_options(-Wl,-mllvm,--misched-topdown=true)
    sdk_zcc_ld_options(-Wl,-mllvm,--disable-constant-hoisting=true)
    sdk_zcc_ld_options(-Wl,--andes-relax-gp)

    foreach(opt ${opt_list})
        sdk_ses_compile_options(${opt})
    endforeach()
else()
    foreach(opt ${opt_list})
        sdk_compile_options(${opt})
    endforeach()
endif()

sdk_get_compile_options(all_opts)
sdk_compile_definitions(COMPILER_FLAGS="${all_opts}")
sdk_nds_compile_options(-mcmov)

