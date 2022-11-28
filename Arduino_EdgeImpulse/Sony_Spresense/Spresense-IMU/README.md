* Folder `Arduino Libraries` are folders to be copied into the Arduino library folder `C:\Users\<userName>\Documents\Arduino\libraries` so that an Arduino program can implement the KX126 and KX224 IMUs.
* Folder `Spresense-Dual-Model-IMU` is an attempt to run an Edge Impulse machine learning Model using the KX126 data on the MainCore and on SubCore1 run a Model using data from the KX224 IMU.
  * The MainCore successfully runs the KX126 model.
  * The SubCore1 successfully runs the KX224 model.
  * **Both models cannot be run in parallel on seperate Sony Spresense cores.**
* Folder `Spresense_Sensor-EVK-701_PlusKX224` is a Arduino program that shows how to gather data from the KX126 and KX224 within the same program.  There is no Model in use here. This program just makes sure the sensors are working.
* Folder `Spresese_MainCore` is used in conjunction with `Spresense-Dual-Model-IMU`.
