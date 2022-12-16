/************************************************************/
/* Declarations  for Connecting to The Things Network (TTN} */
/*             https://www.thethingsnetwork.org             */
/*                                                          */     
/* TTN is a global collaborative Internet of Things         */
/* ecosystem that creates networks, devices and             */
/* solutions using LoRaWAN®.                                */
/************************************************************/

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
// AppEUI and JoinEUI refer to the same hex ID.  LoRaWAN v1.1 renamed JoinEUI to AppEUI.
// This AppEUI must be in little-endian format,
// so least-significant-byte first.
// When copying an JoinEUI from The Things Network Console (TTNCON) output,
// this means to reverse the bytes.
// The following AppEUI was derived from the MAC address of the attatched LoRa radio board:
//  MAC Addy:                 98:   76:   B6:               12:   92:   02
//  Derived AppEUI:           98:   76:   B6:   FF:   FE:   12:   92:   02
//  Derived AppEUI:           98    76    B6    FF    FE    12    92    02 <--  <-- <-- Enter thsi in to the TTN <-- <-- <--
//  AppEUI (little endian): 0x02, 0x92, 0x12, 0xfe, 0xff, 0xb6, 0x76, 0x98
static const u1_t PROGMEM APPEUI[8]={ 0x02, 0x92, 0x12, 0xfe, 0xff, 0xb6, 0x76, 0x98 };

void os_getArtEui (u1_t* buf) { memcpy_P(buf, APPEUI, 8);}      // The implementation of this callback function has to provide the application EUI and
                                                                // copy it to the given buffer.
                                                                // The application EUI is 8 bytes in length and is stored in little-endian format,
                                                                // that is, leastsignificant-byte-first (LSBF).

/* The DevEUI is a 64-bit globally-unique Extended Unique Identifier (EUI-64)
   assigned by the manufacturer, or the owner, of the end-device.
   To create EUI-64 identifiers, the assignor must have an Organizationally Unique Identifier (OUI) from the IEEE Registration Authority.
 */
// DevEUI should also be in little endian format, see AppEUI above.
// DevEUI from TTN Console:     70   B3   D5   7E   D0   05   88   F0 <-- TTN Console gernerated this value.
// DevEUI (little endian) ID: 0xF0 0x88 0x05 0xD0 0x7E 0xD5 0xB3 0x70<-- Little-Endian.
// For TTN issued EUIs the last bytes should be                     0xD5, 0xB3, 0x70.
static const u1_t PROGMEM DEVEUI[8]={ 0xF0, 0x88, 0x05, 0xD0, 0x7E, 0xD5, 0xB3, 0x70 };
void os_getDevEui (u1_t* buf) { memcpy_P(buf, DEVEUI, 8);}      // The implementation of this callback function has to provide the device EUI and
                                                                // copy it to the given buffer.
                                                                // The device EUI is 8 bytes in length and is stored in little-endian format,
                                                                // that is, least-significantbyte-first (LSBF).
// This key should be in BIG endian format.
// Since it is not really a number but a block of memory, endianness does not really apply.
// In practice, a key taken from TTNCON can be copied as-is.
// The TTNCON generates the AppKey[].
//                                         86    66    8D    04    B5    AD    CF    0E    15    48    66    7E    94    2E    DF    1D
static const u1_t PROGMEM APPKEY[16] = { 0x86, 0x66, 0x8D, 0x04, 0xB5, 0xAD, 0xCF, 0x0E, 0x15, 0x48, 0x66, 0x7E, 0x94, 0x2E, 0xDF, 0x1D };
void os_getDevKey (u1_t* buf) { memcpy_P(buf, APPKEY, 16);}     // The implementation of this callback function has to provide the device-specific cryptographic
                                                                // application key and copy it to the given buffer.
                                                                // The device-specific application key is a 128-bit AES key (16 bytes in length).
