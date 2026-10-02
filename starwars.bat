@echo off
rem Beginning of the Star Wars main theme, played by the vibration motor.
rem The pitch of the motor rises with its drive voltage: about 100 mV per semitone.
rem Usage: starwars.bat [path of wp81powertool.exe]

setlocal
set TOOL=%~1
if "%TOOL%"=="" set TOOL=wp81powertool.exe

rem Notes (drive voltage in mV)
set F4=1300
set BB4=1800
set C5=2000
set D5=2200
set EB5=2300
set F5=2500
set BB5=3000

rem Durations in ms (about 108 bpm), silence between the notes included
set G=40
set T=145
set Q=515
set H=1070

call :note %F4% %T%
call :note %F4% %T%
call :note %F4% %T%

call :note %BB4% %H%
call :note %F5% %H%

call :note %EB5% %T%
call :note %D5% %T%
call :note %C5% %T%
call :note %BB5% %H%
call :note %F5% %Q%

call :note %EB5% %T%
call :note %D5% %T%
call :note %C5% %T%
call :note %BB5% %H%
call :note %F5% %Q%

call :note %EB5% %T%
call :note %D5% %T%
call :note %EB5% %T%
call :note %C5% %H%

endlocal
goto :eof

:note
"%TOOL%" --vibrate %1 %2
"%TOOL%" --vibrate 0 %G%
goto :eof
