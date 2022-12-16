#ifndef _SPRESENSE_GNSS_H_
#define _SPRESENSE_GNSS_H_

/************** Includes ****************************/
#include <Arduino.h>                            // Since this is a CPP file it doesn't know about things like "Serial.print()", so we include it.
#include <GNSS.h>                               // SpGnss object.

/************** Defines *****************************/

#define STRING_BUFFER_SIZE  128                 // GNSS Buffer size for messages to the Serial Monitor.

/************** Declarations *****************************/

/* GNSS Variables */
/**
   @enum ParamSat
   @brief Satellite system
*/
enum ParamSat {
  eSatGps,            /**< GPS                     World wide coverage  */
  eSatGlonass,        /**< GLONASS                 World wide coverage  */
  eSatGpsSbas,        /**< GPS+SBAS                North America        */
  eSatGpsGlonass,     /**< GPS+Glonass             World wide coverage  */
  eSatGpsQz1c,        /**< GPS+QZSS_L1CA           East Asia & Oceania  */
  eSatGpsGlonassQz1c, /**< GPS+Glonass+QZSS_L1CA   East Asia & Oceania  */
  eSatGpsQz1cQz1S,    /**< GPS+QZSS_L1CA+QZSS_L1S  Japan                */
};

/************** Global Variables *************************/
static  SpGnss Gnss;                            // Global: SpGnss object
static enum ParamSat satType =  eSatGpsGlonass; // Global: What GNSS systems we want to RX.
float   NavData_latitude;                       // Global: So we don't have to pass around the SpNavData object.
float   NavData_longitude;                      // Global: So we don't have to pass around the SpNavData object.
uint8_t NavData_numSatellites;                  // Global: So we don't have to pass around the SpNavData object.
bool junk = false;

/************** Prototypes *******************************/
void setup_GNSS();
static void print_pos(SpNavData *pNavData);
static void print_condition(SpNavData *pNavData);
void checkGnssForAnUpdate();

#endif // __SPRESENSE_GNSS_H_