#include <RadioLib.h>
#include <util/crc16.h>
#include "rs8.h"
SX1268 radio = new Module(7,2,3,4);
uint8_t ccsds_sync_word[4]={0x1A,0xCF,0xFC,0x1D};
uint8_t packet[90];
uint8_t callsign[6]={'V','U','3','O','I','R'};
uint16_t resets =5;
uint32_t packet_no =0;
uint8_t mode =0x11;
uint16_t uptemp=0x0F10;
uint16_t downtemp=0xFF80;
uint16_t mag[3]={0x07FF,0xF800,0x0333};
uint16_t gyro[3]={0xF5AF,0xF800,0x13A3};
uint16_t ambient_light[6]={0x001F,0x0101,0x0010,0xBFFF,0x111D,0xFFAA};
uint16_t solar_power[5]={0x001F,0x0101,0x0010,0xBFFF,0x111D};
uint16_t battery_voltage=0xDAFA;
uint16_t battery_soc=0xA1B2;
void setup()
{
radio.beginFSK(435.5,1.2,0.3,14.6,0,140,64,0.5,3.3);
radio.setCRC(0);
radio.setSyncWord(ccsds_sync_word,4);
radio.fixedPacketLengthMode(90);
Serial.begin(9600);
Serial.println("Hello");

}

void loop() {
packet_no++;
packet[0]=0xAB;//packettype
memcpy(&packet[1],&callsign[0],6); //callsign
memcpy(&packet[7],&resets,2);
memcpy(&packet[9],&packet_no,4);
memcpy(&packet[13],&mode,1);
memcpy(&packet[14],&uptemp,2);
memcpy(&packet[16],&downtemp,2);
memcpy(&packet[18],&mag,6);
memcpy(&packet[24],&gyro,6);
memcpy(&packet[30],&ambient_light,12);
memcpy(&packet[42],&solar_power,10);
memcpy(&packet[52],&battery_voltage,2);
memcpy(&packet[54],&battery_soc,2);
uint16_t crc=0xffff;
for(int i=0;i<56;i++)
{crc= _crc_ccitt_update(crc,packet[i]);}
memcpy(&packet[56],&crc,2);
encode_rs_8(&packet[0],&packet[58],(223-58));
radio.transmit(packet,90);
Serial.println("Hello");
delay(3000);
}
