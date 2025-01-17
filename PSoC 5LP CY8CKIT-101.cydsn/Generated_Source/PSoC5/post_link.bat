@REM This script allows a 3rd party IDE to use CyHexTool to perform
@REM any post processing that is necessary to convert the raw flash
@REM image into a complete hex file to use in programming the PSoC.
@REM USAGE: post_link.bat
@REM    arg1: Persistant path back to the directory containing the app project.
@REM    arg2: Path (relative to arg1) of the directory where the hex files go.
@REM    arg3: Name of the project.
@REM NOTE: This script is auto generated. Do not modify.

"C:\Program Files\Cypress\PSoC Creator\2.1\PSoC Creator\bin\cyvalidateide.exe" -dev CY8C5868AXI-LP035 -ide "%~1\%~3" -flsAddr 0x0 -flsSize 0x40000 -sramAddr 0x1FFF8000 -sramSize 0x10000
@IF %errorlevel% NEQ 0 EXIT /b %errorlevel% 
move "%~1\%~2\%~n3.hex" "%~1\%~2\%~n3.ihx"
@IF %errorlevel% NEQ 0 EXIT /b %errorlevel% 
"C:\Program Files\Cypress\PSoC Creator\2.1\PSoC Creator\bin\cyhextool" -o "%~1\%~2\%~n3.hex" -f "%~1\%~2\%~n3.ihx" -prot "%~dp0protect.hex" -id 2E123069 -a EEPROM=90200000:800,PROGRAM=00000000:40000,CONFIG=80000000:8000,PROTECT=90400000:100 -meta 0001 -cunv 00004005 -wonv AFAC90BC -ecc "%~dp0config.hex" 
@IF %errorlevel% NEQ 0 EXIT /b %errorlevel% 
