/***
 ************************************************************************************
 * Code for Machine Learning taken from: https://www.edgeimpulse.com/               *
 *                                                                                  *
 * Edge Impulse ingestion SDK                                                       *
 * Copyright (c) 2022 EdgeImpulse Inc.                                              *
 *                                                                                  * 
 * Licensed under the Apache License, Version 2.0 (the "License");                  *    
 * you may not use this file except in compliance with the License.                 *
 * You may obtain a copy of the License at                                          *
 * http://www.apache.org/licenses/LICENSE-2.0                                       *
 *                                                                                  *
 * Unless required by applicable law or agreed to in writing, software              *
 * distributed under the License is distributed on an "AS IS" BASIS,                *  
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.         *  
 * See the License for the specific language governing permissions and              *
 * limitations under the License.                                                   *
 ************************************************************************************
 * Code for SNR and RSSI taken from https://github.com/lnlp/LMIC-node and           *  
 * has the following license                                                        *
 *                                                                                  *
 * MIT License                                                                      * 
 *                                                                                  * 
 * Copyright (c) 2021 Leonel Lopes Parente                                          *  
 * Copyright (c) 2018 Terry Moore, MCCI                                             *
 * Copyright (c) 2015 Thomas Telkamp and Matthijs Kooijman                          *
 *                                                                                  *  
 * Permission is hereby granted, free of charge, to any person obtaining a copy     *
 * of this software and associated documentation files (the "Software"), to deal    *
 * in the Software without restriction, including without limitation the rights     *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
 * copies of the Software, and to permit persons to whom the Software is            *
 * furnished to do so, subject to the following conditions:                         *
 *                                                                                  *
 * The above copyright notice and this permission notice shall be included in all   *
 * copies or substantial portions of the Software.                                  *
 *                                                                                  *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR       *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,         *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      *  
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER           *  
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,    *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE    *
 * SOFTWARE.                                                                        *
 ************************************************************************************
 * The MIT License (MIT)                                                            *
 *                                                                                  *
 * The machine learning model was based on the Edge Impulse "static buffer.ino"     *
 * and incorporated herein. The copyright for Edge Impulse is shown above           *
 * entitled "Edge Impulse ingestion SDK" was found in the "static buffer.ino".      *
 *                                                                                  *
 * This software "Spresense-Moisture_Sensor_Model_LoRaWAN.ino"                      *
 * Copyright (c) 2022 M. Marcial                                                    *
 * All Rights Reserved                                                              *
 * If you use any of this code please give me credit.                               *
 *                                                                                  *
 * Permission is hereby granted, free of charge, to any person obtaining a copy     *
 * of this software and associated documentation files (the "Software"), to deal    *
 * in the Software without restriction, including without limitation the rights     *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
 * copies of the Software, and to permit persons to whom the Software is            *
 * furnished to do so, subject to the following conditions:                         *
 *                                                                                  *
 * The above license and copyright notice shall be included in                      *
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
 ***/

/***********************************************************************************/
/********** Includes ***************************************************************/
/***********************************************************************************/
#include <SPI.h>                                  // SPI 4-wire bus.
                                                  // SPI = Serial Peripheral Interface
                                                  // See https://en.wikipedia.org/wiki/Serial_Peripheral_Interface 

/* These inlcudes are need so this INO can have access to the other INO's global variables and functions. */
#include "Spresense-GNSS.h"                     // Include Defines, Declarations and Global Variables othe CPPs and the main INO file can use.
#include "Spresense-LMiC.h"                     // Include Defines, Declarations and Global Variables othe CPPs and the main INO file can use.
#include "Spresense-EdgeImpulse.h"              // Include Defines, Declarations and Global Variables othe CPPs and the main INO file can use.


/***********************************************************************************/
/********** Declarations ***********************************************************/
/***********************************************************************************/

// State of LED onboard the Spresense.
bool bLED0 = false;                             // Global: 
bool bLED1 = false;                             // Global: 
bool bLED2 = false;                             // Global:   
bool bLED3 = false;                             // Global:   

/* Print to COMM Port Timing Variables*/

bool   joinedTTN           = false;             // Global: Have we joined the TTN yet?  
long   lastSendTime        = 0;                 // Global: Last send time of moisture value to COMM port.
int    interval            = 0;                 // Global: Interval between sends of moisture value to COMM port.
long   lastTimeML          = 0;                 // Global: Last time we did an ML.
int    intervalML          = 0;                 // Global: Interval between MLs.

/* Moisture Sensor Vaiables */

int sensorPin              = A0;                // Global: Moisture sensor signal is connected to this pin.
int myMoistureValue        = 0;                 // Global: The analogh value read from the moisture sensor.

/***********************************************************************************/
/********** Local Routines *********************************************************/
/***********************************************************************************/

/**
 * @brief      Print 2 digit HEX value.
 *
 * @param[in]  aByte    A value fro 0 to 255.
 * @return     void
 */
void printHex2(unsigned aByte)
{
    aByte &= 0xff;
    if (aByte < 16)
    {
        Serial.print('0');
    }
    Serial.print(aByte, HEX);
}



/**
 * @brief      Arduino setup run-once code.
 *
 * @param[in]  NA
 * @return     void
 **/
void setup()
{
  Serial.begin(115200);
  while(!Serial);  
  Serial.print(F("Starting Serial...\r\n"));

  Serial2.begin(9600);
  while(!Serial2);  
  Serial.print(F("Starting Serial2...\r\n"));

  setup_GNSS();
  setup_LMIC();

 
}

/**
 * @brief      Arduino run-forever code loop.
 *
 * @param[in]  NA
 * @return     void
 **/
void loop()
{ 
  // Update GNSS Values
  checkGnssForAnUpdate();
  
  // Handle LoRaWAN and Show Moisture Value Periodically.  
  if (joinedTTN)
  {  
    // Run machine learning model on "myMoistureValue".
    ML_loop();
  }

  // Show moisture value periodically on COMM port.
  intervalML = millis() + 30000;                // Every 30 seconds.

  // See https://github.com/oktavianabd/arduino-lmic/blob/master/README.md#timing-issues 
  /*
        ** Timing Issues ** 
        The LoRaWAN technology for class A devices requires devices to meet hard real-time deadlines.
        The Arduino environment doesn't provide built-in support for this,
        and this port of the LMIC doesn't really ensure it, either.
        It is your responsibility, when constructing your application, to ensure that you call "os_runloop_once()" ** often enough **.

        How often is often enough?

        It depends on what the LMIC is doing. For Class A devices, 
        when the LMIC is idle, "os_runloop_once()"" need not be called at all.
        However, during a message transmit,
        it's critical to ensure that "os_runloop_once()" is called frequently prior to hard deadlines.
        The API "os_queryTimeCriticalJobs()" can be used to check whether there are any deadlines due soon.
        Before doing work that takes n milliseconds,
        call "os_queryTimeCriticalJobs(ms2osticks(n))", and skip the work if the API indicates that the LMIC needs attention.
        However, in the current implementation, the LMIC is tracking the completion of uplink transmits.
        This is done by checking for transmit-complete indications, which is done by polling.
        So you must also continually call "os_runloop_once()" while waiting for a transmit to be completed.
        This is an area for future improvement.
  */
  while (millis() < intervalML)
  {
    // Periodically call os_runloop_once() "often enuf".
    // Internally to the library os_runloop_once() calls os_runloop().
    // Jobs which are executed on the main thread by the run-time scheduler function os_runloop().
    os_runloop_once();                        // Execute run-time jobs from the timer and from the run queues.
                                              // This function is just like os_runloop(),
                                              // except that it returns after dispatching the first available job.
    // Show moisture value periodically on COMM port.
    if (millis() > interval)
    {
      interval = millis + 10000;                // Every 10 seconds.
      // Read the analog value from the sensor.
      // TODO: Add a moving average here and send that value to TTN instead of the current value.      
      myMoistureValue = analogRead(sensorPin);
      Serial.print("loop() says: Moisture = " );
      Serial.println(myMoistureValue);
    }
  }
}

