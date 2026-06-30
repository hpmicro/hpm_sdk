# Copyright 2023 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

if(EXCLUDED_IDES)
    foreach(ide IN ITEMS ${EXCLUDED_IDES})
        if(${ide} STREQUAL "iar")
            set(EXCLUDED_IDE_IAR 1)
        elseif(${ide} STREQUAL "ses")
            set(EXCLUDED_IDE_SES 1)
        endif()
    endforeach()
endif()

include(${HPM_SDK_BASE}/cmake/ide/iar.cmake)
include(${HPM_SDK_BASE}/cmake/ide/segger.cmake)

# Generate IDE projects for all supported IDEs
#
# This function generates project files for all supported IDEs (IAR Embedded Workbench
# and Segger Embedded Studio). It is a convenience wrapper that calls generate_iar_project()
# and generate_ses_project() internally. IDEs can be excluded via the EXCLUDED_IDES variable.
#
# Example:
#   generate_ide_projects()
#
# :param None: This function does not take any parameters.
# @public
function (generate_ide_projects)
    if(NOT EXCLUDED_IDE_IAR OR NOT ${EXCLUDED_IDE_IAR})
        generate_iar_project()
    endif()
    if(NOT EXCLUDED_IDE_SES OR NOT ${EXCLUDED_IDE_SES})
        generate_ses_project()
    endif()
endfunction()
