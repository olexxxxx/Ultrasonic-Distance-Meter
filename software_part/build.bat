@echo off
REM --- 1. Налаштування шляхів ---
REM Увага: шляхи без лапок тут, лапки додамо пізніше
SET COMPILER_PATH=C:\Program Files (x86)\COSMIC\FSE_Compilers\CXSTM8
SET LIB_PATH=C:\Program Files (x86)\COSMIC\FSE_Compilers\CXSTM8\Lib
SET OUT_DIR=Debug
SET TARGET=main

REM --- 2. Підготовка ---
if not exist %OUT_DIR% mkdir %OUT_DIR%
REM Очистка сміття перед збіркою
del /Q %OUT_DIR%\*.o %OUT_DIR%\*.sm8 %OUT_DIR%\*.map %OUT_DIR%\*.s19 2>NUL

echo [1/3] Compiling Source Files...

REM --- 3. Компіляція (cxstm8) ---
REM Компілюємо main.c
"%COMPILER_PATH%\cxstm8" +mods0 -i"%COMPILER_PATH%\Hstm8" -i"drivers\inc" -i"registers_include" -cl%OUT_DIR% -co%OUT_DIR% start\main.c
if %errorlevel% neq 0 goto error

REM Компілюємо вектори переривань
"%COMPILER_PATH%\cxstm8" +mods0 -i"%COMPILER_PATH%\Hstm8" -i"drivers\inc" -i"registers_include" -cl%OUT_DIR% -co%OUT_DIR% start\stm8_interrupt_vector.c
if %errorlevel% neq 0 goto error

echo [2/3] Linking...

echo [2/3] Linking...

REM --- 4. Лінковка (clnk) ---
REM Ми залишаємо тільки -l (шлях до ліб) і сам .lkf файл.
REM Список об'єктів і назви ліб тепер беруться зсередини .lkf
"%COMPILER_PATH%\clnk" -o %OUT_DIR%\%TARGET%.sm8 -m %OUT_DIR%\%TARGET%.map -l"%LIB_PATH%" ajax_ultrasonic_meter.lkf
if %errorlevel% neq 0 goto error

echo [3/3] Converting to Hex...

REM --- 5. Конвертація (chex) у Intel Hex ---
REM -fi вмикає формат Intel Hex
"%COMPILER_PATH%\chex" -fi -o %OUT_DIR%\%TARGET%.ihx %OUT_DIR%\%TARGET%.sm8
if %errorlevel% neq 0 goto error

echo.
echo ==========================================
echo BUILD SUCCESSFUL! 
echo Firmware: %OUT_DIR%\%TARGET%.s19
echo ==========================================
goto end

:error
echo.
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo BUILD FAILED! Check errors above.
echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
exit /b 1

:end