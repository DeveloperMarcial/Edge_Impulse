An example senerio for this app would be to monitor soil moisture at various locations in a cherry orchard were power and internet connectivity is not avaliable. The moisture data is displyed on a Dashboard [here](https://thingspeak.com/channels/1977008).

This program is targeted to the Sony Spresense using the [Arduino](https://www.arduino.cc/) IDE for compilation. There are different INO files to make it easy to adapt this program for other uses. For example, if you don't want to use LoRaWAN, the delete files `Spresense-LMiC.h` and `Spresense-LMiC.ino` from the Arduino build folder.

This project uses the following hardware:
* [Sony Spresense](https://developer.sony.com/develop/spresense/) (has built-in GNSS module).
* Seeed Studio [moisture gauge](https://www.seeedstudio.com/Grove-Moisture-Sensor.html).
* Seeed Studio [WIO Terminal](https://www.seeedstudio.com/Wio-Terminal-p-4509.html) (for displaying status when out roaming about).
* Adafruit [LoRa radio board](https://www.adafruit.com/product/3072).
* The Things Network (TTN) [LoRa to Internet gateway](https://www.adafruit.com/product/4345).
* An Internet router.
* An ISP to get you on the internet.
* A free account with [TTN](https://www.thethingsnetwork.org).
  * Need to register your gateway with TTN.
  * Need to create an App in the TTN Console to recieve the data from your Spresense driven LoRa radio.
* A free account with Mathworks on their [ThingsSpeak](https://thingspeak.com/) web service for publicly or privately displaying data from your Spresense.

![image](https://user-images.githubusercontent.com/92460732/208208272-942ef7dd-7aae-4485-bae2-505d226ac7a7.png)

|Filename|Purpose|
| --- | --- |
|LoRa Radio.md|The LoRa radio board used in this project.|
|LoRaWAN_GNSS.[drawio](https://app.diagrams.net/)|The file for the block diagram shown below.|
|README.md|This file.|
|Spresense-EdgeImpulde.ino|The subroutines needed to run the Edge Impulse machine learning mode.|
|Spresense-EdgeImpulse.h|Edge Impulse specific global `defines`, `includes`, and `variables` used in the main INO file.|
|Spresense-GNSS.h|Sony Spresense specific global `defines`, `includes`, and `variables` used in the main INO file.|
|Spresense-GNSS.ino|The subroutines needed to interface with the Spresense onboard GNSS module.|
|Spresense-LMiC.h|MCCI specific global `defines`, `includes`, and `variables` used in the main INO file.|
|Spresense-LMiC.ino|The arduino-lorawan library provides a structured way of using the arduino-lmic library to send sensor data over The Things Network or a similar LoRaWAN-based data network.|
|Spresense-Moisture_Sensor_Model_LoRaWAN_GNSS.ino|The main Arduino INO file with `setup()` and `loo()`.|
|TTN_IDsAndKeys.h|This file holds my personal AppEUI or JoinEUI, DEVEUI and App API key. This file should noty exist in this GitHub repository.|
|YourTTN_IDsAndKeys.h|This file holds your personal AppEUI or JoinEUI, DEVEUI and App API key. Modify this file with your TTN EUIs and key.|
|ei-soil-moisture-with-lora-arduino-1.0.3.zip|The Arduino custom machine learning model delpoyed from the [Edge Impulse Studio](https://www.edgeimpulse.com/).|

TODO:

* Put the Spresense into low power or sleep mode, waking up once an hour to x-mit a new value over LoRa.
* Depending on what enclosure is selected and what battery is selected consider using a [SuperCap](https://store.mcci.com/products/model-4906) for added amps needed during a LoRa x-mission
