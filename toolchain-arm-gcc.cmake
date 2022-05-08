include(CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(hints_paths)

if (CMAKE_HOST_WIN32)
    set(X86_PROGRAM_FILES "ProgramFiles(x86)")
    list(APPEND hints_paths "$ENV{${X86_PROGRAM_FILES}}/GNU Tools Arm Embedded/8 2019-q3-update/bin/")
    file(GLOB bin_paths "C:/ST/STM32CubeIDE_*/STM32CubeIDE/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.*/tools/bin/")
    list(APPEND hints_paths ${bin_paths})
endif()

find_program(CMAKE_C_COMPILER
    NAMES arm-none-eabi-gcc
    HINTS ${hints_paths})

find_program(CMAKE_CXX_COMPILER
    NAMES arm-none-eabi-g++
    HINTS ${hints_paths})

find_program(CMAKE_ASM_COMPILER
    NAMES arm-none-eabi-as
    HINTS ${hints_paths})

find_program(CMAKE_AR
    NAMES arm-none-eabi-ar
    HINTS ${hints_paths})

find_program(CMAKE_RANLIB
    NAMES arm-none-eabi-ranlib
    HINTS ${hints_paths})

find_program(CMAKE_NM
    NAMES arm-none-eabi-nm
    HINTS ${hints_paths})

find_program(CCOLA_LD_TOOL
    NAMES arm-none-eabi-ld
    HINTS ${hints_paths})

find_program(CCOLA_APP_SIZE_TOOL
    NAMES arm-none-eabi-size
    HINTS ${hints_paths})

find_program(CCOLA_OBJ_COPY_TOOL
    NAMES arm-none-eabi-objcopy
    HINTS ${hints_paths})

find_program(CCOLA_OBJ_DUMP_TOOL
    NAMES arm-none-eabi-objdump
    HINTS ${hints_paths})

find_program(CCOLA_DEBUG_TOOL
    NAMES arm-none-eabi-gdb
    HINTS ${hints_paths})

add_link_options(LINKER:--gc-sections,--print-memory-usage)
add_link_options(-specs=nano.specs -nostartfiles -mthumb -mcpu=cortex-m7)

add_compile_options(-ffunction-sections -fdata-sections -mthumb -mcpu=cortex-m7 -g3)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit$<SEMICOLON>-fno-threadsafe-statics$<SEMICOLON>-fno-rtti$<SEMICOLON>-fno-exceptions>)
