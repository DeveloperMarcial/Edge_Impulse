**Project Description**

This project reads the analog voltage of a moisture sensor. A machine learning model was developed that predicts if the soil in a houseplant is dry or wet. The predictions are made on a small microcontroller unit (MCU) running near the plant. The results of the machine learning inference are sent over LoRa to an LoRaWAN internet connected gateway.  The gateway is configured to send data to *The Things Network* (TTN). A custom application in my TTN Console was created to recieve the data package created on the Sprecense MCU. In the TTN application a webhook was configured to send data to *ThingsSpeak* for charting and public display. (Since I don't have a Sony Spresense I can dedicate to daily logging to ThingsSpeak, take a look at this example of what can be done: https://thingspeak.com/channels/1623648)

| Filename                                    | Description                                 |             
| ------------------------------------------- | ------------------------------------------- |
| LoRa Radio.md                               | Info on the LoRa radio used in this project.| 
| Spresense-Moisture_Sensor_Model_LoRaWAN.ino | The Arduino file |
| YourTTN_IDsAndKeys.h                        | Edit this file with your "The Things Network" private DeviceID, JoinID or AppID, and API Key.|
| ei-soil-moisture-with-lora-arduino-1.0.3.zip| The Edge Impulse created Machine Learning model.| 

![image](https://user-images.githubusercontent.com/92460732/207221802-a3386302-7ae3-4bad-8b87-de045e01f95d.png)

