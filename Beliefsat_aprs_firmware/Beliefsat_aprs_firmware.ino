#include <Adafruit_INA219.h>
#include <Adafruit_MPU6050.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include <LibAPRS.h>
#include <OneWire.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <avr/wdt.h>

#define ADC_REFERENCE REF_3V3
#define OPEN_SQUELCH true
#define debug

#define SATCALL_SIGN "VU2CWN"
#define GSCALL_SIGN "VU3OIR"
#define SATTELEM_SSID 11
#define SATDIGI_SSID 6
#define GS_SSID 1
#define FIRST_BOOT_DELAY 1800     // seconds
#define RESISTOR_BURN_DELAY 10000 // milliseconds
#define TELEMETRY_INTERVAL 60000  // milliseconds
#define APRS_TOCALL "CQ"

#define ANTENNA_DEPLOYMENT_PIN (8)
#define UHF_POWER_DOWN_PIN (9)
#define UHF_RX (12)
#define UHF_TX (13)
#define VHF_RX (10)
#define VHF_TX (11)
#define TEMPERATURE_SENSOR_PIN 2

String temp_sensor_1 = "";
String temp_sensor_2 = "";
String gyro_sensor_1 = "";
String gyro_sensor_2 = "";
String gyro_sensor_3 = "";
String current = "";
String battery_voltage = "";

#define VHF_SETUP_STRING "AT+DMOSETGROUP=0,145.8250,145.8250,0000,0,0000\r\n"
#define UHF_SETUP_STRING "AT+DMOSETGROUP=0,435.6000,435.6000,0000,1,0001\r\n"
#define TURN_OFF_VOICE_REPEATER_INPUT digitalWrite(UHF_POWER_DOWN_PIN, LOW)
#define TURN_ON_VOICE_REPEATER_INPUT                                           \
  do {                                                                         \
    digitalWrite(UHF_POWER_DOWN_PIN, HIGH);                                    \
    uhf_port.write(F(UHF_SETUP_STRING));                                       \
  } while (0)

#define TOTAL_RESETS_EEPROM_ADDR 0x00
#define WD_INDUCED_RESETS_EEPROM_ADDR 0x02
#define TX_STATUS 0x04

#define COMMAND_INDICATOR '!'

OneWire oneWire(TEMPERATURE_SENSOR_PIN);
DallasTemperature sensors(&oneWire);
SoftwareSerial vhf_port(VHF_RX, VHF_TX);
SoftwareSerial uhf_port(UHF_RX, UHF_TX);

boolean gotPacket = false;
AX25Msg incomingPacket;
uint8_t *packetData;
boolean digipeater_is_on = false;
boolean voice_repeater_is_on = false;
boolean transmission_is_on;
uint16_t total_resets;
uint16_t wd_induced_resets;
long last_telemetry_time;

// initialize sensor objects
Adafruit_MPU6050 mpu;
Adafruit_INA219 ina219;

String num_to_string(uint8_t X)
{
  String str = "";
  if(X < 10)
    {
      String str = "00" + String(X, DEC);
    }
    if(X < 100 && X > 9)
    {
      String str = "0" + String(X, DEC);
    }
    if(X >= 100)
    {
      String str = String(X, DEC);
    }
   return str;
}

String float_to_string(float value){
    long X,temp = 0;
    X = (long)(value * 100);
    temp = X % 100;
    X = X - temp;
    String str = num_to_string(X);
    return str;
}

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

ISR(WDT_vect) {
  wd_induced_resets++;
  EEPROM.write(WD_INDUCED_RESETS_EEPROM_ADDR, wd_induced_resets);
}

void setup() {
#ifdef debug
  Serial.begin(9600);
#endif
  watchdogSetup();
  EEPROM.get(TOTAL_RESETS_EEPROM_ADDR, total_resets);

  if (total_resets < 1) {
#ifdef debug
    Serial.println(F("First_boot_delay started"));
#endif
    for (int i = 0; i < FIRST_BOOT_DELAY; i++) {
      delay(1000);
      wdt_reset();
    }
    pinMode(ANTENNA_DEPLOYMENT_PIN, OUTPUT);
    digitalWrite(ANTENNA_DEPLOYMENT_PIN, HIGH);
    delay(RESISTOR_BURN_DELAY / 2);
    wdt_reset();
    delay(RESISTOR_BURN_DELAY / 2);
    digitalWrite(ANTENNA_DEPLOYMENT_PIN, LOW);
  }

  total_resets++;
  EEPROM.put(TOTAL_RESETS_EEPROM_ADDR, total_resets);
  EEPROM.get(WD_INDUCED_RESETS_EEPROM_ADDR, wd_induced_resets);
  EEPROM.get(TX_STATUS, transmission_is_on);

  pinMode(UHF_POWER_DOWN_PIN, OUTPUT);

  vhf_port.begin(9600);
  uhf_port.begin(9600);

  vhf_port.write(F(VHF_SETUP_STRING));
  wdt_reset();
  APRS_init(ADC_REFERENCE, OPEN_SQUELCH);
  APRS_setCallsign(SATCALL_SIGN, SATTELEM_SSID);
  APRS_setDestination(APRS_TOCALL, 0);

#ifdef debug
  APRS_printSettings();
  Serial.print(F("Free RAM:     "));
  Serial.println(freeMemory());
#endif
  Wire.begin();
  ////////////////////////////////////
  // Sensor initializations
  wdt_reset();

  //MPU6050
  mpu.begin();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  //INA219
  ina219.begin();

  //Dallas temperature sensor
  
  ////////////////////////////////////
  last_telemetry_time = millis();
}

void processPacket() {
  if (gotPacket) {
    gotPacket = false;

#ifdef debug
    Serial.print(F("Received APRS packet. SRC: "));
    Serial.print(incomingPacket.src.call);
    Serial.print(F("-"));
    Serial.print(incomingPacket.src.ssid);
    Serial.print(F(". DST: "));
    Serial.print(incomingPacket.dst.call);
    Serial.print(F("-"));
    Serial.print(incomingPacket.dst.ssid);
    Serial.print(F(". Data: "));

    for (int i = 0; i < incomingPacket.len; i++) {
      Serial.write(incomingPacket.info[i]);
    }
    Serial.println("");
#endif
    if (incomingPacket.src.call == SATCALL_SIGN &&
        incomingPacket.src.ssid == GS_SSID &&
        incomingPacket.info[0] == COMMAND_INDICATOR &&
        incomingPacket.dst.call == SATCALL_SIGN &&
        incomingPacket.dst.ssid == SATTELEM_SSID) {
      switch (incomingPacket.info[1]) {
      case '@': {
        // turn on digipeater
        TURN_OFF_VOICE_REPEATER_INPUT;
        digipeater_is_on = true;
        break;
      }
      case '#': {
        // turn off digipeater
        digipeater_is_on = false;
        break;
      }
      case '$': {
        // turn on voice repeater
        if (transmission_is_on) {
          digipeater_is_on = false;
          voice_repeater_is_on = true;
          TURN_ON_VOICE_REPEATER_INPUT;
        }
        break;
      }
      case '%': {
        // turn off voice repeater
        TURN_OFF_VOICE_REPEATER_INPUT;
        voice_repeater_is_on = false;
        break;
      }
      case '^': {
        // turn of the trasmissions
        TURN_OFF_VOICE_REPEATER_INPUT;
        digipeater_is_on = false;
        transmission_is_on = false;
        EEPROM.put(TX_STATUS, transmission_is_on);
        break;
      }
      case '&': {
        // turn on transmissions
        transmission_is_on = true;
        EEPROM.put(TX_STATUS, transmission_is_on);
        break;
      }
      case '*': {
        // reset the satellite by trigering the watch dog
        while (1)
          ;
        break;
      }
      }
    } else {
      if (digipeater_is_on && transmission_is_on) {
        boolean do_repeat = false;
        for (int i = 0; i < incomingPacket.rpt_count; i++) {
          if (!strcmp(incomingPacket.rpt_list[i].call, "ARISS") ||
              !strcmp(incomingPacket.rpt_list[i].call, SATCALL_SIGN)) {
            do_repeat = true;
          }
        }
        if (do_repeat) {
          APRS_setCallsign(incomingPacket.src.call, incomingPacket.src.ssid);
          APRS_setPath1(SATCALL_SIGN, (SATDIGI_SSID | 0x40));
          APRS_sendPkt(&incomingPacket.info[0], incomingPacket.len);
          APRS_setCallsign(SATCALL_SIGN, SATTELEM_SSID);
          APRS_setPath1("WIDE1", 1);
        }
      }
      free(packetData);
    }
  }
}

void loop() {
  wdt_reset();
  processPacket();
  if (millis() - last_telemetry_time >= TELEMETRY_INTERVAL &&
      transmission_is_on) {
    send_telem();
  }
}
void watchdogSetup(void) {
  cli();
  wdt_reset();
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = (1 << WDIE) | (1 << WDE) | (1 << WDP3) | (0 << WDP2) | (0 << WDP1) |
           (1 << WDP0);
  sei();
}

void get_gyro()
{
  //code to get gyro sensor values
  wdt_reset();
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float w_x = (a.acceleration.x) * (g.gyro.x-0.02148);
  float w_y = (a.acceleration.y) * (g.gyro.y+0.03639);
  float w_z = (a.acceleration.z) * (g.gyro.z-0.00652);

  gyro_sensor_1 = float_to_string(w_x);
  gyro_sensor_2 = float_to_string(w_y);
  gyro_sensor_3 = float_to_string(w_z);
}

void get_temperature()
{
  //code to get temperature sensor values
  wdt_reset();
}

void get_current()
{
  //code to get current
  wdt_reset();
  float busvoltage = 0;
  float current_mA = 0;

  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();

  current = float_to_string(current_mA);
  battery_voltage = float_to_string(busvoltage);
}

void get_sensor_values()
{
  get_temperature();
  get_gyro();
  get_current();
}

void send_telem() {
  wdt_reset();
  TURN_OFF_VOICE_REPEATER_INPUT;
  EEPROM.get(TX_STATUS, transmission_is_on);
  if (voice_repeater_is_on && transmission_is_on) {
    TURN_ON_VOICE_REPEATER_INPUT;
  }
  if(transmission_is_on && millis() - last_telemetry_time >= TELEMETRY_INTERVAL)
  {
    // Logic for seding telemetry
    // APRS TELEMETRY FORMAT: The on-air packet telemetry format is as follows:
    // sss = total number of resets LSB
    // 111 = total number of resets MSB
    // 222 = watchdog induced resets LSB
    // 333 = watchdog induced resets MSB
    // 444, 555 = temperature sensor 1, 2 resp
    // 666, 777, 888 = gyrosensor 1, 2, 3 resp
    // 999 = current
    // AAA = battery voltage
    // xxxxxx10, xxxxxx01 = digipeater on, voice repeater on resp

    get_sensor_values();
  
    EEPROM.get(TOTAL_RESETS_EEPROM_ADDR, total_resets);
    uint8_t total_resets_lsb = (total_resets & 0x00FF);
    uint8_t total_resets_msb = ((total_resets & 0xFF00) >>8);
    String sss = num_to_string(total_resets_lsb);
    String resets_msb = num_to_string(total_resets_msb);
    EEPROM.get(WD_INDUCED_RESETS_EEPROM_ADDR, wd_induced_resets);
    uint8_t wd_induced_resets_lsb = (wd_induced_resets & 0x00FF);
    uint8_t wd_induced_resets_msb = ((wd_induced_resets & 0xFF00) >>8);
    String wd_lsb = num_to_string(wd_induced_resets_lsb);
    String wd_msb = num_to_string(wd_induced_resets_msb);
  
    int digipeater_status = 0;
    int voice_repeater_status = 0;
    if(digipeater_is_on)
    {
      digipeater_status = 1;
    }
    else
    {
      digipeater_status = 0;
    }
    if(voice_repeater_is_on)
    {
      voice_repeater_status = 1;
    }
    else
    {
      voice_repeater_status = 0;
    }
    String telem_packet = "t#" + sss + "," + resets_msb + "," + wd_lsb + "," + wd_msb + "," + temp_sensor_1 + "," + temp_sensor_2 + "," + gyro_sensor_1 + "," + gyro_sensor_2 + "," + gyro_sensor_3 + "," + current + "," + battery_voltage + "," + "xxxxxx" + String(digipeater_status, DEC) + String(voice_repeater_status, DEC);
    char buf[telem_packet.length()] = "";
    telem_packet.toCharArray(buf, telem_packet.length());
    wdt_reset();
    APRS_setMessageDestination(GSCALL_SIGN, GS_SSID);
    APRS_sendMsg(buf, strlen(buf));
  }
  wdt_reset();
  last_telemetry_time = millis();
}
