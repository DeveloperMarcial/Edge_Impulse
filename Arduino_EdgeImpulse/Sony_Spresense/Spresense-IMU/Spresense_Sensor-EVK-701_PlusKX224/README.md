This INO file can read and display values from the
`Sony Spresense Sensor EVK 701` sensor board and the `KX224` sensor board.

All sensors can be read within the loop() without conflicts between sensors.

The `Sony Spresense Sensor EVK 701` sensor board includes 4 sensors:
* Chip KX126 with a 3-axis IMU sensor.
* Chip BM1383AGLV with a barometric pressure & tempeture sensors.
* Chip BM1422AGMV with a 3-axis magnetometer.

The `KX224` sensor board includes one sensor:
* Chip KX224 with a 3-axis IMU.

Both boards communicate over I2C.

The following libraries will be required:
* Wire.h
* KX126.h
* KX224_I2C.h
* BM1383AGLV.h
* BM1422AGMV.h

The manufacturer of theese sensors called Rohm has an Arduino library [here](https://github.com/RohmSemiconductor/Arduino).
