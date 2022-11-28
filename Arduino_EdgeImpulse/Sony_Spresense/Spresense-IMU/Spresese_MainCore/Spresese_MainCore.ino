/*
 ***************************************************************************************
 *  Main.ino - MP Example to boot multiple SubCores                                    *
 *  Copyright 2019 Sony Semiconductor Solutions Corporation                            *
 *                                                                                     *
 *  This library is free software; you can redistribute it and/or                      *
 *  modify it under the terms of the GNU Lesser General Public                         *
 *  License as published by the Free Software Foundation; either                       *
 *  version 2.1 of the License, or (at your option) any later version.                 *
 *                                                                                     *
 *  This library is distributed in the hope that it will be useful,                    * 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                     *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                  *
 *  Lesser General Public License for more details.                                    *
 *                                                                                     *
 *  You should have received a copy of the GNU Lesser General Public                   *
 *  License along with this library; if not, write to the Free Software                *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA         *   
 ***************************************************************************************
 * The MIT License (MIT)                                                               *
 *                                                                                     *
 * Modifications to the Software Rohn Main.ino program                                 *
 * and saved herin as the Software "Spresense_MainCore.ino"                            *
 * Copyright (c) 2022 M. Marcial                                                       *
 * All Rights Reseved                                                                  *
 *                                                                                     *
 * Permission is hereby granted, free of charge, to any person obtaining a copy        *
 * of this software and associated documentation files (the "Software"), to deal       *
 * in the Software without restriction, including without limitation the rights        *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell           *
 * copies of the Software, and to permit persons to whom the Software is               *
 * furnished to do so, subject to the following conditions:                            *
 * The above copyright notice shall be included in                                     *    
 * all copies or substantial portions of the Software.                                 *
 *                                                                                     *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR          *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,            *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE         *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER              *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,       *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE       *
 * SOFTWARE.                                                                           *
 ***************************************************************************************
 */

/* 
  This program is intended to work with "Spresese-Dual-Mode-IMU.ino".
  The purpose of this program is to boot SubCore1 and allow the
  machine learning model deployed from Edge Impulse Studio to make
  inferences on data from the KX224 IMU.
 */
#ifdef SUBCORE
#error "Core selection is wrong! Compile for MainCore."
#endif

#include <MP.h>

/* Define pins ------------------------------------------------------------ */
const int pinOutMainCoreIsUsingI2C    = PIN_D00;  // Output.
const int pinInSubCoreCheckIsI2CFree  = PIN_D01;  // Input.
const int pinOutSubCoreIsUsingI2C     = PIN_D02;  // Output.
const int pinInMainCoreCheckIsI2CFree = PIN_D03;  // Input.

void setup()
{
  // Setup hardware handshaking for I2C bus usage.  
  pinMode(pinOutMainCoreIsUsingI2C,    OUTPUT); // The MainCore sets this pin HIGH while it is using the I2C bus.
  pinMode(pinInSubCoreCheckIsI2CFree,  INPUT);  // This input must be low before the SubCore uses the IC2.
  pinMode(pinOutSubCoreIsUsingI2C,     OUTPUT); // The SubCore sets this pin HIGH while it is using the I2C bus.
  pinMode(pinInMainCoreCheckIsI2CFree, INPUT);  // This input must be low before the MainCore uses the IC2.
  
  digitalWrite(pinOutMainCoreIsUsingI2C,LOW);
  digitalWrite(pinOutSubCoreIsUsingI2C, LOW);
  
  int ret = 0;
  int subidStart = 1;
  int subidEnd   = 1;
  int subid;


  Serial.begin(115200);
  while (!Serial);

  /* Boot SubCore */
  for (subid = subidStart; subid <= subidEnd; subid++) {
    ret = MP.begin(subid);
    if (ret < 0) {
      MPLog("MP.begin(%d) error = %d\n", subid, ret);
    }
  }
}

void loop()
{
  digitalWrite(pinOutMainCoreIsUsingI2C,LOW);
  MPLog("loop\n");
  Serial.println("MainCore...");
  delay(1000);
}

