/*****************************************************************************
  KX126_I2C.ino

 Copyright (c) 2018 ROHM Co.,Ltd.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
  ************************************************************************************
 * The MIT License (MIT)                                                            *
 *                                                                                  *
 * Modifications to the Software "KX126.ino, BM1383AGLV.ino, BM1422AGMV.ino         *
 * and KX224_I2C.ino"                                                               *
 * and saved herin as the Software "KX126_KX224.ino"                                *
 * Copyright (c) 2022 M. Marcial                                                    *
 *                                                                                  *
 * Permission is hereby granted, free of charge, to any person obtaining a copy     *
 * of this software and associated documentation files (the "Software"), to deal    *
 * in the Software without restriction, including without limitation the rights     *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
 * copies of the Software, and to permit persons to whom the Software is            *
 * furnished to do so, subject to the following conditions:                         *
 * The above copyright notice shall be included in                                  *    
 * all copies or substantial portions of the Software.                              *
 *                                                                                  *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR       *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,         *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER           *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,    *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE    *
 * SOFTWARE.                                                                        *
 ************************************************************************************
******************************************************************************/
// Use 3-axis IMU.
//#define IMU_126
#define IMU_224
// Use Barometric pressure & tempeture.
//#define BAR
// Use 3-axis magnetometer.
//#define MAG

#include <Wire.h>
#include <KX126.h>                                    // 3-axis IMU.
#include <KX224_I2C.h>                                // 3-axis IMU.
#include <BM1383AGLV.h>                               // Barometric pressure & tempeture.
#include <BM1422AGMV.h>                               // 3-axis magnetometer.

#ifdef IMU_126
KX126 KX126(KX126_DEVICE_ADDRESS_1F);                 // 3-axis IMU from KX126.
#endif
#ifdef IMU_224
KX224 KX224(0x1E);                                    // 3-axis IMU from KX224.
#endif

BM1383AGLV bm1383aglv;                                // Barometric pressure & tempeture from BM1383AGLV.
BM1422AGMV bm1422agmv(BM1422AGMV_DEVICE_ADDRESS_0F);  // 3-axis magnetometer from BM1422AGMV.

//KX126_WHO_AMI      Register Value = 0x38
//BM1383GL ID        Register Value = 0x32
//BM1422AGMV_WHO_AMI Register Value = 0x41

void setup()
{
  byte rc;

  // Start serial port for showing status.
  Serial.begin(115200);
  while (!Serial);

  // Begin I2C bus.
  Wire.begin();

  #ifdef IMU_126
  // Initialize 3-axis IMU from KX126.
  rc = KX126.init();
  if (rc != 0)
  {
    Serial.println("KX126 initialization failed");
    Serial.flush();
  }
  #endif
  #ifdef IMU_224
  // Initialize 3-axis IMU from KX224.
  rc = KX224.init();
  if (rc != 0)
  {
    Serial.println("KX224 initialization failed");
    Serial.flush();
  }
  #endif

  #ifdef BAR
  // Initialize barometric pressure & tempeture from BM1383AGLV.
  rc = bm1383aglv.init();
  if (rc != 0) {
    Serial.println("BM1383AGLV initialization failed");
    Serial.flush();
  }
  #endif

  #ifdef MAG
  // Initialize // 3-axis magnetometer from BM1422AGMV.
  rc = bm1422agmv.init();
  if (rc != 0) {
    Serial.println(F("BM1422AGMV initialization failed"));
    Serial.flush();
  }
  #endif
}

void loop()
{
  byte rc;
  float acc[3];               // 3-axis IMU values from KX126.
  float press = 0;            // Barometric pressure from BM1383AGLV.
  float temp = 0;             // Temperature from BM1383AGLV.
  float mag[3];               // 3-axis magnetometer from BM1422AGMV.

  #ifdef IMU_126
  // Get 3-axis IMU values from KX126.
  rc = KX126.get_val(acc);
  if (rc == 0)
  {
    // Format for Serial Plotter:
    Serial.print("x:");
    Serial.print(acc[0]);
    Serial.print("\ty:");
    Serial.print(acc[1]);
    Serial.print("\tz=:");
    Serial.println(acc[2]);
    /*
    // Format for Termial:
    Serial.write("KX126 (X) = ");
    Serial.print(acc[0]);
    Serial.println(" [g]");
    Serial.write("KX126 (Y) = ");
    Serial.print(acc[1]);
    Serial.println(" [g]");
    Serial.write("KX126 (Z) = ");
    Serial.print(acc[2]);
    Serial.println(" [g]");
    Serial.println();
    */
  }
  #endif
  #ifdef IMU_224
  // Get 3-axis IMU values from KX224.
  rc = KX224.get_val(acc);
  if (rc == 0)
  {
    // Format for Serial Plotter:
    Serial.print("x:");
    Serial.print(acc[0]);
    Serial.print("\ty:");
    Serial.print(acc[1]);
    Serial.print("\tz=:");
    Serial.println(acc[2]);
  }
  #endif

  #ifdef BAR
  // Get barometric pressure & tempeture from BM1383AGLV.
  rc = bm1383aglv.get_val(&press, &temp);
  if (rc == 0) {
    Serial.print("BM1383AGLV (pressure) = ");
    Serial.print(press);
    Serial.println(" [hPa]");
    Serial.print("BM1383AGLV (temperature) =  ");
    Serial.print(temp);
    Serial.println(" [degrees Celsius]");    
    Serial.println();
  }
  #endif

  #ifdef MAG
  // Get 3-axis magnetometer from BM1422AGMV.
  rc = bm1422agmv.get_val(mag);

  if (rc == 0) {
    Serial.print("BM1422AGMV XDATA=");
    Serial.print(mag[0], 3);
    Serial.println("[uT]");
    Serial.print("BM1422AGMV YDATA=");
    Serial.print(mag[1], 3);
    Serial.println("[uT]");
    Serial.print("BM1422AGMV ZDATA=");
    Serial.print(mag[2], 3);
    Serial.println("[uT]");
    Serial.println();    
  }
  #endif
 
  delay(1000);

}
