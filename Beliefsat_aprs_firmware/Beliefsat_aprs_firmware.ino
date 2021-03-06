#include <LibAPRS.h>
#include <SoftwareSerial.h>

#define TEPERATURE_SESOR_PIN 2
#define SOFTWARE_SERIAL_RX_PIN 8
#define SOFTWARE_SERIAL_TX_PIN 9
#define CAMERA_NSS_PIN 10
#define CAMERA_ENABLE_PIN A2
#define ANTENNA_DEPLOYMENT_PIN A3

#define ADC_REFERENCE REF_3V3
#define OPEN_SQUELCH false
#define SAT_CALLSIGN "VU2CWN"
#define TO_CALL "CQ"
#define SAT_SSID 11
#define DIGI_SSID 6
#define GS_SSID 1

#define debug
#define TELEMETRY_INTERVAL 30000 //miliseconds

#define COMMAND_INDICATOR '!'
#define DIGI_TURN_ON_TYPE '@'
#define DIGI_TURN_OFF_TYPE '#'
#define TX_TURN_ON_TYPE '$'
#define TX_TURN_OFF_TYPE '%'
#define SSDV_CAPTURE_TYPE '^'

bool digipeater_is_on = false;
bool tx_is_on = true;
bool is_sending_ssdv = false;
boolean gotPacket = false;
AX25Msg incomingPacket;
uint8_t *packetData;
long last_telemetry;
SoftwareSerial vhf_port(SOFTWARE_SERIAL_RX_PIN, SOFTWARE_SERIAL_TX_PIN);

void aprs_msg_callback(struct AX25Msg *msg) {
  if (!gotPacket) {
    gotPacket = true;
    memcpy(&incomingPacket, msg, sizeof(AX25Msg));
    if (freeMemory() > msg->len) {
      packetData = (uint8_t *)malloc(msg->len);
      memcpy(packetData, msg->info, msg->len);
      incomingPacket.info = packetData;
    } else {
      gotPacket = false;
    }
  }
}

void setup() {
  Serial.begin(115200);
  vhf_port.begin(9600);
  vhf_port.write(F("AT+DMOSETGROUP=0,145.8250,145.8250,0000,0,0000\r\n"));
#ifdef debug
  Serial.println(vhf_port.readString());
#endif
  APRS_init(ADC_REFERENCE, OPEN_SQUELCH);
  APRS_setCallsign(SAT_CALLSIGN, SAT_SSID);
  APRS_setDestination(TO_CALL, 0);
  APRS_setPreamble(500);
  APRS_setTail(100);
#ifdef debug
  APRS_printSettings();
#endif
  last_telemetry=millis();
}

void processPacket() {
  if (gotPacket) {
    gotPacket = false;

#ifdef debug
    for (int i = 0; i < incomingPacket.len; i++) {
      Serial.write(incomingPacket.info[i]);
    }
    Serial.println("");
#endif
    if (incomingPacket.src.call == SAT_CALLSIGN &&
        incomingPacket.src.ssid == GS_SSID &&
        incomingPacket.dst.call == SAT_CALLSIGN &&
        incomingPacket.dst.ssid == SAT_SSID &&
        packetData[0] == COMMAND_INDICATOR) {
      switch (packetData[1]) {
        case DIGI_TURN_ON_TYPE:
          {
            digipeater_is_on = true;
            break;
          }
        case DIGI_TURN_OFF_TYPE:
          {
            digipeater_is_on = false;
            break;
          }
        case TX_TURN_ON_TYPE:
          {
            tx_is_on = true;
            break;
          }
        case TX_TURN_OFF_TYPE:
          {
            tx_is_on = false;
            break;
          }
        case SSDV_CAPTURE_TYPE:
          {
            if (!is_sending_ssdv && tx_is_on) //if we are laready sensing ssdv we shouldn't capture new pic in middle
            {
              digitalWrite(CAMERA_ENABLE_PIN, HIGH); //turn on camera
              delay(1000);
              is_sending_ssdv = true;
              break;
            }
          }
      }
    } else {
      if (digipeater_is_on && tx_is_on) {
        bool do_repeat = false;
        for (int i = 0; i < incomingPacket.rpt_count; i++) {
          if (!strcmp(incomingPacket.rpt_list[i].call, "ARISS") ||
              !strcmp(incomingPacket.rpt_list[i].call, SAT_CALLSIGN)) {
            do_repeat = true;
          }
        }
        if (do_repeat) {
          APRS_setCallsign(incomingPacket.src.call, incomingPacket.src.ssid);
          APRS_setPath1(SAT_CALLSIGN, (DIGI_SSID | 0x40));
          APRS_sendPkt(packetData, incomingPacket.len);
          APRS_setCallsign(SAT_CALLSIGN, SAT_SSID);
          APRS_setPath1("WIDE1", 1);
        }
      }
      free(packetData);
    }
  }
}
void loop() {
  processPacket();
  if(is_sending_ssdv)
  {
    send_next_ssdv_packet();
  }
  if(millis()-last_telemetry>TELEMETRY_INTERVAL && tx_is_on)
  {
    telemtry();
  }
}
void telemtry()
{

}
void send_next_ssdv_packet()
{
  // make sure to make  is_sending_ssdv = false if this is last ssdv packet
}