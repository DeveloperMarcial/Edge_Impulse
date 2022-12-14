The *Spresense* can be outfitted with an IMU called the [KX224](https://www.kionix.com/product/KX224-1053). The company *Rohm* make a circuit board `KX224-I2C` that plugs into their *Sensor Shield*. The *Rohm Sensor Shield* has an *Arduino Uno* compatiable pinout.

`Photo of Rohm Sensor Shield`:

![image](https://user-images.githubusercontent.com/92460732/203922775-56c120aa-488a-4c54-96ca-fcccfa726019.png)

Photo of the `KX224-I2C PCB` (circled in blue) plugged into the `Rohm Sensor Shield`:

![image](https://user-images.githubusercontent.com/92460732/203921546-5c068873-12ef-4ac1-8848-b8343dbe0a48.png)

Make sure to set the J15 jumper (lower right in above photo) on the `Rohm Sensor Shield` to the **3.3V** position as the `KX224 Sensor Board` requires 3.3V. (Note: The voltage on the *Arduino* header will bet 5V and 3.3V ![image](https://user-images.githubusercontent.com/92460732/204110604-a53cd2db-9a3e-43e7-b687-75e6162c46bb.png)
 and is not affected by the J15 jumper placement.

This photo shows the [Sony Spresense Main Board - CXD5602](https://www.adafruit.com/product/4419?gclid=CjwKCAiAyfybBhBKEiwAgtB7fnBQHFhblyGEwmhzojUOQMwh49nrxt_EtRr14POoRobWu1Kt64K0sBoCQaEQAvD_BwE) mounted on the [Sony Spresense Extension Board](https://www.adafruit.com/product/4418). The `Rohm Sensor Shield` plugs into the `Sony Spresense Extension Board`.

![image](https://user-images.githubusercontent.com/92460732/203921680-99a83265-0679-4942-8183-ff2327dee265.png)

* [Other Rohm Sensors](https://www.rohm.com/news-detail?news-title=new-sensor-shield-expansion-board&defaultGroupId=false)
* [KX224 Datasheet](https://www.farnell.com/datasheets/2630940.pdf?_ga=2.137977912.964043680.1538082932-1150069825.1507669889)
* [At least one place you can buy it.](https://www.mouser.com/ProductDetail/ROHM-Semiconductor/SENSORSHIELD-EVK-003?qs=wUXugUrL1qz9s9mzyCT18w%3D%3D&gclid=CjwKCAiAyfybBhBKEiwAgtB7fmcR2woWYy7FHlzvMeI8fIAjwQbEIjdbz0PMI0heFxF-g14Esb6nNxoCEn4QAvD_BwE)
* The [KX224 chip](https://www.kionix.com/product/KX224-1053) that makes it work.

<HR>

To get the **KX224** IMU working with *Edge Impulse Studio* execute the following:
* Download the [Sony Spresese Edge Impulse Firmware](https://github.com/edgeimpulse/firmware-sony-spresense) herin called *SSEI_Firmware*.
  * For these instructions the `root` folder of the SSEI_Firmware is called `trunk`.
* In the root folder modify `trunk/main.cpp`
  * `#include "KX126.h"                         // Where you see this...`
  * `#include "EI_KX224_I2C.h"                  // ...add this.`
  * `KX126 kx126(KX126_DEVICE_ADDRESS_1F);      // Where you see this...`
  * `KX224 kx224(KX224_DEVICE_ADDRESS_1E);      // ...add this.`
  * `return (int)kx126.get_val(acc_val);        // Where you see this,`
  * `//return (int)kx126.get_val(acc_val);      // then rem it out...`
  * `return (int)kx224.get_val(acc_val);        // ...and add this.`
  * `                                           // Of course this code should handle any case kx122, kx126 or kx224 by using "WHO_AM_I".`
* Add these files to `trunk/libraries/KXxx`
  * EI_KX224_I2C.cpp
  * EI_KX224_I2C.h

* If you have deployed an Edge Impulse C++ library with you custom motions, then copy the 3-folders in that library overtop of the existing folders of the same name in the SSEI_Firmware.
* The 3-folders to copy are:
  * edge-impulse-sdk
  * model-parameters
  * tflite-model

* Then following the instructions [here](https://github.com/edgeimpulse/firmware-sony-spresense), aka:
  * make -j
  * make flash
  * edge-impulse-daemon 
  * Start recording new Samples in Edge Impulse Studio.

<HR>
 
Notes on installing the *Sony Spresese Edge Impulse Firmware* using *Ubuntu v22.04*:
* The aforementioned *Edge Impulse Firmware* is [here](https://github.com/edgeimpulse/firmware-sony-spresense).
* The `README.md` file states:
  * Install: `GNU ARM Embedded Toolchain 9-2019-q4-major - make sure arm-none-eabi-gcc is in your PATH`.
    * Yes, yes, install this even though `9-2019-q4-major` *is deprecated*.
    * If you plow ahead and install the most recent toolchain, such as `arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi.tar.xz` the firmware will not compile.
    
<HR>

The current *Sony Spresese Edge Impulse Firmware* is compatiable with the **KX126** IMU. The **KX126** IMU plug in directly to the Spresense MCU board. (The KX224 sensor board can plug into the KX126 as desired.)

Photo of **KX126** IMU:

![image](https://user-images.githubusercontent.com/92460732/203925582-caeb6209-0707-43ea-94be-a81e7adab87c.png)
