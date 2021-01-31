# BeliefSat

BeliefSat is a 2p-PocketQube standard student nano-satellite being developed by the undergraduate students of [**_K.J.Somaiya Institute of Engineering and Information Technology, Sion, Mumbai_** .](https://kjsieit.somaiya.edu/en) The satellite itself is a sub-part of team's proposal under [PS4-Orbital platform program](https://www.isro.gov.in/update/15-jun-2019/announcement-of-opportunity-ao-orbital-platform) of ISRO, wherein, team aims to demonstrate indegenously developed technologies for PocketQube standard nano-satellites. As a part of this demonstration, **BeliefSat** will be launched out of **SomaiyaPod** which is a Pocketqube standard deployer being indegenously developed at the institute. The unique construction technique, combination of COTS components for communication, on-board computer and power sub-systems , together constitute of **SomaiyaPQBus**, around which the satellite is being made, is also one of the technologies that the team wants to demonstrate and open-source to enable use by future missions.

## So what will BeliefSat do in space and what results it will give?

1. If it is successfully deployed out of the SomaiyaPod and completes startup procedure (it will be dead inside the Pod and will only get power after it is out of pod)and then sends a beacon; reliability of both deployer and the design of the satellite will be confirmed.
2. Every 30 seconds or 1 minute (depending on which mode it is in currently), it will send a telemetry packet, which will have values from different COTS sensors. The team aims to project the activity of telemetry reception as a way of involving more students in to Software Defined Radio and RF in general.
3. It will have a functionality as of a amateur-radio digipeater. Which means if you hold a proper Amateur radio license then you can send a 42 character message (including the spaces) to the satellite and the satellite will repeat it (after delay of upto 10 seconds depending on other important processes running on-board). Making it a utility for Amateur radio community.
4. There is a COTS 2MP colour Camera onboards, which can be activated either for single snapshot by a command or multiple snapshots once evey 10 min. These captured snaps will be sent in SSDV i.e. Slow Scan Digital Video format, which is a famous format used by amateur radio community for transmitting images. Thus will also contribute as a service to amateur radio community. However, anyone with COTS radio receiver modules and software, can recieve the images and thus they will also help attracting more young students towards Amateur radio by giving a hint of exiting world of amateur radio.
5. If the satellite is able to operate for more than 3 months while responding to the commands and sending the health data at proper intervals, it will prove the reliablity of the unique design and set of COTS hardware used. Thus future mission could use flight proven hardwares for communication and power generation.
6. As the satellite doesn't have attitutde stabilization system (apart from a magnet parallel to antennas to orient them in N-S direction), all the sensor readings taken and images captured will be the ones in random orientation. For some this may look like useless thing, but it may actually serve a very useful puspose. The captured images could be analysed and a rough estimation of orientation of the satellite can be determined, then this estimation could be feed along with sensor readings at the instant of capturing the images to a machine learning algo to develop a model of orientation w.r.t. the sensor readings. Once a large no. of data points are avaialble, the generated model will be able to be used in future missions along with COTS sensors to reduce complexity and cost involved in **_Attitude Determination and Control_** subsystem.

## What is it made of ?, What does it look like ?, What are its dimenssions and mass?

BeliefSat's structure is made by "Interlocking FR4" plates/PCBs. This is a very novel approach in designing the structure of a satellite. Generally, satellites are made from
Aluminium or its alloys, but using our novel approach we were able to design a very sturdy but cost effective sateliite. Although this technique is generally limited to nano-satellites. The PCBs that make the structure of our satellite have the solar panels and other components embedded in itself, so the complexity of assembly is minimized.

This is a concept render of our BeliefSat:

![SATFOlded](https://user-images.githubusercontent.com/67508161/85923794-c3ac7880-b8ab-11ea-8a19-435cf76c867a.jpg)

The dimension of the BeliefSat is in accordance with the PocketQube Standards. This standard includes a base plate on which the satellite is mounted and then it slides out of
the satellite deployer via rails. As per the standards, our BeliefSat is 2P ie, Base plate is 58mm x 128mm x 1.6mm (WxLxH), whereas the satellite is 50mm x 50mm x 114mm (WxLxH).
The estimated mass of BeliefSat is around 400 grams.

## How is it powered?

The satellite is powered by AltaDevices Single Juntion cells ([which unfortunately are out of buisness as of date of writing](https://pv-magazine-usa.com/2019/12/31/shutdown-continues-at-hanergy-owned-alta-devices-high-efficiency-pv-pioneer/)). Each of these cell produce 0.25W power when illumniated at intensity of 1000W/m^2. There are 4 such cells on long sides and 2 on shorter sides. So when illuminated, satellite may produce 0.5W watt/1watt/>1W depending on the orientation. The power remaining after being used is then stored in 2 parallely connected [2600mah Samsung 18650 batteries](https://robokits.co.in/batteries-chargers/samsung-premium-li-ion-battery/3.7v-samsung-li-ion-batteries/samsung-icr-18650-26j-2600mah-li-ion-cell-original?gclid=CjwKCAjwltH3BRB6EiwAhj0IUHaLyAB-D4SHw_PhNjM4cJyQy68a7cGyKCsCMS-I6JLJSGFChlp92BoC6-4QAvD_BwE). [SPV1040](https://www.st.com/en/power-management/spv1040.html) is used on every panel for maximum power point tracking and giving regulated voltage from the solar cells. The choice of SPV1040 was made based on flight history in [FossaSat-1](https://github.com/FOSSASystems/FOSSASAT-1). Now that AltaDevices cells won't be available in market, team will also be making one prototype with [SM141K06L by IXYS](https://www.digikey.com/product-detail/en/ixys/SM141K06L/SM141K06L-ND/9990462) and will be publishing test results for it too. Along with other things, Every solar panel has a [INA219](https://www.ti.com/product/INA219) current sensor and [OPT3001](https://www.ti.com/lit/ds/symlink/opt3001.pdf?ts=1593144788653&ref_url=https%253A%252F%252Fwww.google.com%252F). Making a solar panel a complete module.

## How does it communicate?

**_Transciever Specs_**

-   **Name :** [LORA1268F30](https://www.nicerf.com/product_193_312.html)
-   **Frequency :** Yet to be coordinated by [IARU](https://www.iaru.org/) but will be in UHF HAM Band
-   **Modulation :** MSK (FSK with Fdev= 0.25\*data_rate)
-   **Output Power :** 1W in Normal mode and 0.5W while in Safe mode to save power
-   **Reciever Sensitivity :** -123 dbm @1.2kbps and bw=20khz (As per datasheet, observed sensititvity yet to be calculated)

**_Antenna_**

Dipole made from measuring tape, which is in folded state during launch. Once in orbit, fishing cord which holds the antenna in folded position, is burnt by heating a resistor around which it is tied.

**_Link Budget_**

-   **Orbit Height :** 500 Km (proposed, may vary as per allocation by launch provider)
-   **Slant Range at 15 degree elevation :** 1408 Km (approximately)
-   **Free Space Path Loss at this distance :** 148.2 dbm
-   **Received power neglecting mismatch,connector,atmospheric, pointing and polarization losses and receiving and transmitting antenna gains :** -118.2/-121.2 (op= 30/27dbm)
-   **Note :** A good directional antenna at receving groundstation (e.g. a 6 element yagi, plans for which will be released soon) can ensure the above mentioned losses are made up for and the signals can be received at even lower angles. However, to ensure best performance, circular polarized antennas copuled with a good LNA (e.g. [LNA4ALL](http://lna4all.blogspot.com/)) mounted over a Azimuth-Elevation antenna rotator (e.g. [Satnogs rotator](https://wiki.satnogs.org/SatNOGS_Rotator_v3)/[Sarcnet Rotator](https://www.sarcnet.org/rotator-mk2.html#RotatorMk2a)/[Yaesu G5500](https://www.yaesu.com/indexVS.cfm?cmd=DisplayProducts&ProdCatID=104&encProdID=79A89CEC477AA3B819EE02831F3FD5B8)/[WRAPS portable antenna rotator](https://ukamsat.files.wordpress.com/2013/12/wraps-mark-spencer-wa8sme-qst-jan-2014-copyright-arrl.pdf)) is recommended.

**_Data Rates in bps_**

-   **Telemetry Downlink :** 1200
-   **Digipeater Downlink :** 1200
-   **SSDV Image Downlink :** 9600
-   **Command Uplink :** 1200
-   **Digipeater Uplink :** 1200

**_How the satellite complies with the requirment of turning off the transmitters when required ?_**
All the uplinks and downlinks occur on same frequency. Reciever is disabled while transmitting, to avoid interrupts to controller during the transmission, however, these transmissions last no more than 2 sec (except when transmitting image, when single transmission may last upto 120 sec but no more than that, and there is gap of more than 300 secs between such long trsnmissions during automatic transmit mode). This will ensure that one can relay a command during any pass. Also the transmitter is implemented to be active only when microcontroller is active, so if the main computer fails, the transmitter won't turn on. If the spacecraft computer is hung up for more than 8 sec, it resets along with the transciever. The automatic image capture mode is automatically turned off if battery voltage falls, thus adding extra layer of governance on the mode where long and continous transmission occurs. The satellite is designed to automatically turn off the microcontroller and thus the transciever in case of any thermal or short-circuit event. It is also protected against latchup situations caused by brown-outs or radiation flipping a bit, by the means of internal watchdog, which resets non-responsive or improperly finctioning controller after 8 sec. Whenever, required, satellites transmitters can be turned off until next command to turn those on by uplinking a command to do so. If the shut-down request comes when there is no pass over parent groundstation, the uplink packe format can be shared with a station which will have nearest pass.

## What are different packet types and packet formats ?

**_Telemetry Packet_**

|       | Packet Type Indicator | Call Sign | Resets | Packet No. | Mode | Sensor Data | CRC | RS Parity |
| :---: | :-------------------: | :-------: | :----: | :--------: | :--: | :---------: | :-: | :-------: |
| Bytes |           1           |     6     |   2    |     4      |  1   |     42      |  2  |    32     |
| Index |           0           |     1     |   7    |     9      |  13  |     14      | 56  |    58     |

**_Sensor Data_**

|       | Temperature Sensor 1 | Temperature Sensor 2 | Magnetometer (x,y,z) | Gyrosensor (x,y,z) | Ambient Light | Solar Panel Power | Battery State Of Charge |
| :---: | :------------------: | :------------------: | :------------------: | :----------------: | :-----------: | :---------------: | :---------------------: |
| Bytes |          2           |          2           |          6           |         6          |      12       |        12         |            2            |
| Index |          14          |          16          |          18          |         24         |      30       |        42         |           54            |

**_Digipeater Uplink Packet_**

|       | Packet Type (0xF5) | Sender Callsign | Sat Callsign | Controll Bytes (0x55 0x55 0x55) | RS Bytes | Message |
| :---: | :----------------: | :-------------: | :----------: | :-----------------------------: | :------: | :-----: |
| Bytes |         1          |        6        |      6       |                3                |    32    |   42    |
| Index |         0          |        1        |      7       |               13                |    16    |   48    |

**_Digipeater Downlink Packet_**

|       | Packet Type (0xFF) | Sat Callsign | Sender Callsign | Ligitimizer (0x47) | Message | CRC-CCITT | RS Bytes |
| :---: | :----------------: | :----------: | :-------------: | :----------------: | :-----: | :-------: | :------: |
| Bytes |         1          |      6       |        6        |         1          |   42    |     2     |    32    |
| Index |         0          |      1       |        7        |         13         |   14    |    56     |    58    |

**_SSDV Image packet_**

Standard SSDV packet format specified by UKHAS (https://ukhas.org.uk/guides:ssdv) will be followed for sending the images captured. However, the first byte i.e. sync byte in the packet will be skipped, so as to make the packet length 255 byte instead of 256 as 255 is the maximum length of packet that the used tranciever can accomodate. The recievers must add the sync byte (0x55) before forwarding the packets to the SSDV servers or running the packets through SSDV decoders.

**Preamble and syncword**

64 alternating bits as preamble and CCSDS Sync word **0x1ACFFC1D** will be added before all of the above defined packets by the transciever. While doing uplink, one must either add these in the string being uplinked or, should set the tranciever modules to accordingly to add these to the packets.

**Reed-solomon FEC**

The FEC used to encode all the above mentioned packets is Reed-Solomon FEC implementation by [Philip Heron](https://github.com/fsphil) in [SSDV Project](https://github.com/fsphil/ssdv) and [Hadie Project](https://github.com/fsphil/hadie), which itself is based on portable implementation by [Phil Karn](http://www.ka9q.net/) available at http://www.ka9q.net/code/fec/ . The reciever implementers should take this into account and use the compatible decoder to avoid errors.

**References for converting Sensor data into readable format**

1. Ambient Light Sensor (OPT3001) :- [Datasheet](https://www.ti.com/lit/ds/symlink/opt3001.pdf?ts=1593455018360&ref_url=https%253A%252F%252Fwww.google.com%252F) (Refer page 20 ) [Implementation code](https://cdn.instructables.com/ORIG/FCI/JGX9/JFBMPFVZ/FCIJGX9JFBMPFVZ.ino) (Refer lines 38-44 )

2. Temperature Sensor (LM75) :- [Datasheet](https://datasheets.maximintegrated.com/en/ds/LM75.pdf) (Refer page 6)

3. Gyro Sensor (MPU-6050) :- [Datasheet](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf)

4. Magnetometer (HMC5883L) :- [Datasheet](https://cdn-shop.adafruit.com/datasheets/HMC5883L_3-Axis_Digital_Compass_IC.pdf)

5. Current/Power Sensor (INA219) :- [Datasheet](https://www.ti.com/lit/ds/symlink/ina219.pdf?ts=1593455980944&ref_url=https%253A%252F%252Fwww.google.com%252F)

6. Battery voltage and State of charge sensor (MAX17043G+U) :- [Datasheet](https://datasheets.maximintegrated.com/en/ds/MAX17043-MAX17044.pdf)
