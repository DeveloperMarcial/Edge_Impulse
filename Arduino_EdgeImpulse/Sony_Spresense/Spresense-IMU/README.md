**Hardware**
* The `KX224_I2C` PCB looks like this:

![image](https://user-images.githubusercontent.com/92460732/203920280-c92e735e-c343-4eea-b6d7-780928885a2d.png)

<HR>

**Edge Impulse Usage**
* Folder `KX224_I2C` is for use with [Sony Spresese Edge Impulse Firmware](https://github.com/edgeimpulse/firmware-sony-spresense).
* See [this](https://github.com/DeveloperMarcial/Edge_Impulse/blob/trunk/Arduino_EdgeImpulse/Sony_Spresense/Spresense-IMU/EI_KX224_I2C/README.md) to see how to mod the Edge Impulse Firmware so it will operate with the KX224.
 
 <HR>
 
**Arduino Usage**
* Folder `KX224_I2C` is for use with *Arduino*.
* Copy folder `KX224_I2C` to `C:\...\Documents\Arduino\libraries\KX224_I2C`
* On my Win10 it is folder `C:\Users\<userName>\OneDrive\Documents\Arduino\libraries\KX224_I2C`
* And then in your INO file:
  * `#include <Wire.h>`
  * `#include <KX224_I2C.h>`

