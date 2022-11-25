The Spresense can be outfitted with an IMU called the (Kx224)[https://www.kionix.com/product/KX224-1053]. A company Rohm make a circuit board KX224-I2C that plugs into their *Sensor Shield*. The *Sensor Shield* has an Arduino Uno compatiable pinout.
https://www.farnell.com/datasheets/2630940.pdf?_ga=2.137977912.964043680.1538082932-1150069825.1507669889

The current *Sony Spresese Edge Impulse Firmware* is compatiable with the Kx126 IMU.

To get the Kx224 working with *Edge Impulse Studio* execute the following:
* Download the (Sony Spresese Edge Impulse Firmware)[https://github.com/edgeimpulse/firmware-sony-spresense].
  * For these instructions the `root` folder is called `trunk`
* In the root folder modify `trunk/main.cpp`
  * `#include "KX126.h"						// Where you see this...`
  * `#include "EI_KX224_I2C.h"  			// ...add this.`
  * `KX126 kx126(KX126_DEVICE_ADDRESS_1F);	// Where you see this...`
  * `KX224 kx224(KX224_DEVICE_ADDRESS_1E);  // ...add this.`
  * `return (int)kx126.get_val(acc_val);	// Where you see this,`
  * `//return (int)kx126.get_val(acc_val);	// then rem it out...`
    `return (int)kx224.get_val(acc_val); 	// ...and add this.`
											// Of course this code should handle any case kx122, kx126 or kx224 by using "WHO_AM_I".`
* Add these files to `trunk/libraries/KXxx`
  * EI_KX224_I2C.cpp
  * EI_KX224_I2C.h
* Then following the instructions (here)[https://github.com/edgeimpulse/firmware-sony-spresense]:
  * make -j
  * make flash
  * edge-impulse-daemon 

Notes on installing the *Sony Spresese Edge Impulse Firmware* using *Ubuntu v22.04*
	* The aforementioned Edge Impulse firmware is (here)[https://github.com/edgeimpulse/firmware-sony-spresense].
	* The README.md file states:
	  * Install: `GNU ARM Embedded Toolchain 9-2019-q4-major` - make sure arm-none-eabi-gcc is in your PATH.
	    * Yes, install this even though `9-2019-q4-major` is deprecated.
		* If you plow ahead and install the most recent toolchain, `arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi.tar.xz` the firmware will not compile.
