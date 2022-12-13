**Indoor LoRaWAN WiFi Gateway**

* Product: *The Things* Indoor LoRaWAN WiFi Gateway - 8 Channel LoRa 900 MHz
* Product ID: 4345
* Cost: $109.95
* Order here: https://www.adafruit.com/product/4345
![image](https://user-images.githubusercontent.com/92460732/207217394-0759e023-328f-4770-813c-b796c8bb53d4.png)

**Additional Info:**
* It lets you bridge LoRa wireless network to an IP network via WiFi.
* Simply converts RF packets to IP packets and vice versa.
* Needed for communicating with RFM9x LoRa Radio from a desktop computer or cloud server such as a *"The Things Network"* (TTN) server.
* Operates at 902–928MHz and has a 2-prong 120V plug for use in the US.
* Built-in omni directional antenna.
  * The internal antenna is connected via a uFL connector so one could litterally hack the case and connect an external antenna.
* To learn more and sign up, see *The Things Network*.: https://www.thethingsindustries.com/docs/getting-started/ttn/ 
  * *NOTE: The Adruino LMiC library has only been tested with LoRaWAN 1.0.2/1.03 networks and does not have the separated key structure defined by LoRaWAN 1.1.*
    * Therefore, in TNN when prompted for `LoRaVersion`, select `LoRaWAN Specification 1.0.3`.
    * The `Regional Parameters Version` should then automatically populate with `RP001 Regional Parameters 1.0.3 revison A`.
  * Getting Started: https://www.thethingsindustries.com/docs/getting-started/ 
  * Create a free *TTN* account: https://console.cloud.thethings.network/ 
  * Setup Device: https://www.thethingsindustries.com/docs/gateways/models/thethingsindoorgateway/ 
  * Create an app: https://nam1.cloud.thethings.network/console/applications 
  * 2 layers of cryptography mean network operator cannot understand the user’s payload:
    * A unique 128-bit Network Session Key shared between the end-device and network server.
    * A unique 128-bit Application Session Key (AppSKey) shared end-to-end at the application level.
  * NOTE: Single-channel packet forwarders no longer work after the Things Network migration to The Things Stack v3. For more information about this decision, visit: https://www.thethingsnetwork.org/forum/t/single-channel-packet-forwarders-scpf-are-deprecated-and-not-supported/31117

**Creating Your Gateway EUI ID**
* The Gateway EUI is used by *TTN* to register your Gateway so that you *TTN* Console can talk with your Gateway.
![image](https://user-images.githubusercontent.com/92460732/207217816-3bf29c56-3eee-4a76-ad84-1694ead71d01.png)

**Gateway Setup**
* Plug in Gateway
* Hold RESET button 5s
  * GRN/RED (repeats)
* Hold SETUP button for 10s
  * RED
* Connect to WiFi AP with SSID: MiniHub-80409B
* Browse to 192.168.4.1.
* Configure settings as desire
* Save
  * GRN ⇒ connecting to WiFi
  * GRN/RED ⇒ Fetching config, downloading any updates
    * Takes 5-10 minutes

Additional Info:
  * https://www.thethingsnetwork.org/map 
  * https://ttnmapper.org/heatmap/ 
  * https://account.thethingsnetwork.org/users/login 
