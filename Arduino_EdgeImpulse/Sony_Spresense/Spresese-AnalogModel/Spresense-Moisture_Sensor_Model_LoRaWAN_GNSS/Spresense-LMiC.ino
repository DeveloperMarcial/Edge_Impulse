//#include "Spresense-LMiC.h"

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
      
      //                        1         2         3         4           5         6         7         8         9         10        11      
      //               1234567890123456789012345678901234567890901234567890123456789012345678901234567890123456789012345678901234567890
      // LoRa Payload: {"moisture":305,"snr": 12,"rssi": -120, "dry": 123.0, "wet": 123.0, "lat":39.062246, "lng":-108.523813, "num":20}
      snprintf((char *)mydata, LORA_PKG_LENGTH, "{\"moisture\":%3d,\"snr\":%3d,\"rssi\":%3d,\"dry\":%0.1f,\"wet\":%0.1f,\"lat\":%0.1f,\"lng\":%0.1f,\"num\":%2d}",  myMoistureValue, snrTenfold, myRSSI, dryPrediction, wetPrediction, NavData_latitude, NavData_longitude, NavData_numSatellites);
      Serial.print("LoRa Payload: "); Serial.println((char *)mydata);

    /* Send Information Over LaRa */
    // Prepare upstream data transmission at the next possible time.
    // Convenience function for LMIC_setTxData(). If data is NULL, the data in LMIC.pendTxData[] will be used.
    LMIC_setTxData2(1, mydata, sizeof(mydata) - 1, 0);
    Serial.print(F("Packet queued.\r\n"));

    // Next TX is scheduled after we rx a TX_COMPLETE event.

  }
}

void setup_LMIC()
{
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