@echo off
REM --- Налаштування ---
REM Повний шлях до папки з stm8flash (згідно з твоїм скріном)
SET TOOL_PATH=C:\tools\stm8flash
SET FIRMWARE=Debug\main.ihx
SET PART=stm8s103f3
SET PROGRAMMER=stlinkv2

echo Flashing %PART% with %FIRMWARE%...

REM --- Прошивка ---
"%TOOL_PATH%\stm8flash.exe" -c %PROGRAMMER% -p %PART% -w %FIRMWARE%

if %errorlevel% neq 0 goto error

echo.
echo FLASHING SUCCESSFUL!
goto end

:error
echo.
echo FLASHING FAILED!
exit /b 1

:end