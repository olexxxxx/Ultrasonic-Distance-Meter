@echo off
setlocal EnableDelayedExpansion

REM ==========================================
REM      CONFIGURATIONS
REM ==========================================
SET COMPILER_PATH=C:\Program Files (x86)\COSMIC\FSE_Compilers\CXSTM8
SET LIB_PATH=C:\Program Files (x86)\COSMIC\FSE_Compilers\CXSTM8\Lib
SET OUT_DIR=Debug
SET TARGET=main
SET LKF_FILE=%OUT_DIR%\%TARGET%.lkf

REM Source folders
SET SOURCE_DIRS=start drivers\src

REM Include folders
SET INCLUDE_FLAGS=-i"%COMPILER_PATH%\Hstm8" -i"drivers\inc" -i"registers_include"

REM ==========================================
REM      PREPARATION
REM ==========================================
if not exist %OUT_DIR% mkdir %OUT_DIR%
REM Clean old debug files
del /Q %OUT_DIR%\*.sm8 %OUT_DIR%\*.map %OUT_DIR%\*.s19 %OUT_DIR%\*.ihx %OUT_DIR%\*.elf 2>NUL

echo ==========================================
echo [1/5] Compiling Source Files...
echo ==========================================

REM 1. Compile ALL files
for %%D in (%SOURCE_DIRS%) do (
    if exist %%D\*.c (
        for %%F in (%%D\*.c) do (
            echo Compiling %%F...
            "%COMPILER_PATH%\cxstm8" +mods0 +debug -no %INCLUDE_FLAGS% -cl%OUT_DIR% -co%OUT_DIR% "%%F"
            if !errorlevel! neq 0 goto error
        )
    )
)

echo ==========================================
echo [2/5] Generating Linker Script...
echo ==========================================

REM 2. Generate LKF file
REM Крок 1: Основна розмітка пам'яті (як у stm8_ex_edu.lkf)
(
    echo # LINK COMMAND FILE
    echo.
    echo # Segment Code,Constants:
    echo +seg .const -b 0x8080 -m 0x1f80 -n .const -it
    echo +seg .text  -a .const           -n .text
    echo # Segment Eeprom:
    echo +seg .eeprom -b 0x4000 -m 0x280 -n .eeprom
    echo # Segment Zero Page:
    echo +seg .bsct   -b 0x0    -m 0x100 -n .bsct
    echo +seg .ubsct  -a .bsct           -n .ubsct
    echo +seg .bit    -a .ubsct          -n .bit   -id
    echo +seg .share  -a .bit            -n .share -is
    echo # Segment Ram:
    echo +seg .data   -b 0x100  -m 0x100 -n .data
    echo +seg .bss    -a .data           -n .bss
    echo.
    
    echo # Startup file
    echo crtsi0.sm8
    echo.
    
    echo # Object files list
) > "%LKF_FILE%"

REM Крок 2: Додаємо всі файли КРІМ векторів
for %%D in (%SOURCE_DIRS%) do (
    if exist %%D\*.c (
        for %%F in (%%D\*.c) do (
            if "%%~nxF" neq "stm8_interrupt_vector.c" (
                echo %OUT_DIR%\%%~nF.o >> "%LKF_FILE%"
            )
        )
    )
)

REM Крок 3: Бібліотеки та ВЕКТОРИ (Вектори суворо в кінці!)
(
    echo.
    echo # Library list
    echo libis0.sm8
    echo libm0.sm8
    echo.
    
    echo # Interrupt vectors file
    echo +seg .const -b 0x8000 -k
    echo %OUT_DIR%\stm8_interrupt_vector.o
    echo.
    
    echo # Defines
    echo +def __endzp=@.ubsct
    echo +def __memory=@.bss
    echo +def __startmem=@.bss
    echo +def __endmem=0x1ff
    echo +def __stack=0x3ff
) >> "%LKF_FILE%"

echo ==========================================
echo [3/5] Linking...
echo ==========================================
"%COMPILER_PATH%\clnk" -o %OUT_DIR%\%TARGET%.sm8 -m %OUT_DIR%\%TARGET%.map -l"%LIB_PATH%" "%LKF_FILE%"
if !errorlevel! neq 0 goto error

echo ==========================================
echo [4/5] Converting to Hex...
echo ==========================================
"%COMPILER_PATH%\chex" -fi -o %OUT_DIR%\%TARGET%.ihx %OUT_DIR%\%TARGET%.sm8
if !errorlevel! neq 0 goto error

echo ==========================================
echo [5/5] Creating Debug Info (ELF)...
echo ==========================================
"%COMPILER_PATH%\cvdwarf" -o %OUT_DIR%\%TARGET%.elf %OUT_DIR%\%TARGET%.sm8
if !errorlevel! neq 0 goto error

echo.
echo BUILD SUCCESSFUL!
goto end

:error
echo.
echo BUILD FAILED!
exit /b 1

:end