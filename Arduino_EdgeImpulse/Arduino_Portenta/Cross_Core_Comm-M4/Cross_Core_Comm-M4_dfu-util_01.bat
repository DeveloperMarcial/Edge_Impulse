@ECHO OFF

CALL:ECHOGRN "***"
CALL:ECHOGRN "Flashing --- --- --- --- --- Cross_Core_Comm-M4 to M4..."
CALL:ECHOGRN "***"

REM ***
REM Add DFU Suffix
REM ***

"C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1\dfu-suffix" --add "C:\Users\mmarc\AppData\Local\Temp\arduino-sketch-0BEFB2F5C8FC89C90C4479D1C6674FE4\Cross_Core_Comm-M4.ino.bin" --vid 0x2341 --pid 0x035b 
if errorlevel==65 (
   CALL:ECHOred "DFU Suffix Already Exists"
   GOTO flashIt
)
if errorlevel==0 (
   CALL:ECHOgrn "DFU Suffix Added"
   GOTO flashIt
)

:flashIt


REM ***
REM ECHO Programming "Cross_Core_Comm-M4" to M7-M7-M7-M7-M7-M7-M7
REM ***
REM "C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1\dfu-util" --device 0x2341:0x035b -D "C:\Users\mmarc\AppData\Local\Temp\arduino-sketch-0BEFB2F5C8FC89C90C4479D1C6674FE4\Cross_Core_Comm-M4.ino.bin" --alt=0 --serial="0038002A3230510F31303431" --dfuse-address=0x08040000:leave --verbose
REM ECHO errorlevel %errorlevel%
REM if errorlevel==74 (
REM    CALL:ECHOred "Programming Failed. Put Portenta in programming mode by double-clicking its RESET button."
REM    GOTO flashErr
REM )
REM if errorlevel==0 (
REM CALL:ECHOgrn "Portenta Programed!"
REM CALL:ECHOGRN "***"
REM CALL:ECHOGRN "Flashing --- --- --- --- --- Cross_Core_Comm-M4 to M7..."
REM CALL:ECHOGRN "***"
REM 
REM GOTO flashErr
)

REM ***
ECHO Programming "Cross_Core_Comm-M4" to M4-M4-M4-M4-M4-M4-M4-M4
REM ***
"C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1\dfu-util" --device 0x2341:0x035b -D "C:\Users\mmarc\AppData\Local\Temp\arduino-sketch-0BEFB2F5C8FC89C90C4479D1C6674FE4\Cross_Core_Comm-M4.ino.bin" --alt=0 --serial="0038002A3230510F31303431" --dfuse-address=0x08100000:leave --verbose
REM ECHO errorlevel %errorlevel%
if errorlevel==74 (
   CALL:ECHOred "Programming Failed. Put Portenta in programming mode by double-clicking its RESET button."
   GOTO flashErr
)
if errorlevel==0 (
   CALL:ECHOGRN "***"
   CALL:ECHOGRN "Flashing --- --- --- --- --- Cross_Core_Comm-M4 to M4...DONE!"
   CALL:ECHOGRN "***"
   CALL:ECHOgrn "Portenta Programed!"
   GOTO myEnd
)

:flashErr

GOTO myEnd

:ECHORED
%Windir%\System32\WindowsPowerShell\v1.0\Powershell.exe write-host -foregroundcolor Red %1
EXIT /B

:ECHOGRN
%Windir%\System32\WindowsPowerShell\v1.0\Powershell.exe write-host -foregroundcolor Green %1
EXIT /B

:myEnd

PAUSE
