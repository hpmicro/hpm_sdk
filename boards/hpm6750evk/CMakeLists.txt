# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

if(BOARD_LCD_WIDTH)
sdk_compile_definitions("-DBOARD_LCD_WIDTH=${BOARD_LCD_WIDTH}")
endif()

if(BOARD_LCD_HEIGHT)
sdk_compile_definitions("-DBOARD_LCD_HEIGHT=${BOARD_LCD_HEIGHT}")
endif()

if(BUILD_FOR_SECONDARY_CORE)
sdk_compile_definitions(BOARD_RUNNING_CORE=HPM_CORE1)
endif()

sdk_inc(.)
sdk_src(pinmux.c)
sdk_src(board.c)
