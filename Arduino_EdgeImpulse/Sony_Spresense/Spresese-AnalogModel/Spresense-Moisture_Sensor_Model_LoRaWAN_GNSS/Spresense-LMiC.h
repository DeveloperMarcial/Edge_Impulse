#ifndef _SPRESENSE_LMIC_H_
#define _SPRESENSE_LMIC_H_

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

/******************************************************************************************************************************************************************/
/* <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <> */
#include "TTN_IDsAndKeys.h"                       // Rem this line. 
                                                  // The file contains "The Things Network" private DeviceID, JoinID, aka AppID, and API Key.
                                                  // This file is excluded from the GIT Commit. See next #include line "YourTTN_IDsAndKeys.h".
                                                  
//#include "YourTTN_IDsAndKeys.h"                 // Un-Rem this line.  
                                                  // Edit this file with your "The Things Network" private DeviceID, JoinID or AppID, and API Key.
/* <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <> */
/******************************************************************************************************************************************************************/

/******************************************************************************************************************************************************************/
/* <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <>  <> */
#include <Soil_Moisture_with_LoRa_inferencing.h>  // Edge Impulse ML library. <-- Change to your Edge Impulse Arduino Deplyed Library
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
                  //function decodeUplink(input) {
                  //  return {
                  //    data: {
                  //      bytes: input.bytes
                  //    },
                  //    warnings: [],
                  //    errors: []
                  //  };
                  //}
                  function Decoder(bytes, port) { 
                    // The Decoder() function takes your transmitted LoRa string and prses it into variables.
                    var data = bytes.filter(function (byte) { return byte !== 0x00 } )
                    var json = JSON.parse(String.fromCharCode.apply(null, data));

                    var var1      = json.moisture;
                    var var2      = json.snr;
                    var var3      = json.rssi;
                    var var4      = json.dry;
                    var var5      = json.wet;
                    var var_latitude  = json.lat;
                    var var_longitude = json.lng;
                    var var_status    = json.num;  // We will use "number of statelites" as our status.
                    // In The Things Network Console use the Payload Formats tab in the application for The Things Network
                    // to control the payload sent to ThingSpeak.
                    // ThingSpeak can accept
                    //    up to 8 fields of data per channel,
                    //    a status field, and
                    //    three position fields including
                    //      latitude,
                    //      longitude, and
                    //      elevation.
                    //  The position fields are used in message "as.up.location.forward".
                    //  Your payload must be a JSON formatted object with at least one of the allowed fields.
                    //  The names of the payload fields must be field1, field2, etc. else ThingSpeak will get null values.
                    return {
                      field1:    var1,
                      field2:    var2,
                      field3:    var3,
                      field4:    var4,
                      field5:    var5,
                      latitude:  var_latitude,
                      longitude: var_longitude,
                      status:    var_status
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

/* LoRa Constants and Variables */

#define LORA_PKG_LENGTH 120                     // Global: The largest message we can send the TTN. Adjust this based on your data package length.
static uint8_t mydata[LORA_PKG_LENGTH] = "";    // Global: The storage for the message we send to the TTN.
static osjob_t sendjob;                         // Global: Identifies the job and stores context information.

// Schedule TX every this many seconds.
// Might become longer due to dutycycle limitations.
const unsigned TX_INTERVAL = 60;              // Send a msg to TTN every hour.

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

/* SNR and RSSI Variables */

// TTN STRONGLY recommends that you turn floats in to integers by multiplying it be 10, 100, 1000 or whatever,
// as they are so much easier to encode and then at the application end you divide by the multiplier you used.

int16_t snrTenfold         = 0;                 // Global: Data comes from "getSnrTenfold()".
int8_t  mySNR              = 0;                 // Global: Data comes from "snrTenfold / 10".
int8_t  snrDecimalFraction = 0;                 // Global: Data comes from "snrTenfold % 10".
int16_t myRSSI             = 0;                 // Global: Data comes from "getRssi(snr)".

#endif // _SPRESENSE_LMIC_H_