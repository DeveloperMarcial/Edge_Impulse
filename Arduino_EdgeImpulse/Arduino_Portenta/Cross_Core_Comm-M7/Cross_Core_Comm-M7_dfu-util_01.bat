@ECHO OFF
CALL:ECHOGRN "***"
CALL:ECHOGRN "Flashing --- --- --- --- --- Cross_Core_Comm-M7 to M7..."
CALL:ECHOGRN "***"

REM ***
REM Add DFU Suffix
REM ***
"C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1\dfu-suffix" --add "C:\Users\mmarc\AppData\Local\Temp\arduino-sketch-18D6E906CB12455DF6C233C2B3917CDF\Cross_Core_Comm-M7.ino.bin" --vid 0x2341 --pid 0x035b 
if errorlevel==65 (
   CALL:ECHOred "DFU Suffix Already Exists"
   GOTO flashIt
)
if errorlevel==0 (
   CALL:ECHOgrn "DFU Suffix Added"
   GOTO flashIt
)

:flashIt

ECHO Programming "Cross_Core_Comm-M7" to M7 <------------------------
"C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1\dfu-util" --device 0x2341:0x035b -D "C:\Users\mmarc\AppData\Local\Temp\arduino-sketch-18D6E906CB12455DF6C233C2B3917CDF\Cross_Core_Comm-M7.ino.bin" --alt=0 --serial="0038002A3230510F31303431" --dfuse-address=0x08040000:leave --verbose
REM ECHO errorlevel %errorlevel%
if errorlevel==74 (
   CALL:ECHOred "Programming Failed. Put Portenta in programming mode by double-clicking its RESET button."
   GOTO flashErr
)
if errorlevel==0 (
   CALL:ECHOGRN "***"
   CALL:ECHOGRN "Flashing --- --- --- --- --- Cross_Core_Comm-M7 to M7...DONE!"
   CALL:ECHOGRN "***"
   CALL:ECHOgrn "Portenta Programed!"
   GOTO myEnd
)

:flashErr

REM ECHO Programming "Cross_Core_Comm-M7" to M4 <------------------------
REM "C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1\dfu-util" --device 0x2341:0x035b -D "C:\Users\mmarc\AppData\Local\Temp\arduino-sketch-18D6E906CB12455DF6C233C2B3917CDF\Cross_Core_Comm-M7.ino.bin" --alt=0 --serial="0038002A3230510F31303431" --dfuse-address=0x08100000:leave --verbose
REM ECHO errorlevel %errorlevel%
REM if errorlevel==74 (
REM    CALL:ECHOred "Programming Failed. Put Portenta in programming mode by double-clicking its RESET button."
REM    GOTO flashErr
REM )
REM if errorlevel==0 (
REM    CALL:ECHOGRN "***"
REM    CALL:ECHOGRN "Flashing --- --- --- --- --- Cross_Core_Comm-M4 to M4...DONE!"
REM    CALL:ECHOGRN "***"
REM    CALL:ECHOgrn "Portenta Programed!"
REM    GOTO flashIt
REM )

GOTO myEnd

:ECHORED
%Windir%\System32\WindowsPowerShell\v1.0\Powershell.exe write-host -foregroundcolor Red %1
EXIT /B

:ECHOGRN
%Windir%\System32\WindowsPowerShell\v1.0\Powershell.exe write-host -foregroundcolor Green %1
EXIT /B

:myEnd

PAUSE