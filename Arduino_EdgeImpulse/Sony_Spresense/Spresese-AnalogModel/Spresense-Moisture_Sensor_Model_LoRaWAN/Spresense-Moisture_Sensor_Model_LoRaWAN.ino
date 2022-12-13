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
#include <lmic.h>                                 // LoRAWAN library from https://github.com/mcci-catena/arduino-lmic.
                                                  // Thank you MCCI Corporation! Checkout their website https://mcci.com/, they have some cool stuff for sale.
                                                  // Nice LMIC implementation example: https://github.com/lnlp/LMIC-node/blob/main/src/LMIC-node.cpp 
                                                  // The Arduino LMIC library can be configured to support LoRaWAN Class A and Class B operation.
                                                  //    A Class A device receives only at fixed times after transmitting a message. This allows for low power
                                                  //    operation, but means that downlink latency is controlled by how often the device transmits.
                                                  //    A Class B device synchronizes to beacons transmitted by the network, and listens for messages at certain
                                                  //    intervals (“ping slots”) during the interval between beacons. 

#include <hal/hal.h>                              // HAL to run LMIC on top of the Arduino environment.
                                                  // HAL = Hardware Abstraction Layer
                                                  // See https://en.wikipedia.org/wiki/Hardware_abstraction 

#include <SPI.h>                                  // SPI 4-wire bus.
                                                  // SPI = Serial Peripheral Interface
                                                  // See https://en.wikipedia.org/wiki/Serial_Peripheral_Interface 

/******************************************************************************************************************************************************************/
/* <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <> */
#include <Soil_Moisture_with_LoRa_inferencing.h>  // Edge Impulse ML library. <-- Change to your Edge Impulse Arduino Deplyed Library
/* <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <> */
/******************************************************************************************************************************************************************/

/******************************************************************************************************************************************************************/
/* <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <> */
#include "TTN_IDsAndKeys.h"                       // Rem this line. 
                                                  // The file contains "The Things Network" private DeviceID, JoinID, aka AppID, and API Key.
                                                  // This file is excluded from the GIT Commit. See next #include line "YourTTN_IDsAndKeys.h".
                                                  
//#include "YourTTN_IDsAndKeys.h"                 // Un-Rem this line.  
                                                  // Edit this file with your "The Things Network" private DeviceID, JoinID or AppID, and API Key.
/* <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <> */
/******************************************************************************************************************************************************************/

/***********************************************************************************/
/********** HOWTOs *****************************************************************/
/***********************************************************************************/

/***********************************************************************************/
// ***                                                                          *** /
// *** Notes on LoRaWAN                                                         *** /
// ***                                                                          *** /
/***********************************************************************************/
//
//  Using LoRaWAN in C
//  “LMiC” stands for “LoRaWAN MAC in C”
//      See https://basicmac.io/ 
//  What does MAC stand for?
//      See https://en.wikipedia.org/wiki/LoRa#LoRaWAN states:
//      Since LoRa defines the lower physical layer, the upper networking layers were lacking.
//      LoRaWAN is one of several protocols that were developed to define the upper layers of the network.
//      LoRaWAN is a cloud-based medium access control (MAC) layer protocol,
//      but acts mainly as a network layer protocol for managing communication between LPWAN gateways and
//      end-node devices as a routing protocol, maintained by the LoRa Alliance.
//
//  LoRaWAN was once called LoRaMAC! LoRaMAC is a much better name.

/***********************************************************************************/
// ***                                                                          *** /
// *** Setting up The Things Network Application                                *** /
// ***                                                                          *** /
/***********************************************************************************/
// TTN is a cloud that captures your LoRaWAN data transmitted from
// your LoRa radio thru a LoRaWAN gateway connected to the Internet.
//  1.  Login to: https://www.thethingsnetwork.org/
//  2.  Open your `Console`.
//  3.  Goto `Applications`.
//  4.    Click `Create Application` button.
//  5.    Fill out form. Click `Create Application` button.
//  6.  Click `Register End Device`
//  7.    Click radio button `Enter end device specifics manually`.
//  8.    Select `Frequency Plan` = `United States 902-928 MHz, FSB 2 (used by TTN)`
//  9.    Select `LoRaWAN Version` = `LoRaWAN Specifications 1.0.3` <--This version is currently compatiable with the `lmic.h` library.
// 10.    NOTE: `Regional Parameters Version` was automatically selected = `RP001 Regional Parameters 1.0.3 revision A`
// 11.    In the `JoinEUI` field enter the value shown below in `APPEUI[]` with commans and `0x` removed (spaces are ok).
//        Also see commenst above the declaration of APPEUI[] to see how it was derived.
// 12.    The `DevEUI`, `AppKey`, and `End Device ID fields` now morfs into view.
// 13.    Click the `Generate` button next to *DevEUI*.
// 14.    Click the `Generate` button next to *AppKey*.
// 15.    Note: `End Device ID` filed is automatically filled in.
// 16.    Copy the *DevEUI* value to `DEVEUI[]` below (see comments in file "YourTTN_IDsAndKeys.h").
// 17.    Copy the *AppKey* value to `APPKEY[]` below (see comments in file "YourTTN_IDsAndKeys.h").
// 18.    Click `Register End Device` button.
// 19.  In TTN on the left panel, click `Payload Formatters`, then `Uplink`.
// 20.    From the `Formatter Type` dropdown select `Custom Javascript Formatter`.
// 21.    Enter:
          /* <code>                  
                  function Decoder(bytes, port)
                  {
                    // The Decoder() function takes your transmitted LoRa string and prses it into variables. 
                    var data = bytes.filter(function (byte) { return byte !== 0x00 } )
                    var json = JSON.parse(String.fromCharCode.apply(null, data));

                    var var1 = json.moisture;
                    var var2 = json.snr;
                    var var3 = json.rssi;
                    var var4 = json.dry;
                    var var5 = json.wet;
                    // In The Things Network Console use the Payload Formats tab in the application for The Things Network
                    // to control the payload sent to ThingSpeak.
                    // ThingSpeak can accept
                    //    up to 8 fields of data per channel,
                    //    a status field, and
                    //    three position fields including
                    //      latitude,
                    //      longitude, and
                    //      elevation.
                    //  Your payload must be a JSON formatted object with at least one of the allowed fields.
                    //  The names of the payload fields must be field1, field2, etc. else ThingSpeak will get null values.
                    return { field1: var1,
                             field2: var2,
                             field3: var3,
                             field4: var4,
                             field5: var5
                           };
                  }
              </code>
            */
// 22.      Click `Save Changes` button. 
// 23.      In the left TTN panel click `Live Data` to see moisture data coming in from your LoRaWAN device.
//      ----------------------------------------------------------------------
// 24.  Compile this Arduino program and make sure TTN is RXing moisture data.
//      ----------------------------------------------------------------------

/***********************************************************************************/
// ***                                                                          *** /
// *** Setting up The Things Speak Application                                  *** /
// ***                                                                          *** /
/***********************************************************************************/
// ThingsSpeak is a Mathwworks hosted web site that accepts data from your TTN Application
// via a TTN WebHook and charts and manipulates your data with MATLAB without needing a MATLAB license.
// How cool Mathwworks! Thankyou!!
// Since I don't have a Sony Spresense I can dedicate to daily logging to ThingsSpeak,
// take a look at this example of what can be done: https://thingspeak.com/channels/1623648 
//  1.  Sign in to: https://thingspeak.com/ 
//  2.  Click `New Channel` button.
//  3.    Fill out the form.
//          Select the `checkmark` on Fields1, Fields2, Fields3, Fields4, and Fields5.
//          In the textbox give the Field a human readable name.
//  4.  Select the `API KEys tab`
//          Copy the `Channel ID`    = 1234567            <-- <> <> <> Example ID  <> <> <>
//          Copy the `Write API Key` = 0123456789ABCDEF   <-- <> <> <> Example Key <> <> <>
//  5.  Setup a Webhook into ThingsSpeak
//        Go to the TTN website, select you application.      
//  6.  From the left panel select `Integrations`, then `Webhooks`
//  7.  Click the `+ Add Webhook` button
//  8.  Click the `ThingsSpeak` tile and enter:
//        Webhook ID: devmarcial-thingsspeak-webhook      <-- This can be anything you want.
//        Channel ID: 1234567                             <-- <> <> <> Example ID  <> <> <>
//        API Key:    0123456789ABCDEF                    <-- <> <> <> Example Key <> <> <>
//  9.  Click `Create ThingsSpeak Webhook` button.
// 10.  Click on `Live Data` to assure data is comining in from the Arduino Mega LoRa system..
// 11.  Once data comes in, switch to the ThingsSpeak website and you should see new data.

/***********************************************************************************/
/********** Declarations ***********************************************************/
/***********************************************************************************/

/* LoRa Constants and Variables */

#define LORA_PKG_LENGTH 66                      // Global: The largest message we can send the TTN. Adjust this based on your data package length.
static uint8_t mydata[LORA_PKG_LENGTH] = "";    // Global: The storage for the message we send to the TTN.
static osjob_t sendjob;                         // Global: Identifies the job and stores context information.

// Schedule TX every this many seconds.
// Might become longer due to dutycycle limitations.
const unsigned TX_INTERVAL = 3600;              // Send a msg to TTN every hour.

/* Setup Pins on Sony Spresense */

// Pin Mappings
// RFM9x------Spresense Carrier Board
// Vin========3.3V
// Gnd--------Gnd

// Four digital I/O lines are needed in output mode to drive the radio’s antenna switch (RX and TX), the SPI chip select (NSS), and the reset line (RST).
// Clk========D13                               // A SPI unit is needed to read and write the radio’s registers.
// MISO-------D12                               // A SPI unit is needed to read and write the radio’s registers.
// MOSI=======D11                               // A SPI unit is needed to read and write the radio’s registers.
// CS---------D6                                // The SPI chip select (NSS).
// RST========D5                                // The reset line (RST).

// Three digital I/O lines are needed in input mode to sense the radio’s transmitter and receiver states (DIO0, DIO1 and DIO2)
// G0 (IRQ)---D2
// G1 (IRQ)===D3
// G2 (IRQ)---D4

// Why do we arbitrarily choose pin #6 and not Named Pin, SPI_CS, #10?
// Answer: Sony "Pin Groups".
// If we set "lmic_pinmap lmic_pins.nss" equal to 10 the Sony library thinks you are redefining this 
// named SPI CS pin as something else.  As a Sony side effect, it tears down the SPI5 pins.
// "Pin Groups" are a Sony library concept.
//      Pin Groups can be nasty:
//            See https://developer.sony.com/develop/spresense/docs/arduino_developer_guide_en.html 
//            pinMode() sets the specified pin to GPIO mode,
//            but note that the mode setting is controlled in units of pin groups.
//            For example, in the case of SPI, the 4 pins CS, SCK, MOSI, and MISO belong to the same group,
//            and if pinMode() specified as the argument of CS pin is called,
//            !!! ---> it will not be able to operate as the SPI function. <---!!!
//            For more information about the pin groups, see Connector pin list (xlsx).
//            See https://github.com/sonydevworld/spresense-hw-design-files/raw/master/Pin/Spresense_pin_function_en.xlsx.

const lmic_pinmap lmic_pins = {
  .nss = 6,                                     // SPI_CS
  .rxtx = LMIC_UNUSED_PIN,                      // Not used.
  .rst = 5,                                     // RFM9x_RST
  .dio = {2, 3, 4},                             // G0, G1, G2
};

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

/* SNR and RSSI Variables */

int16_t snrTenfold         = 0;                 // Global: Data comes from "getSnrTenfold()".
int8_t  mySNR              = 0;                 // Global: Data comes from "snrTenfold / 10".
int8_t  snrDecimalFraction = 0;                 // Global: Data comes from "snrTenfold % 10".
int16_t myRSSI             = 0;                 // Global: Data comes from "getRssi(snr)".

/* Edge Impulse Variables */
// Copy raw features here (for example from the 'Live classification' page).
// See https://docs.edgeimpulse.com/docs/running-your-impulse-arduino
static float features[2]   = {};  // Results in [0,0,0]
       float dryPrediction = 0;
       float wetPrediction = 0;

/***********************************************************************************/
/********** Local Routines *********************************************************/
/***********************************************************************************/

/**
 * @brief      Copy raw feature data in out_ptr.
 *             Function called by Edge Impulse inference library.
 *
 * @param[in]  offset   The offset
 * @param[in]  length   The length
 * @param      out_ptr  The out pointer. The Edge Impulse Classifier uses this buffer as inpuy.
 *
 * @return     0
 */
int raw_feature_get_data(size_t offset, size_t length, float *out_ptr)
{
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}

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
 * @brief      Calculation per SX1276 datasheet rev.7 §6.4, SX1276 datasheet rev.4 §6.4.
 *             LMIC.snr contains value of PacketSnr, which is 4 times the actual SNR value.
 *
 * @param[in]  NA
 * @return     Returns ten times the SNR (dB) value of the last received packet.
 *             Ten times to prevent the use of float but keep 1 decimal digit accuracy.
 */
int16_t getSnrTenfold()
{    
    return (LMIC.snr * 10) / 4;
}

/**
 * @brief      Alternate to get RSSI value.
 *
 * @param[in]  SNR.
 * @return     Returns RSSI.
 */
int16_t getRssi_TRY(int8_t snr)
{
  oslmic_radio_rssi_s my_oslmic_radio_rssi_t;

  //void radio_monitor_rssi(ostime_t 180, oslmic_radio_rssi_t *pRssi);
         radio_monitor_rssi(180,          &my_oslmic_radio_rssi_t);
  return my_oslmic_radio_rssi_t.mean_rssi;
}

/**
 * @brief      Get RSSI value.
 *
 * @param[in]  SNR.
 * @return     Returns RSSI.
 */
int16_t getRssi(int8_t snr)
{
    // Returns correct RSSI (dBm) value of the last received packet.
    // Calculation per SX1276 datasheet rev.7 §5.5.5, SX1272 datasheet rev.4 §5.5.5.
    // SX1276 datasheet: https://semtech.my.salesforce.com/sfc/p/#E0000000JelG/a/2R0000001Rbr/6EfVZUorrpoKFfvaF_Fkpgp5kzjiNyiAbqcpqh9qSjE

    #define RSSI_OFFSET            64
    #define SX1276_FREQ_LF_MAX     525000000    // 525 MHz...per datasheet 6.3.
    #define SX1272_RSSI_ADJUST     -139
    #define SX1276_RSSI_ADJUST_LF  -164
    #define SX1276_RSSI_ADJUST_HF  -157

    int16_t rssi = 0xAAAA;

    #ifdef MCCI_LMIC
        Serial.println("Doing MCCI_LMIC...");
        rssi = LMIC.rssi - RSSI_OFFSET;

    #else
        Serial.println("Not doing MCCI_LMIC...");
        int16_t rssiAdjust = 0x5555;
        #ifdef CFG_sx1276_radio
            if (LMIC.freq > SX1276_FREQ_LF_MAX)
            {
                rssiAdjust = SX1276_RSSI_ADJUST_HF;
            }
            else
            {
                rssiAdjust = SX1276_RSSI_ADJUST_LF;   
            }
        #else
            // CFG_sx1272_radio    
            rssiAdjust = SX1272_RSSI_ADJUST;
        #endif    
        
        // Revert modification (applied in lmic/radio.c) to get PacketRssi.
        int16_t packetRssi = 0xAAAA;
                packetRssi = LMIC.rssi + 125 - RSSI_OFFSET;
        if (snr < 0)
        {
            rssi = rssiAdjust + packetRssi + snr;
        }
        else
        {
            rssi = rssiAdjust + (16 * packetRssi) / 15;
        }
    #endif

    return rssi;
}

/*
    The LMIC library offers a simple event-based programming model where all protocol events are
    dispatched to the application’s onEvent() callback function (see 2.3.4).
    In order to free the application of details like timings or interrupts,
    the library has a built-in run-time environment to take care of timer queues and job management.

    The implementation of this callback function may react on certain events and trigger new actions
    based on the event and the LMIC state.
    Typically, an implementation processes the events it is interested in and
    schedules further protocol actions using the LMIC API.
*/
void onEvent (ev_t ev)
{ 
  /*
      The LMIC uses values of the type ostime_t to represent time in ticks.
      The rate of these ticks defaults to 32768 ticks per second,
      but may be configured at compile time to any value between 10000 ticks per second and 64516 ticks per second.
      In general, one tick is not an integral number of microseconds or milliseconds.
      Convenience functions are provided for switching back and forth. See typedef int32_t ostime_t;
  */
  Serial.print(os_getTime());   // Query absolute system time (in ticks). Difference between millis() and os_getTime(): os_getTime() = 62.5 * millis().
  Serial.print(F(": "));

  switch (ev) {
    case EV_RFU1:
      Serial.print(F("EV_RFU1\r\n"));
      break;
    case EV_SCAN_FOUND:
      /*
          This event is reserved for future use, and is never reported.
      */
      Serial.print(F("EV_SCAN_FOUND\r\n"));
      break;
    case EV_SCAN_TIMEOUT:
    /*
        After a call to LMIC_enableTracking() no beacon was received within
        the beacon interval. Tracking needs to be restarted.
    */
      Serial.print(F("EV_SCAN_TIMEOUT\r\n"));
      break;
    case EV_BEACON_FOUND:
      /*
        After a call to LMIC_enableTracking() the first beacon has been received within the beacon interval.
      */
      Serial.print(F("EV_BEACON_FOUND\r\n"));
      break;
    case EV_BEACON_MISSED:
      /*
          No beacon was received at the expected time.
      */
      Serial.print(F("EV_BEACON_MISSED\r\n")); 
      break;
    case EV_BEACON_TRACKED:
      /*
        The next beacon has been received at the expected time
      */
      Serial.print(F("EV_BEACON_TRACKED\r\n"));
      break;
    case EV_JOINING:
      /*
          The node has started joining the network.
      */
      Serial.print(F("EV_JOINING...\r\n"));
      break;
    case EV_JOINED:
      /*
          The node has successfully joined the network and is now ready for data exchanges
      */
      // Example printout:
      //    EV_JOINED
      //    devaddr: 260C1C3F
      //    AppSKey: 7D-4C-78-71-04-B6-3B-C1-A9-8F-4D-D1-85-58-8B-6B
      //    NwkSKey: 6E-20-05-DA-CB-7A-46-EE-CD-74-65-62-61-77-6D-E0
      Serial.print(F("EV_JOINED!\r\n"));

      {
        joinedTTN = true;
        u4_t netid = 0;
        devaddr_t devaddr = 0;
        u1_t nwkKey[16];
        u1_t artKey[16];
        LMIC_getSessionKeys(&netid, &devaddr, nwkKey, artKey);
        Serial.print("\tnetid: ");   Serial.println(netid, DEC);
        Serial.print("\tdevaddr: "); Serial.println(devaddr, HEX);
        Serial.print("\tAppSKey: ");
        for (size_t i=0; i<sizeof(artKey); ++i)
        {
          if (i != 0)
          {
             Serial.print("-");
          }
          printHex2(artKey[i]);
        }
        Serial.println("");
        Serial.print("\tNwkSKey: ");
        for (size_t i=0; i<sizeof(nwkKey); ++i)
        {
          if (i != 0)
          {
            Serial.print("-");
          }
          printHex2(nwkKey[i]);
        }
        Serial.println();
      }
      // Disable link check validation (automatically enabled during join, but not supported by TTN at this time).
      LMIC_setLinkCheckMode(0);
      break;    
    case EV_JOIN_FAILED:
      /*
          The node could not join the network (after retrying)
      */
      Serial.print(F("EV_JOIN_FAILED!!\r\n"));
      break;
    case EV_REJOIN_FAILED:
      /*
          The node did not join a new network but is still connected to the old network.
      */
      Serial.print(F("EV_REJOIN_FAILED!!\r\n"));
      break;
    case EV_JOIN_TXCOMPLETE:
      Serial.print(F("EV_JOIN_TXCOMPLETE: no JoinAccept\r\n"));
      break;
    case EV_LOST_TSYNC:
      /*
          Beacon was missed repeatedly and time synchronization has been lost.
          Tracking or pinging needs to be restarted.
      */
      Serial.print(F("EV_LOST_TSYNC\r\n"));
      break;
    case EV_LINK_DEAD:
      /*
          No confirmation has been received from the network server for an extended
          period of time. Transmissions are still possible but their reception is uncertain.
      */
      Serial.print(F("EV_LINK_DEAD\r\n"));
      break;
    case EV_LINK_ALIVE:
      /*
          The link was dead, but now is alive again.
      */
      Serial.print(F("EV_LINK_ALIVE\r\n"));
      break;    
    case EV_TXSTART:
      /*
          This event is reported just before telling the radio driver to start transmission.
      */
      Serial.print(F("EV_TXSTART\r\n"));
      break;
    case EV_TXCANCELED:
      Serial.print(F("EV_TXCANCELED\r\n"));
      break;
    case EV_TXCOMPLETE:
    {
      /*
        The data prepared via LMIC_setTxData() has been sent, and the receive window for
        downstream data is complete. If confirmation was requested, the acknowledgement has been
        received. When handling this event, the code should also check for data reception.
      */
      //Serial.print(F("EV_TXCOMPLETE (includes waiting for RX windows)\r\n"));
      
      // Radio Stats: 
      //    https://github.com/mcci-catena/arduino-lmic/blob/master/doc/RadioDriver.md
      // RSSI: Set to RSSI + RSSI_OFF after LoRa receive. (Set to 0 for FSK receive; RSSI_OFF is 64.) You must subtract RSSI_OFF from LMIC.rssi to get the RSSI in dB.
      //    https://lora.readthedocs.io/en/latest/#rssi
      //    Typical values range from -120 dBm to -30 dBm.
      //     RSSI (dBm) = -157 + LMIC.rssi
      // SNR: Set to SNR * 4 after LoRa receive. (Set to 0 for FSK receive.)
      //    https://lora.readthedocs.io/en/latest/#snr 
      //    Typical LoRa SNR values are between: -20 dB and +10 dB
      // RPS = "radio parameter setting", and it encodes several radio settings:
      //    Spreading factor: FSK or SF7..12
      //    Bandwidth: 125, 250 or 500 kHz
      //    Coding Rate: 4/5, 4/6, 4/7 or 4/8.
      //    CRC enabled/disabled
      //    Implicit header mode on/off. (If on, receive length must be known in advance.)
      // FREQ = The frequency, in Hertz.
      
      snrTenfold         = 0;
      mySNR              = 0;
      snrDecimalFraction = 0;
      myRSSI             = 0;
      
      snrTenfold         = getSnrTenfold();
      mySNR              = snrTenfold / 10;
      snrDecimalFraction = snrTenfold % 10;
      myRSSI             = getRssi(mySNR);

      Serial.print(F("EV_TXCOMPLETE"));
      Serial.print(F(", SNR="));         Serial.print(mySNR); Serial.print(F(".")); Serial.print(snrDecimalFraction);    // Typical values range from  -20 dB  to +10 dB.
      Serial.print(F(", RSSI [dBm]="));  Serial.print(myRSSI);                                                           // Typical values range from -120 dBm to -30 dBm.
    //Serial.print(F(", RPS="));         Serial.print(LMIC.rps); 
    //Serial.print(F(", Freq="));        Serial.print(LMIC.freq);
      Serial.print("\r\n");
    
      if (LMIC.txrxFlags & TXRX_ACK)
      {
        Serial.print(F( "Received ack\r\n"));
      }
      // Any data to be received (must be non-zero length)?
      if (LMIC.dataLen != 0 || LMIC.dataBeg != 0)
      {
        // Grab 1st 2 bytes:
        //int TxPeriod = ( LMIC.frame[LMIC.dataBeg] << 8 ) + LMIC.frame[LMIC.dataBeg+1];
        Serial.print(F("Received "));
        Serial.print(LMIC.dataLen);
        Serial.print(F(" bytes of payload.\r\n"));

         // Data was received. Extract port number if any.
        uint8_t bPort = 0;
        if (LMIC.txrxFlags & TXRX_PORT)
        {
          bPort = LMIC.frame[LMIC.dataBeg - 1];
        }
        // Call user-supplied function with  port #, pMessage,                  nMessage
        //receiveMessage(                     bPort, LMIC.frame + LMIC.dataBeg, LMIC.dataLen);
        for (int i = 0; i < LMIC.dataLen; i++)
        {
          if (LMIC.frame[LMIC.dataBeg + i] < 0x10)
          {
            Serial.print(F("0"));
          }
          Serial.print(LMIC.frame[LMIC.dataBeg + i], HEX);
        }
        Serial.println();
      }

      os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL),  do_send);   // Send every TX_INTERVAL plus duty cycle time.
      
      break;
    }
    case EV_RXSTART:
      /* Do Not Print Anything -- It Wrecks Timing */
      //Serial.print(F("EV_RXSTART\r\n"));
      break;
    case EV_RXCOMPLETE:
      /*
        Class B only: a downlink has been received in a ping slot.
        The code should check for data reception.
      */      
      Serial.print(F("EV_RXCOMPLETE\r\n"));
      break;
    case EV_RESET:
      /*
          Session reset due to rollover of sequence counters.
          Network will be rejoined automatically to acquire new session.
      */
      Serial.print(F("EV_RESET\r\n"));
      break;
    default:
      Serial.print("Unknown event:" + String(ev) + "\r\n");
      break;
  }
}

/**
 * @brief      Assemble to message we want to send over LoRaWAN.
 *
 * @param[in]  j        The LMiC "job" object.
 * @return     void
 */
void do_send(osjob_t* j)
{
  // Check if there is not a current TX/RX job running.
  if (LMIC.opmode & OP_TXRXPEND)
  {
    Serial.print(F("OP_TXRXPEND, not sending.\r\n"));
  }
  else
  {
      float mySNR_local = 0.0;
            mySNR_local = (float)snrTenfold/10.0;
      ///Serial.print("snrTenfold: "); Serial.println(snrTenfold);
      ///Serial.print("mySNR_local: "); Serial.println(mySNR_local);

      // Moisture settings from a houseplant.
      //    Sensor Output Value in dry soil:    0 to 300
      //    Sensor Output Value in humid wet: 300 to 700
      //    Sensor Output Value in water:     700 to 950
      myMoistureValue = analogRead(sensorPin);
      
      //                        1         2         3         4           5         6         7
      //               123456789012345678901234567890123456789090123456789012345678901234567890
      // LoRa Payload: {"moisture":305,"snr": 12,"rssi": -120, "dry": 123.0, "wet": 123.0}
      snprintf((char *)mydata, LORA_PKG_LENGTH, "{\"moisture\":%3d,\"snr\":%3d,\"rssi\":%3d,\"dry\":%0.1f,\"wet\":%0.1f}",  myMoistureValue, snrTenfold, myRSSI, dryPrediction, wetPrediction);
      Serial.print("LoRa Payload: "); Serial.println((char *)mydata);

    /* Send Information Over LaRa */
    // Prepare upstream data transmission at the next possible time.
    // Convenience function for LMIC_setTxData(). If data is NULL, the data in LMIC.pendTxData[] will be used.
    LMIC_setTxData2(1, mydata, sizeof(mydata) - 1, 0);
    Serial.print(F("Packet queued.\r\n"));

    // Next TX is scheduled after we rx a TX_COMPLETE event.

  }
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
  Serial.print(F("Starting...\r\n"));

  // LMIC init. Initialize the operating system by calling os_init_ex(NULL).
  Serial.print(F("os_init()...\r\n"));
  os_init();
  Serial.print(F("os_init()...Done!\r\n"));

  // Reset the MAC state. Session and pending data transfers will be discarded.
  Serial.print(F("LMIC_reset()...\r\n"));
  LMIC_reset(); 
  Serial.print(F("LMIC_reset()...Done!\r\n"));

  // Disable link-check mode and ADR, because ADR tends to complicate testing.
  Serial.print(F("LMIC_setLinkCheckMode()...\r\n"));
  LMIC_setLinkCheckMode(0);
  Serial.print(F("LMIC_setLinkCheckMode()...Done!\r\n"));
  
  // Set the data rate to Spreading Factor 7.  This is the fastest supported rate for 125 kHz channels, and it
  // minimizes air time and battery power. Set the transmission power to 14 dBi (25 mW).
  Serial.print(F("LMIC_setDrTxpow()...\r\n"));
  LMIC_setDrTxpow(DR_SF7,14);
  Serial.print(F("LMIC_setDrTxpow()...Done!\r\n"));
  
  // in the US, with TTN, it saves join time if we start on subband 1 (channels 8-15). This will
  // get overridden after the join by parameters from the network. If working with other
  // networks or in other regions, this will need to be changed.
  Serial.print(F("LMIC_selectSubBand()...\r\n"));
  LMIC_selectSubBand(1);
  Serial.print(F("LMIC_selectSubBand()...Done!\r\n"));

  // Start job (sending automatically starts OTAA too)
  // 1980/01/06 00:00:01.000641, numSat: 0, No-Fix, No Position
  // 13:33:25.172 -> {"lat":0.0000000,"lng":-0.0000000,"num": 0}
  // 13:33:25.172 -> Packet queued.
  Serial.print(F("do_send()...\r\n"));
  do_send(&sendjob);
  Serial.print(F("do_send()...Done!\r\n"));
}

/**
 * @brief      Arduino run-forever code loop.
 *
 * @param[in]  NA
 * @return     void
 **/
void loop()
{ 
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

/**
 * @brief      Edge Impulse Machine Learning Model Implementation Function
 *
 * @param[in]  NA
 * @return     void
 **/
void ML_loop()
{
    ei_printf("Edge Impulse standalone inferencing (Arduino)\n");
    
    features[0] = myMoistureValue;

    if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE)
    {
        ei_printf("The size of your 'features' array is not correct. Expected %lu items, but had %lu\n",
                   EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
        delay(1000);
        return;
    }

    ei_impulse_result_t result = { 0 };         // The results of the ML classification.

    // The Edge Impulse Classifier() requires a "signal" as input.
    signal_t features_signal;
    features_signal.total_length = sizeof(features) / sizeof(features[0]);
    features_signal.get_data = &raw_feature_get_data;

    // Invoke the Edge Impulse Impulse.
    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
    ei_printf("run_classifier returned: %d\n", res);

    if (res != 0)
    {
      ei_printf("run_classifier() errored...\n", res);
      return;
    }
    
    // print the predictions
    ei_printf("Predictions ");
    ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
                result.timing.dsp,
                result.timing.classification,
                result.timing.anomaly);
    ei_printf(": \n");

    ei_printf("[");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
    {
        ei_printf("%.5f", result.classification[ix].value);
        #if EI_CLASSIFIER_HAS_ANOMALY == 1
        ei_printf(", ");
        #else
        if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
            ei_printf(", ");
        }
        #endif
    }
    #if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("%.3f", result.anomaly);
    #endif
    ei_printf("]\n");

    // Human-readable predictions.
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
    {
        ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
        if ("Dry"==result.classification[ix].label)
        {
          dryPrediction = result.classification[ix].value;
        }
        if ("Wet"==result.classification[ix].label)
        {
          wetPrediction = result.classification[ix].value;
        }
    }
    #if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
    #endif
}
