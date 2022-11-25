/*****************************************************************************
  KX224_I2C.ino

 Copyright (c) 2017 ROHM Co.,Ltd.

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
******************************************************************************/
#include <Wire.h>
#include <KX224_I2C.h>

KX224 kx224(KX224_DEVICE_ADDRESS_1E);

void setup() {
  byte rc;

  Serial.begin(9600);
  while (!Serial);

  Wire.begin();

  rc = kx224.init();
  if (rc != 0) {
    Serial.println(F("KX224 initialization failed"));
    Serial.flush();
  }
}

void loop() {
  byte rc;
  float acc[3];

  rc = kx224.get_val(acc);
  if (rc == 0) {
    Serial.write("KX224 (X) = ");
    Serial.print(acc[0]);
    Serial.println(" [g]");
    Serial.write("KX224 (Y) = ");
    Serial.print(acc[1]);
    Serial.println(" [g]");
    Serial.write("KX224 (Z) = ");
    Serial.print(acc[2]);
    Serial.println(" [g]");
    Serial.println();
  }
 
  delay(500);

}
