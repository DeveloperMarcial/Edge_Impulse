/***
 ************************************************************************************
 * The MIT License (MIT)                                                            *
 *                                                                                  *
 * Modifications to the Software "Spresense GPS tracker Arduino sketch"             *
 * originally found at https://www.hackster.io/141676/spresense-gps-tracker-5e4fe8  *
 * and saved herin as the Software "Spresense--GPS_Tracker_Arduino_Sketch.ino"      *
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
 ***/

/************** Includes ****************************/
//#include "Spresense-GNSS.h"

/**************************/
/* GNSS related functions */
/**************************/

void setup_GNSS()
{
  int result;
  int error_flag = 0;

  /* GNSS Debug Stuff */
  Serial.print(F("setDebugMode()..."));
  Serial2.print(F("setDebugMode()..."));
  Gnss.setDebugMode(PrintInfo);           
  Serial.print(F("Done!\r\n"));
  Serial2.print(F("Done!\r\n"));

  /* Activate GNSS Device onboard the Spresense */
  Serial.print(F("Gnss.begin()...\r\n"));
  Serial2.print(F("Gnss.begin()...\r\n"));
  result = Gnss.begin();
  Serial.print(F("Gnss.begin()...Done!\r\n"));
  Serial2.print(F("Gnss.begin()...Done!\r\n"));
  

  if (result != 0)
  {
    Serial.print(F("Gnss begin error!!\r\n"));
    Serial2.print(F("Gnss begin error!!\r\n"));
    error_flag = 1;
  }
  else
  {
    /* Setup GNSS
        It is possible to setup up to two GNSS satellites systems.
        Depending on your location you can improve your accuracy by selecting different GNSS system than the GPS system.
        See: https://developer.sony.com/develop/spresense/developer-tools/get-started-using-nuttx/nuttx-developer-guide#_gnss
        for detailed information.
    */
    Serial.print(F("Gnss.select()...\r\n"));
    Serial2.print(F("Gnss.select()...\r\n"));
    switch (satType)
    {
      case eSatGps:
        Gnss.select(GPS);
        Serial.print(F("Gnss.select() = GPS\r\n"));
        Serial2.print(F("Gnss.select() = GPS\r\n"));
        break;

      case eSatGpsSbas:
        Gnss.select(GPS);
        Gnss.select(SBAS);
        Serial.print(F("Gnss.select() = GPS, SBAS\r\n"));
        Serial2.print(F("Gnss.select() = GPS, SBAS\r\n"));
        break;

      case eSatGlonass:
        Gnss.select(GLONASS);
        Serial.print(F("Gnss.select() = GLONASS\r\n"));
        Serial2.print(F("Gnss.select() = GLONASS\r\n"));
        break;

      case eSatGpsGlonass:
        Gnss.select(GPS);
        Gnss.select(GLONASS);
        Serial.print(F("Gnss.select() = GPS, GLONASS\r\n"));
        Serial2.print(F("Gnss.select() = GPS, GLONASS\r\n"));
        break;

      case eSatGpsQz1c:
        Gnss.select(GPS);
        Gnss.select(QZ_L1CA);
        Serial.print(F("Gnss.select() = GPS, QZ_L1CA\r\n"));
        Serial2.print(F("Gnss.select() = GPS, QZ_L1CA\r\n"));
        break;

      case eSatGpsQz1cQz1S:
        Gnss.select(GPS);
        Gnss.select(QZ_L1CA);
        Gnss.select(QZ_L1S);
        Serial.print(F("Gnss.select() = GPS, QZ_L1CA, QZ_L1S\r\n"));
        Serial2.print(F("Gnss.select() = GPS, QZ_L1CA, QZ_L1S\r\n"));
        break;

      case eSatGpsGlonassQz1c:
      default:
        Gnss.select(GPS);
        Gnss.select(GLONASS);
        Gnss.select(QZ_L1CA);
        Serial.print(F("Gnss.select() = GPS, GLONASS, QZ_L1CA\r\n"));
        Serial2.print(F("Gnss.select() = GPS, GLONASS, QZ_L1CA\r\n"));
        break;
    }
    Serial.print(F("Gnss.select()...Done!\r\n"));
    Serial2.print(F("Gnss.select()...Done!\r\n"));

    /* Start Positioning */
    Serial.print(F("Gnss.start()...\r\n"));
    Serial2.print(F("Gnss.start()...\r\n"));
    result = Gnss.start(COLD_START);
    if (result != 0)
    {
      Serial.print(F("Gnss start error!!\r\n"));
      Serial2.print(F("Gnss start error!!\r\n"));
      error_flag = 1;
    }
    else
    {
      Serial.print(F("Gnss setup OK.\r\n"));
      Serial2.print(F("Gnss setup OK.\r\n"));
    }
    Serial.print(F("Gnss.start()...Done!\r\n"));
    Serial2.print(F("Gnss.start()...Done!\r\n"));
  }

  /* Set error LED. */
  if (error_flag == 1)
  {
    exit(0);
  }

}

static void print_pos(SpNavData *pNavData)
{
  char StringBuffer[STRING_BUFFER_SIZE];

  /*
  if (bSyncTopOfMinute && pNavData->posFixMode != FixInvalid)
  {
    bSyncTopOfMinute = false;
    syncSec = 60 - (uint8_t)pNavData->time.sec - 4; // "-4" is a magic number emperically found.
    if (syncSec < 0)
    {
      syncSec = 0;
    }
    Serial.print( "Initializing syncSec="); Serial.println( syncSec);
    Serial2.print("Initializing syncSec="); Serial2.println(syncSec);
    syncSecDoIt = true;
  }
  */

  /* Print Time */
//snprintf(StringBuffer, STRING_BUFFER_SIZE, "\t%02d : %02d : %02d.%06d,\r\n", pNavData->time.hour, pNavData->time.minute, pNavData->time.sec, pNavData->time.usec);
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "     %02d : %02d : %02d,\r\n",      pNavData->time.hour, pNavData->time.minute, pNavData->time.sec, pNavData->time.usec);
  Serial.print(StringBuffer);
  Serial2.print(StringBuffer);

  /* Print Date */
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "   %04d / %02d / %02d,\r\n", pNavData->time.year, pNavData->time.month, pNavData->time.day);
  Serial.print(StringBuffer);
  Serial2.print(StringBuffer);

  /* Print Satellites Count */
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "   numSat:%2d, ", pNavData->numSatellites);
  Serial.print(StringBuffer);
  Serial2.print(StringBuffer);

  /* Print Position Data */
  if (pNavData->posFixMode == FixInvalid)
  {
    Serial.print(F("No-Fix, "));
    Serial2.print(F("No-Fix, "));
  }
  else
  {
    Serial.print(F("Fix, "));
    Serial2.print(F("Fix, "));
  }
  if (pNavData->posDataExist == 0)
  {
    Serial.print(F("No Position, "));
    Serial2.print(F("No Position, "));
  }
  else
  {
    Serial.print(F(", Lat="));
    Serial.print(pNavData->latitude, 6);
    Serial.print(F(", Lon="));
    Serial.println(pNavData->longitude, 6);
    
    Serial2.print(F("\n   Lat="));
    Serial2.print(pNavData->latitude, 6);
    Serial2.print(F(", Lon="));
    Serial2.println(pNavData->longitude, 6);
  }
  Serial.println("");
  Serial2.println("");
}

static void print_condition(SpNavData *pNavData)
{
  char StringBuffer[STRING_BUFFER_SIZE];
  unsigned long cnt;

  /* Print satellite count. */
  snprintf(StringBuffer, STRING_BUFFER_SIZE, "numSatellites:%2d\n", pNavData->numSatellites);
  Serial.print(StringBuffer);
  Serial2.print(StringBuffer);

  for (cnt = 0; cnt < pNavData->numSatellites; cnt++)
  {
    const char *pType = "---";
    SpSatelliteType sattype = pNavData->getSatelliteType(cnt);

    /* Get satellite type. */
    /* Keep it to three letters. */
    switch (sattype)
    {
      case GPS:
        pType = "GPS";
        break;

      case GLONASS:
        pType = "GLN";
        break;

      case QZ_L1CA:
        pType = "QCA";
        break;

      case SBAS:
        pType = "SBA";
        break;

      case QZ_L1S:
        pType = "Q1S";
        break;

      default:
        pType = "UKN";
        break;
    }

    /* Get print conditions. */
    unsigned long Id  = pNavData->getSatelliteId(cnt);
    unsigned long Elv = pNavData->getSatelliteElevation(cnt);
    unsigned long Azm = pNavData->getSatelliteAzimuth(cnt);
    float sigLevel = pNavData->getSatelliteSignalLevel(cnt);

    /* Print satellite condition. */
    //                                                                                        "[%2d] Type:%s, Id:%2d, Elv:%2d, Azm:%3d, CN0:"
    snprintf(StringBuffer, STRING_BUFFER_SIZE, "[%2d] Type:%s, Id:%2d, Elv:%2d, Azm:%3d, CN0:", cnt, pType,   Id,     Elv,     Azm );
    Serial.print(StringBuffer);
    Serial.println(sigLevel, 6);                // Print CN0.
    Serial2.print(StringBuffer);
    Serial2.println(sigLevel, 6);               // Print CN0.
  }
}

void checkGnssForAnUpdate()
{
  Serial.print(F("Entering checkGnssForAnUpdate()....\r\n"));
  Serial.print(F("Entering checkGnssForAnUpdate()....\r\n"));
  /* Check update. */
  // The waitUpdate() method blocks the application and waits until some data has arrived.
  // The parameter to the method is a timeout determining for how long it should wait if nothing is received,
  // but when set to “-1” it will wait indefinitely.
  // Wait time is in units of seconds.
  //if (Gnss.waitUpdate(-1))
  if (Gnss.waitUpdate(1))
  {
    /* Get NaviData. */
    SpNavData NavData;
    Gnss.getNavData(&NavData);
    NavData_latitude      = NavData.latitude;
    NavData_longitude     = NavData.longitude;
    NavData_numSatellites = NavData.numSatellites;

    /* Set posfix LED. */
    bool LedSet = (NavData.posDataExist && (NavData.posFixMode != FixInvalid));
    digitalWrite(LED2, LedSet);

    /* Print position information. */
    print_pos(&NavData);
  }
  else
  {
    /* Not update. */
    Serial.print(F("GNSS data not updated.\r\n"));
    Serial2.print(F("GNSS data not updated.\r\n"));
  }

}



