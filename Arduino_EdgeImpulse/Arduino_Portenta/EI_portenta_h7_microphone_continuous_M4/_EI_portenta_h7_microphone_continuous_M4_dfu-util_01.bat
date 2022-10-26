@ECHO OFF

CALL:ECHOGRN "***"
CALL:ECHOGRN "Flashing --- --- --- --- --- EI_portenta_h7_microphone_continuous_M4 to M4..."
CALL:ECHOGRN "***"

REM ***
REM Add DFU Suffix
REM ***
"C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1\dfu-suffix" --add "C:\Users\mmarc\AppData\Local\Temp\arduino-sketch-6CDB4E66CE703700836072B0BA5E6942\EI_portenta_h7_microphone_continuous_M4.ino.bin" --vid 0x2341 --pid 0x035b 
rem ECHO errorlevel %errorlevel%
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
REM Flash M4
REM ***
"C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1\dfu-util" --device 0x2341:0x035b -D "C:\Users\mmarc\AppData\Local\Temp\arduino-sketch-6CDB4E66CE703700836072B0BA5E6942\EI_portenta_h7_microphone_continuous_M4.ino.bin" --alt=0 --serial="0038002A3230510F31303431" --dfuse-address=0x08100000:leave --verbose
REM ECHO errorlevel %errorlevel%
if errorlevel==74 (
   CALL:ECHOred "Programming Failed. Put Portenta in programming mode by double-clicking its RESET button."
   GOTO flashErr
)
if errorlevel==0 (
   CALL:ECHOGRN "***"
   CALL:ECHOGRN "Flashing --- --- --- --- --- EI_portenta_h7_microphone_continuous_M4 to M4...Done!"
   CALL:ECHOGRN "***"
   CALL:ECHOgrn "Portenta Programed!"
   GOTO myEnd
)

:flashErr

CALL:ECHOGRN "***"
CALL:ECHOGRN "Flashing --- --- --- --- --- EI_portenta_h7_microphone_continuous_M4 to M4...DONE!"
CALL:ECHOGRN "***"

GOTO myEnd

:ECHORED
%Windir%\System32\WindowsPowerShell\v1.0\Powershell.exe write-host -foregroundcolor Red %1
EXIT /B

:ECHOGRN
%Windir%\System32\WindowsPowerShell\v1.0\Powershell.exe write-host -foregroundcolor Green %1
EXIT /B

:myEnd
PAUSE