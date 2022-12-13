**RFM95W LoRa Radio**
* Product: Adafruit RFM95W LoRa Radio Transceiver Breakout - 868 or 915 MHz - RadioFruit
  * Note: The board has silkscreen showing "RFM9x LoRa Radio".
* Product ID: 3072
* Cost: $19.95
* Quantity: 1
* Order here: https://www.adafruit.com/product/3072
* Assembly: https://learn.adafruit.com/adafruit-rfm69hcw-and-rfm96-rfm95-rfm98-lora-packet-padio-breakouts/assembly 
![image](https://user-images.githubusercontent.com/92460732/207215425-f6505c82-8be8-4bd5-adcf-6d4e780a9951.png)


Connect an external antenna directly to the board.
* $2.50, also include 1 x Edge-Launch SMA Connector for 1.6mm / 0.062" Thick PCBs

Connect external antenna to your enclosure
* $0.75, also include 1 x uFL SMT Antenna Connector
* Will need an adapter cable:
  * SMA to uFL/u.FL/IPX/IPEX RF Adapter Cable, or
  * RP-SMA to uFL/u.FL/IPX/IPEX RF Adapter Cable

* Use a ¼ wave whip antenna (a wire dipole, aka a piece of wire)
  * 433 MHz - 6.5 inches, or 16.5 cm
  * 868 MHz - 3.25 inches or 8.2 cm
  * 915 MHz - 3 inches or 7.8 cm
  * Antenna Length Calculator:

| Value | Units |
| ------| ----- |
|915|MHz|
|915,000,000|Hz|
|0.328|wavelength [meter]|
|32.764|wavelength [cm]|
|12.899|wavelength [inch]|
|3.225|1/4 wave [inch]|

**My Transmission Tests**
* I soldered a piece of wire 3.2" long into the *RFM95W LoRa Radio* board antenna thru hole.
* I was able to get about 1 km duplex communication between
  * [RFM95W LoRa Radio](https://www.adafruit.com/product/3072) to another *RFM95W LoRa Radio*
  * *RFM95W LoRa Radio* to a LoRaWAN [gateway](https://www.adafruit.com/product/4345) with built-in omni directional antenna.
 
**Additional Info:**
* Wires into Spresense board via SPI4 or Spresense Extension Board via SPI5.
* Need 2 radio boards for point-to-point long range tests.
* These are +20dBm LoRa packet radios that have a special radio modulation that is not compatible with the RFM69s but can go much much farther. They can easily go 2 km line of sight using simple wire antennas, or up to 20Km with directional antennas and settings tweakings.
* The chip on the radio board is a SX1276 LoRa® based module with SPI interface:
  * https://www.semtech.com/products/wireless-rf/lora-connect/sx1276
* SX1276 chip datasheet:
  * https://semtech.my.salesforce.com/sfc/p/#E0000000JelG/a/2R0000001Rbr/6EfVZUorrpoKFfvaF_Fkpgp5kzjiNyiAbqcpqh9qSjE 
* +5 to +20 dBm up to 100 mW Power Output Capability (power output selectable in software)
* ~100mA peak during +20dBm transmit, ~30mA during active radio listening.
* The RFM9x radios have a range of approx. 2 km line of sight with tuned uni-directional antennas. Depending on obstructions, frequency, antenna and power output, you will get lower ranges - especially if you are not in line of sight.
* The radios must use the same encoding schemes. For example, a 900 MHz RFM69 packet radio will not talk to a 900 MHz RFM9x LoRa radio.
* 900 MHz can be tuned from about 850-950MHz with good performance.
