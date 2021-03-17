# BeliefSat

BeliefSat is a 2p-PocketQube standard student nano-satellite being developed by the undergraduate students of [**_K.J.Somaiya Institute of Engineering and Information Technology, Sion, Mumbai_** .](https://kjsieit.somaiya.edu/en) The satellite itself is a sub-part of team's proposal under [PS4-Orbital platform program](https://www.isro.gov.in/update/15-jun-2019/announcement-of-opportunity-ao-orbital-platform) of ISRO, wherein, team aims to demonstrate indegenously developed technologies for PocketQube standard nano-satellites. As a part of this demonstration, **BeliefSat** will be launched out of **SomaiyaPod** which is a Pocketqube standard deployer being indegenously developed at the institute. The unique construction technique, combination of COTS components for communication, on-board computer and power sub-systems , together constitute of **SomaiyaPQBus**, around which the satellite is being made, is also one of the technologies that the team wants to demonstrate and open-source to enable use by future missions.

## So what will BeliefSat do in space and what results it will give?

1. If it is successfully deployed out of the SomaiyaPod and completes startup procedure (it will be dead inside the Pod and will only get power after it is out of pod)and then sends a beacon; reliability of both deployer and the design of the satellite will be confirmed.
2. Every 1 minute, it will send a telemetry packet, which will have values from different COTS sensors. The team aims to project the activity of telemetry reception as a way of involving more students in to Software Defined Radio and RF in general.
3. It will have a functionality as of a amateur-radio digipeater. Which means if you hold a proper Amateur radio license then you can send a 42 character message (including the spaces) to the satellite and the satellite will repeat it (after delay of upto 10 seconds depending on other important processes running on-board). Making it a utility for Amateur radio community.
4. Not only it can digipeat a message it also hast the functionality to repeat voice. For using this functionality you need a UHF transmitter and you can send a voice message to the satellite and the satellite will repeate it (after some delay depending on other important processes running on-board). It also will be a big utility for Amateur radio community.
5. If the satellite is able to operate for more than 3 months while responding to the commands and sending the health data at proper intervals, it will prove the reliablity of the unique design and set of COTS hardware used. Thus future mission could use flight proven hardwares for communication and power generation.

## What is it made of ?, What does it look like ?, What are its dimenssions and mass?

BeliefSat's structure is made by joining plates/PCBs(made out of FR$ material). This is a very novel approach in designing the structure of a satellite. Generally, satellites are made from Aluminium or its alloys, but using our novel approach we were able to design a very sturdy but cost effective sateliite. Although this technique is generally limited to nano-satellites. The PCBs that make the structure of our satellite have the solar panels and other components embedded in itself, so the complexity of assembly is minimized.

This is a concept render of our BeliefSat:

![SATFOlded](https://user-images.githubusercontent.com/67508161/85923794-c3ac7880-b8ab-11ea-8a19-435cf76c867a.jpg)

The dimension of the BeliefSat is in accordance with the PocketQube Standards. This standard includes a base plate on which the satellite is mounted and then it slides out of
the satellite deployer via rails. As per the standards, our BeliefSat is 2P ie, Base plate is 58mm x 128mm x 1.6mm (WxLxH), whereas the satellite is 58mm x 128mm x 50mm (WxLxH).
The estimated mass of BeliefSat is around 450 grams. The dimensions of our deployer (SomaiyaPod) are 145mm x 200mm x 95mm (WxLxH).

## How is it powered?

The satellite is powered by AltaDevices Single Juntion cells ([which unfortunately are out of buisness as of date of writing](https://pv-magazine-usa.com/2019/12/31/shutdown-continues-at-hanergy-owned-alta-devices-high-efficiency-pv-pioneer/)). Each of these cell produce 0.25W power when illumniated at intensity of 1000W/m^2. There are 4 such cells on long sides and 2 on shorter sides. So when illuminated, satellite may produce 0.5W watt/1watt/>1W depending on the orientation. The power remaining after being used is then stored in 2 parallely connected [2600mah Samsung 18650 batteries](https://robokits.co.in/batteries-chargers/samsung-premium-li-ion-battery/3.7v-samsung-li-ion-batteries/samsung-icr-18650-26j-2600mah-li-ion-cell-original?gclid=CjwKCAjwltH3BRB6EiwAhj0IUHaLyAB-D4SHw_PhNjM4cJyQy68a7cGyKCsCMS-I6JLJSGFChlp92BoC6-4QAvD_BwE). [SPV1040](https://www.st.com/en/power-management/spv1040.html) is used on every panel for maximum power point tracking and giving regulated voltage from the solar cells. The choice of SPV1040 was made based on flight history in [FossaSat-1](https://github.com/FOSSASystems/FOSSASAT-1). Now that AltaDevices cells won't be available in market, team will also be making one prototype with [SM141K06L by IXYS](https://www.digikey.com/product-detail/en/ixys/SM141K06L/SM141K06L-ND/9990462) and will be publishing test results for it too. 

## How does it communicate?

**_Transciever Specs_**
    **VHF**
-   **Name :** [DRA818V](http://www.dorji.com/products.php?Keyword=dra818v)
-   **Frequency Range :** 134~174 Mhz
-   **Channel Space :** 12.5/25 KHz
-   **TX Current :** 450/750 mA
-   **Temperature :** -20°C ~ +70°C
-   **Output Power :** +27/30 dbm
-   **Reciever Sensitivity :** -122 dbm (As per datasheet, observed sensititvity yet to be calculated)

    **UHF**
-   **Name :** [DRA818U](http://www.dorji.com/products.php?Keyword=dra818u)
-   **Frequency Range :** 400~470 Mhz
-   **Channel Space :** 25 KHz
-   **TX Current :** 450/750 mA
-   **Temperature :** -20°C ~ +70°C
-   **Output Power :** +27/30 dbm
-   **Reciever Sensitivity :** -122 dbm (As per datasheet, observed sensititvity yet to be calculated)
    _(UHF will only be used for voice repeater uplink)_
**_Antenna_**

Dipole made from measuring tape, which is in folded state during launch. Once in orbit, fishing cord which holds the antenna in folded position, is burnt by heating a resistor around which it is tied.

**_Link Budget_**

-   **Orbit Height :** 600 Km (proposed, may vary as per allocation by launch provider)
-   **Slant Range at 25 degree elevation :** 1213.4 Km (approximately)
-   **Free Space Path Loss at this distance for 145.825 MHz frequency :** 137.4 dbm
-   **Free Space Path Loss at this distance for 435.6 MHz frequency :** 146.9 dbm
-   **Received power neglecting mismatch,connector,atmospheric, pointing and polarization losses and receiving and transmitting antenna gains :** -118.2/-121.2 (op= 30/27dbm)
-   **Note :** A good directional antenna at receving groundstation (e.g. a 6 element yagi, plans for which will be released soon) can ensure the above mentioned losses are made up for and the signals can be received at even lower angles. However, to ensure best performance, circular polarized antennas copuled with a good LNA (e.g. [LNA4ALL](http://lna4all.blogspot.com/)) mounted over a Azimuth-Elevation antenna rotator (e.g. [Satnogs rotator](https://wiki.satnogs.org/SatNOGS_Rotator_v3)/[Sarcnet Rotator](https://www.sarcnet.org/rotator-mk2.html#RotatorMk2a)/[Yaesu G5500](https://www.yaesu.com/indexVS.cfm?cmd=DisplayProducts&ProdCatID=104&encProdID=79A89CEC477AA3B819EE02831F3FD5B8)/[WRAPS portable antenna rotator](https://ukamsat.files.wordpress.com/2013/12/wraps-mark-spencer-wa8sme-qst-jan-2014-copyright-arrl.pdf)) is recommended.

**_Data Rates in bps_**

-   **Telemetry Downlink :** 1200
-   **Digipeater Downlink :** 1200
-   **Command Uplink :** 1200
-   **Digipeater Uplink :** 1200

**_How the satellite complies with the requirment of turning off the transmitters when required ?_**
All the uplinks and downlinks occur on same frequency. Reciever is disabled while transmitting, to avoid interrupts to controller during the transmission, however, these transmissions last no more than 2 sec. This will ensure that one can relay a command during any pass. Also the transmitter is implemented to be active only when microcontroller is active, so if the main computer fails, the transmitter won't turn on. If the spacecraft computer is hung up for more than 8 sec, it resets along with the transciever. The automatic image capture mode is automatically turned off if battery voltage falls, thus adding extra layer of governance on the mode where long and continous transmission occurs. The satellite is designed to automatically turn off the microcontroller and thus the transciever in case of any thermal or short-circuit event. It is also protected against latchup situations caused by brown-outs or radiation flipping a bit, by the means of internal watchdog, which resets non-responsive or improperly finctioning controller after 8 sec. Whenever, required, satellites transmitters can be turned off until next command to turn those on by uplinking a command to do so. If the shut-down request comes when there is no pass over parent groundstation, the uplink packe format can be shared with a station which will have nearest pass.

## What are different packet types and packet formats ?

**_Telemetry Packet_**

**_APRS TELEMETRY FORMAT : The on-air packet telemetry format is as follows:_**
T#sss,111,222,333,444,555,666,777,888,999,AAA,xxxxxxxx 
_where sss : LSB of total number of system resets_
-      _111 : MSB of total number of system resets_
-      _222 : LSB of total number of resets induced by watch dog_
-      _333 : MSB of total number of resets induced by watch dog_
-      _444 : Temperature sensor 1 reading_
-      _555 : Temperature sensor 2 reading_
-      _666 : Gyro-sensor 1 reading_
-      _777 : Gyro-sensor 2 reading_
-      _888 : Gyro-sensor 3 reading_
-      _999 : Current reading_
-      _AAA : Voltage reading_
-      _xxxxxxxx : If the last two bits are 10 then digipeater is on and if they are 01 then voice repeater is on.

**References for converting Sensor data into readable format**

1. Temperature Sensor (DS18B20) :- [Datasheet](https://pdf1.alldatasheet.com/datasheet-pdf/view/58557/DALLAS/DS18B20.html)

2. Gyro Sensor (MPU-6050) :- [Datasheet](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf)

3. Current/Power Sensor (INA219) :- [Datasheet](https://www.ti.com/lit/ds/symlink/ina219.pdf?ts=1593455980944&ref_url=https%253A%252F%252Fwww.google.com%252F)
