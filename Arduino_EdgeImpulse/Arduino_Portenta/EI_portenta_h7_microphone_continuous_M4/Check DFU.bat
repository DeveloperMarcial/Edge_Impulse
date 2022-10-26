@ECHO OFF

REM "C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1\dfu-suffix" --help

REM DFU suffix with the following properties:
REM BCD device:     0xFFFF
REM Product ID:     0x035B
REM Vendor ID:      0x2341
REM BCD DFU:        0x0100
REM Length:         16
REM CRC:            0xA19ED15D
REM
REM UNCview.exe shows:
REM           ===>Device Descriptor<===
REM bLength:                           0x12
REM bDescriptorType:                   0x01
REM bcdUSB:                          0x0200
REM bDeviceClass:                      0xEF  -> This is a Multi-interface Function Code REM Device
REM bDeviceSubClass:                   0x02  -> This is the Common Class Sub Class
REM bDeviceProtocol:                   0x01  -> This is the Interface Association Descriptor protocol
REM bMaxPacketSize0:                   0x40 = (64) Bytes
REM idVendor:                        0x2341 = Vendor ID not listed with USB.org as of 03-19-2008
REM idProduct:                       0x025B
REM bcdDevice:                       0x0101
REM iManufacturer:                     0x01
REM      English (United States)  "Arduino"
REM iProduct:                          0x02
REM      English (United States)  "Envie M7"
REM iSerialNumber:                     0x03
REM      English (United States)  "0038002A3230510F31303431"
REM bNumConfigurations:                0x01

"C:\Users\mmarc\AppData\Local\Arduino15\packages\arduino\tools\dfu-util\0.10.0-arduino1\dfu-suffix" --check "C:\Users\mmarc\AppData\Local\Temp\arduino-sketch-6CDB4E66CE703700836072B0BA5E6942\EI_portenta_h7_microphone_continuous_M4.ino.bin"
PAUSE