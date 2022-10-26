@ECHO OFF

Call EI_portenta_h7_camera_dfu-util-suffix_01.bat

CALL:ECHOGRN "***"
CALL:ECHOGRN "Flashing --- --- --- --- --- EI_portenta_h7_camera to M7..."
CALL:ECHOGRN "***"

REM C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1>dfu-util
REM dfu-util 0.10-dev
REM 
REM Copyright 2005-2009 Weston Schmidt, Harald Welte and OpenMoko Inc.
REM Copyright 2010-2021 Tormod Volden and Stefan Schmidt
REM This program is Free Software and has ABSOLUTELY NO WARRANTY
REM Please report bugs to http://sourceforge.net/p/dfu-util/tickets/
REM 
REM You need to specify one of -D or -U
REM Usage: dfu-util [options] ...
REM   -h --help                     Print this help message
REM   -V --version                  Print the version number
REM   -v --verbose                  Print verbose debug statements
REM   -l --list                     List currently attached DFU capable devices
REM   -e --detach                   Detach currently attached DFU capable devices
REM   -E --detach-delay seconds     Time to wait before reopening a device after detach
REM   -d --device <vendor>:<product>[,<vendor_dfu>:<product_dfu>]
REM                                 Specify Vendor/Product ID(s) of DFU device
REM   -n --devnum <dnum>            Match given device number (devnum from --list)
REM   -p --path <bus-port. ... .port>       Specify path to DFU device
REM   -c --cfg <config_nr>          Specify the Configuration of DFU device
REM   -i --intf <intf_nr>           Specify the DFU Interface number
REM   -S --serial <serial_string>[,<serial_string_dfu>]
REM                                 Specify Serial String of DFU device
REM   -a --alt <alt>                Specify the Altsetting of the DFU Interface
REM                                 by name or by number
REM   -t --transfer-size <size>     Specify the number of bytes per USB Transfer
REM   -U --upload <file>            Read firmware from device into <file>
REM   -Z --upload-size <bytes>      Specify the expected upload size in bytes
REM   -D --download <file>          Write firmware from <file> into device
REM   -R --reset                    Issue USB Reset signalling once we're finished
REM   -w --wait                     Wait for device to appear
REM   -s --dfuse-address address<:...>      ST DfuSe mode string, specifying target
REM                                 address for raw file download or upload (not
REM                                 applicable for DfuSe file (.dfu) downloads).
REM                                 Add more DfuSe options separated with ':'
REM                 leave           Leave DFU mode (jump to application)
REM                 mass-erase      Erase the whole device (requires "force")
REM                 unprotect       Erase read protected device (requires "force")
REM                 will-reset      Expect device to reset (e.g. option bytes write)
REM                 force           You really know what you are doing!
REM                 <length>        Length of firmware to upload from device

REM Get the path of the *.ino.bin from the Arduino IDE Output window when it programs the device.

REM Get the serial name and alt name by running
REM 	util.exe --list --verbose
REM 		Found DFU: [2341:035b] ver=0200, devnum=19, cfg=1, intf=0, path="1-4", alt=2, name="@Bootloader version 22 /0x00000000/0*4Kg", serial="0038002A3230510F31303431"
REM 		Found DFU: [2341:035b] ver=0200, devnum=19, cfg=1, intf=0, path="1-4", alt=1, name="@External Flash   /0x90000000/4096*4Kg", serial="0038002A3230510F31303431"
REM 		Found DFU: [2341:035b] ver=0200, devnum=19, cfg=1, intf=0, path="1-4", alt=0, name="@Internal Flash   /0x08000000/01*128Ka,15*128Kg", serial="0038002A3230510F31303431"

REM C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1\dfu-util.exe --device 0x2341:0x025b --download  "C:\Users\mmarc\AppData\Local\Temp\arduino-sketch-4E1ACA146AA7D50BE24821EB7B7A22BC/GetUserInputFromKeyboard.ino.bin" --alt=0 --serial="0038002A3230510F31303431" --dfuse-address=0x08040000:leave --verbose

REM C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1\dfu-util.exe --list --verbose

ECHO Programming "EI_portenta_h7_camera" to M7 <------------------------
"C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1\dfu-util" --device 0x2341:0x035b -D "C:\Users\mmarc\AppData\Local\Temp\arduino-sketch-0F02D06D6ACA4A18DA886FED910D1A92\EI_portenta_h7_camera.ino.bin" --alt=0 --serial="0038002A3230510F31303431" --dfuse-address=0x08040000:leave --verbose
REM ECHO errorlevel %errorlevel%
if errorlevel==74 (
   CALL:ECHOred "Programming Failed. Put Portenta in programming mode by double-clicking its RESET button."
   GOTO flashErr
)
if errorlevel==0 (
   CALL:ECHOGRN "***"
   CALL:ECHOGRN "Flashing --- --- --- --- --- EI_portenta_h7_camera to M7...DONE!"
   CALL:ECHOGRN "***"
   CALL:ECHOgrn "Portenta Programed!"
   GOTO myEnd
)

:flashErr
GOTO myEnd

REM The above command produced (this is the same as what the Arduino IDE produces.):
REM Warning: Invalid DFU suffix signature
REM A valid DFU suffix will be required in a future dfu-util release
REM Opening DFU capable USB device...
REM Device ID 2341:035b
REM Device DFU version 011a
REM Claiming USB DFU Interface...
REM Setting Alternate Interface #0 ...
REM Determining device status...
REM DFU state(2) = dfuIDLE, status(0) = No error condition is present
REM DFU mode device DFU version 011a
REM Device returned transfer size 4096
REM DfuSe interface name: "Internal Flash   "
REM Downloading element to address = 0x08040000, size = 136944
REM Erase           [=========================] 100%       136944 bytes
REM Erase    done.
REM Download        [=========================] 100%       136944 bytes
REM Download done.
REM File downloaded successfully
REM Transitioning to dfuMANIFEST state

:ECHORED
%Windir%\System32\WindowsPowerShell\v1.0\Powershell.exe write-host -foregroundcolor Red %1
EXIT /B

:ECHOGRN
%Windir%\System32\WindowsPowerShell\v1.0\Powershell.exe write-host -foregroundcolor Green %1
EXIT /B

:myEnd
PAUSE