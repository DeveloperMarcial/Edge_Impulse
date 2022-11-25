* Folder `KX224_I2C` is for use with *Arduino*.
* Copy folder `KX224_I2C` to `C:\...\Documents\Arduino\libraries\KX224_I2C`
* On my Win10 it is folder `C:\Users\<userName>\OneDrive\Documents\Arduino\libraries\KX224_I2C`
* And then in your INO file:
  * #include <Wire.h>
  * #include <KX224_I2C.h>

* Folder `KX224_I2C` is for use with (Sony Spresese Edge Impulse Firmware)[https://github.com/edgeimpulse/firmware-sony-spresense].
* Download the Firmware from GitHub.
  * For these instructions the `root` folder is called `trunk`
* Add these files to `trunk/libraries/KXxx`
  * EI_KX224_I2C.cpp
  * EI_KX224_I2C.h
* Configure the Spresense MCU hardware with the KX224 and power it up.
* Then following the instructions (here)[https://github.com/edgeimpulse/firmware-sony-spresense]:
  * make -j
  * make flash
  * edge-impulse-daemon
  * Start recording new Samples.
