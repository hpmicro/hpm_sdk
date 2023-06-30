# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

if(BOARD_SEARCH_DIR)
    add_subdirectory(${BOARD_SEARCH_DIR} ${BOARD})
else()
    add_subdirectory(${HPM_BOARD_DIR})
endif()
