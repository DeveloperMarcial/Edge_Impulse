/************************************************************/
/* Declarations  for Connecting to The Things Network (TTN} */
/*             https://www.thethingsnetwork.org             */
/*                                                          */
/* TTN is a global collaborative Internet of Things         */
/* ecosystem that creates networks, devices and             */
/* solutions using LoRaWAN®.                                */
/************************************************************/

/******************************************************************************************************************************************************************/
// !!! Edit the following arrays with your data from The Things Network (TTN) !!!
// APPEUI[8]
// DEVEUI[8]
// APPKEY[16]
// !!! Example values are shown below and need to be changed. !!!
/******************************************************************************************************************************************************************/

/* The Things Network (TTN) Constants and Variables */
/* DeviceEUI and AppKey are end device specific (properties of the end device).
  AppEUI is a property of an entity/application that can handle the join request for this device,
  but only if the end device is registered for that application and an end device can only be registered to a single application.

  Note: Values of all ones or all zeros ( 0xFFFFFFFFFFFF or 0x0) are not valid EUI-64 values and
  should indicate that a value for the EUI-64 is not available (null). Do not use these values for the DevEUI or JoinEUI.
 */

// TTN Fair USe Policy: https://www.thethingsnetwork.org/forum/t/fair-use-policy-explained/1300 

/* The AppEUI is a global application ID in IEEE EUI64 address space that uniquely identifies the entity able to process the JoinReq frame.
   By definition, an EUI-64 address should be unique.
 */
// AppEUI and JoinEUI refer to the same hex ID. LoRaWAN v1.1 renamed JoinEUI to AppEUI.
// This AppEUI must be in little-endian format,
// so least-significant-byte first.
// When copying an JoinEUI from The Things Network Console (TTNCON) output,
// this means to reverse the bytes.
// The following AppEUI was derived from the MAC address of the attatched LoRa radio board:
//  MAC Addy:                 A1:   B2:   C3:               D4:   E5:   F6
//  Derived AppEUI:           A1:   B2:   C3:   FF:   FE:   D4:   E5:   F6
//  Derived AppEUI:           A1    B2    C3    FF    FE    D4    E5    F6 
//  AppEUI (little endian): 0xF6, 0xE5, 0xD4, 0xfe, 0xff, 0xC3, 0xB2, 0xA1
//  Copy to TTN JoinEUI text box: F6 E5 D4 fe ff C3 B2 A1                    <--  <-- <-- Enter this in to the TTN <-- <-- <--
static const u1_t PROGMEM APPEUI[8]={ 0xF6, 0xE5, 0xD4, 0xFE, 0xFF, 0xC3, 0xB2, 0xA1 };

void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}      // The implementation of this callback function has to provide the application EUI and
                                                                // copy it to the given buffer.
                                                                // The application EUI is 8 bytes in length and is stored in little-endian format,
                                                                // that is, leastsignificant-byte-first (LSBF).

/* The DevEUI is a 64-bit globally-unique Extended Unique Identifier (EUI-64)
   assigned by the manufacturer, or the owner, of the end-device.
   To create EUI-64 identifiers, the assignor must have an Organizationally Unique Identifier (OUI) from the IEEE Registration Authority.
 */
// DevEUI should also be in little endian format, see AppEUI above.
// DevEUI from TTN Console:     70   B3   D5   67   89   AB   CD   EF <-- TTN Console gernerated this value.
// DevEUI (little endian) ID: 0xEF 0xCD 0xAB 0x89 0x67 0xD5 0xB3 0x70 <-- Little-Endian.
// The TTNCON generates the devEUI[].
// For TTN issued EUIs the last bytes should be                     0xD5, 0xB3, 0x70.
static const u1_t PROGMEM DEVEUI[8]={ 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0xD5, 0xB3, 0x70 };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}      // The implementation of this callback function has to provide the device EUI and
                                                                // copy it to the given buffer.
                                                                // The device EUI is 8 bytes in length and is stored in little-endian format,
                                                                // that is, least-significantbyte-first (LSBF).
// This key should be in BIG endian format.
// Since it is not really a number but a block of memory, endianness does not really apply.
// In practice, a key taken from TTNCON can be copied as-is.
// The TTNCON generates the AppKey[].
//                                         00,   01    02    03    04    05    06    07    08    09    0A    0B    0C    0D    0E    0F <-- TTN Console gernerated this value.
static const u1_t PROGMEM APPKEY[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
void os_getDevKey (u1_t* buf) { memcpy_P(buf, APPKEY, 16);}     // The implementation of this callback function has to provide the device-specific cryptographic
                                                                // application key and copy it to the given buffer.
                                                                // The device-specific application key is a 128-bit AES key (16 bytes in length).
