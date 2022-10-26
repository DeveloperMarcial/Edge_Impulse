@ECHO OFF

CALL:ECHOGRN "***"
CALL:ECHOGRN "Setting DFU Suffix in --- --- --- --- --- EI_portenta_h7_camera..."
CALL:ECHOGRN "***"

REM ***
REM This batch file is for project: EI_portenta_h7_camera
REM ***

ECHO Setting DFU Suffix in "EI_portenta_h7_camera" <------------------------

REM Usage: dfu-suffix [options] ...
REM   -h --help                     		Print this help message
REM   -V --version                  		Print the version number		   
REM   -c --check <file>             		Check DFU suffix of <file>
REM   -a --add <file>               		Add DFU suffix to <file>
REM   -D --delete <file>            		Delete DFU suffix from <file>
REM   -p --pid <productID>  --pid 0x035b    Add product ID into DFU suffix in <file>
REM   -v --vid <vendorID> 	--vid 0x2341    Add vendor ID into DFU suffix in <file>

REM   -d --did <deviceID>           		Add device ID into DFU suffix in <file>
REM											Specify USB device ID (hexadecimal)
REM   -S --spec <specID>            		Add DFU specification ID into DFU suffix in <file>
REM											Specify DFU specification version (hexadecimal)
REM EXIT VALUES
REM 	  0, Success (also if suffix is missing)
REM 	-64, Usage error

REM LIMITATIONS
REM dfu-suffix can not tell a broken DFU suffix (e.g. checksum mismatch) from a non-existing suffix, so only a valid suffix can be removed.

REM --version
REM		dfu-suffix (dfu-util) 0.10-dev

REM --check
REM "C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1\dfu-suffix" --check "C:\Users\mmarc\AppData\Local\Temp\arduino-sketch-0F02D06D6ACA4A18DA886FED910D1A92\EI_portenta_h7_camera.ino.bin"
REM		The file.bin contains a DFU suffix with the following properties:
REM		BCD device:     0xFFFF
REM		Product ID:     0x035B
REM		Vendor ID:      0x2341
REM		BCD DFU:        0x0100
REM		Length:         16
REM		CRC:            0x5DB0E4AD

REM ***
REM Add DFU Suffix
REM ***
"C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1\dfu-suffix" --add "C:\Users\mmarc\AppData\Local\Temp\arduino-sketch-0F02D06D6ACA4A18DA886FED910D1A92\EI_portenta_h7_camera.ino.bin" --vid 0x2341 --pid 0x035b 
if errorlevel==65 (
   CALL:ECHOred "DFU Suffix Already Exists"
   GOTO flashIt
)
if errorlevel==0 (
   CALL:ECHOgrn "DFU Suffix Added"
   GOTO flashIt
)

:flashIt

CALL:ECHOGRN "***"
CALL:ECHOGRN "Setting DFU Suffix in --- --- --- --- --- EI_portenta_h7_camera...DONE!"
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
