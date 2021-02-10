// ANCHOR MACROS
#define i2c_addr_top_temp 0x4A
#define i2c_addr_bot_temp 0x4B
#define i2c_addr_ambient_light 0x44
#define i2c_addr_magnetometer 0x1E
#define i2c_addr_gyro 0x68 // NOTE Address of MPU6050 is 0x68;
#define i2c_addr_power 0x40
#define i2c_addr_fuel_guage ff0x36
#define north_enable_pin 18
#define LORA_BUSY 4
#define LORA_DIO1 10
#define LORA_NRST 0
#define LORA_NSS 3
#define LORA_DIO3 1
#define Camera_enable_pin 12
#define camera_cs_pin 13
#define normal_mode 0x11
#define safe_mode 0x05
#define auto_ssdv_mode 0xAA
#define Silent_mode 0XFF
#define image_quality 5 // NOTE try changing this from 0-7 and see which quality suits the speed;
#define packet_type_telem_pos 0
#define callsign_telem_pos 1
#define resets_telem_pos 7
#define packets_telem_pos 9
#define mode_telem_pos 13
#define temp1_telem_pos 14
#define temp2_telem_pos 16
#define magnetometer_telem_pos 18
#define gyro_telem_pos 24
#define ambient_light_telem_pos 30
#define power_telem_pos 42
#define bat_volt_telem_pos 52
#define soc_telem_pos 54
#define crc_telem_pos 56
#define rs_telem_pos 58
#define radioFreq 435.4
#define telemetry_packet_indicator 0xAB

// ANCHOR IMPORTS
#include <Wire.h>
#include <avr/wdt.h> // NOTE Issue in wdt.h file.
#include <util/crc16.h>
#include <Adafruit_INA219.h> // NOTE Added Adafruit_INA219 library
#include <EEPROM.h>
#include <RadioLib.h>
#include <ArduCAM.h>
#include "ssdv.h"
#include "rs8.h"
#include "key.h"

// ANCHOR VARIABLES
uint8_t telem_cmd_packet[90], currentmode = safe_mode; //start with safe modes
char callsign_bytes[6] = {'V', 'U', '3', 'O', 'I', 'R'};
uint8_t ccsds_sync_word[4] = {0x1A, 0xCF, 0xFC, 0x1D}, hi_chars[3] = {'H', 'i', '!'}, image_id = 0;
uint32_t no_of_packets = 0;
float battery_soc;
bool received_flag = 0;
uint32_t last_telemetry, last_ssdv;
SX1268 radio = new Module((uint8_t)LORA_NSS, (uint8_t)LORA_DIO1, (uint8_t)LORA_NRST, (uint8_t)LORA_BUSY);
ArduCAM Camera(OV2640, camera_cs_pin);

// ANCHOR STRUCTURES
struct EEprom
{
    bool deployed;
    uint16_t resets;
    bool silent_mode; // FIXME This is a previosly defined macro on line 19;
} flag;

// ANCHOR FUNCTIONS;

// ANCHOR get_temperature();
void get_temperature()
{
    Wire.beginTransmission(i2c_addr_top_temp);
    Wire.write(0x00); //temperature register
    Wire.endTransmission();
    delay(100);
    Wire.requestFrom(i2c_addr_top_temp, 2);
    telem_cmd_packet[temp1_telem_pos] = Wire.read();     //msb of temp
    telem_cmd_packet[temp1_telem_pos + 1] = Wire.read(); //lsb of temp
    wdt_reset();
    delay(100);
    //temp2
    Wire.beginTransmission(i2c_addr_bot_temp);
    Wire.write(0x00); //temperature register
    Wire.endTransmission();
    delay(100);
    Wire.requestFrom(i2c_addr_bot_temp, 2);
    telem_cmd_packet[temp2_telem_pos] = Wire.read();     //msb of temp
    telem_cmd_packet[temp2_telem_pos + 1] = Wire.read(); //lsb of temp
    wdt_reset();
}

// ANCHOR get_ambient_light();
void get_ambient_light()
{
    for (int i = 0; i < 6; i++) //toggles through all the sensors
    {
        digitalWrite(north_enable_pin + i, HIGH); //enables the sensor
        delay(100);                               //wait for the sensor to settle down before reading
        Wire.beginTransmission(i2c_addr_ambient_light);
        Wire.write(0x00); //result register
        Wire.endTransmission();
        delay(100);
        Wire.requestFrom(i2c_addr_ambient_light, 2);
        telem_cmd_packet[ambient_light_telem_pos + (2 * i)] = Wire.read();     //MSB of get_ambient_light
        telem_cmd_packet[ambient_light_telem_pos + (2 * i) + 1] = Wire.read(); //LSB of get_ambient_light
        wdt_reset();
        digitalWrite(north_enable_pin + i, LOW); //disables sensor
    }
}

// ANCHOR get_magnetometer();
void get_magnetometer()
{
    Wire.beginTransmission(i2c_addr_magnetometer);
    Wire.write(0x03); //starting address
    Wire.endTransmission();
    delay(100);
    Wire.requestFrom(i2c_addr_magnetometer, 6);
    for (int i = 0; i < 3; i++)
    {
        telem_cmd_packet[magnetometer_telem_pos + (2 * i)] = Wire.read();     //mag MSB
        telem_cmd_packet[magnetometer_telem_pos + (2 * i) + 1] = Wire.read(); //mag LSB
        wdt_reset();
    }
}

// ANCHOR get_gyro();
void get_gyro()
{
    Wire.beginTransmission(i2c_addr_gyro);
    Wire.write(0x43);
    Wire.endTransmission();
    delay(100);
    Wire.requestFrom(i2c_addr_gyro, 6);
    for (int i = 0; i < 3; i++)
    {
        telem_cmd_packet[gyro_telem_pos + (2 * i)] = Wire.read();     //gyro MSB
        telem_cmd_packet[gyro_telem_pos + (2 * i) + 1] = Wire.read(); //gyro LSB
        wdt_reset();
    }
}

// ANCHOR get_solar_power();
void get_solar_power()
{
    for (int i = 0; i < 5; i++)
    {
        digitalWrite(north_enable_pin + i, HIGH);
        delay(100);

        Adafruit_INA219 ina219;
        ina219.begin();
        ina219.setCalibration_16V_400mA();

        //configure
        Wire.beginTransmission(i2c_addr_power);
        Wire.write(0x00); //config register
        Wire.write(0x01);
        Wire.write(0x9F); //configuration calculated as (019F)H for 16V400ma range
        Wire.endTransmission();
        delay(100); //let the setting sink in
        Wire.beginTransmission(i2c_addr_power);
        Wire.write(0x03); //power register
        Wire.endTransmission();
        delay(100);
        Wire.requestFrom(i2c_addr_power, 2);
        telem_cmd_packet[power_telem_pos + (2 * i)] = Wire.read();     //MSB POWER
        telem_cmd_packet[power_telem_pos + (2 * i) + 1] = Wire.read(); //LSB POWER
        wdt_reset();
        digitalWrite(north_enable_pin + i, LOW); //disable the panel
    }
}

// ANCHOR get_battery_soc();
void get_battery_soc()
{
    Wire.beginTransmission(i2c_addr_fuel_guage);
    Wire.write(0x04); //soc register
    Wire.endTransmission();
    delay(100);
    Wire.requestFrom(i2c_addr_fuel_guage, 2);
    telem_cmd_packet[soc_telem_pos] = Wire.read();
    telem_cmd_packet[soc_telem_pos + 1] = Wire.read();
    wdt_reset();
    battery_soc = telem_cmd_packet[soc_telem_pos] + (float)(telem_cmd_packet[soc_telem_pos + 1] / 256.0);
    if (battery_soc <= 50.0 && (currentmode != safe_mode || currentmode != Silent_mode))
    {
        currentmode = safe_mode; // auto switch to safe mode when battery goes less than 50%
    }
    if (currentmode == safe_mode && battery_soc >= 85.0 && currentmode != Silent_mode)
    {
        currentmode = normal_mode; //auto switch to normal mode when battery goes more than 70%
    }
    wdt_reset();
}

// ANCHOR get_battery_volt();
void get_battery_volt()
{
    Wire.beginTransmission(i2c_addr_fuel_guage);
    Wire.write(0x02);
    Wire.endTransmission();
    delay(100);
    telem_cmd_packet[bat_volt_telem_pos] = Wire.read();
    telem_cmd_packet[bat_volt_telem_pos + 1] = Wire.read();
}

// ANCHOR get_sensor_values();
void get_sensor_values()
{
    get_temperature();
    get_ambient_light();
    get_magnetometer();
    get_gyro();
    get_solar_power();
    get_battery_soc();
    get_battery_volt();
    memcpy(&telem_cmd_packet[packets_telem_pos], &no_of_packets, 4);
    telem_cmd_packet[mode_telem_pos] = currentmode;
    memcpy(&telem_cmd_packet[resets_telem_pos], &flag.resets, 2);
    memcpy(&telem_cmd_packet[callsign_telem_pos], &callsign_bytes, 6);
}

// ANCHOR telemetry();
void telemetry()
{
    telem_cmd_packet[packet_type_telem_pos] = telemetry_packet_indicator; //indicates telemetry packet
    get_sensor_values();
    uint16_t crc = 0xffff;
    for (int i = 0; i < crc_telem_pos; i++)
    {
        crc = _crc_ccitt_update(crc, telem_cmd_packet[i]);
    }
    memcpy(&telem_cmd_packet[crc_telem_pos], &crc, 2);
    encode_rs_8(&telem_cmd_packet[0], &telem_cmd_packet[rs_telem_pos], (223 - 58));
    int output_power;
    currentmode == safe_mode ? output_power = 13 : output_power = 16;
    radio.beginFSK(radioFreq, 1.2, 0.3, 14.6, output_power, 140, 64, 0.5, 0); // FIXME The unexpected 140.
    radio.transmit(telem_cmd_packet, 90);
    delay(100);
    radio.startReceive();
}

// ANCHOR receive_interrupt();
void receive_interrupt()
{
    if (received_flag)
    {
        return;
    }
    received_flag = true;
}

// ANCHOR transmit_ssdv();
void transmit_ssdv()
{
    //Start camera and capture pic
    digitalWrite(Camera_enable_pin, HIGH);
    delay(1000); //let camera turn on properly
    //reset cpld of camera
    Camera.write_reg(0x07, 0x08);
    delay(100);
    Camera.write_reg(0x07, 0x00);
    delay(100);
    Camera.set_format(JPEG);
    Camera.InitCAM();
    Camera.clear_fifo_flag();
    Camera.OV2640_set_JPEG_size(OV2640_1600x1200);
    Camera.start_capture();
    //get sensors during camera capture proceeds and transmit
    telem_cmd_packet[packet_type_telem_pos] = 0xCD; //to indicate camera orientation frame
    get_sensor_values();
    uint16_t crc = 0xffff;
    for (int i = 0; i < crc_telem_pos; i++)
    {
        crc = _crc_ccitt_update(crc, telem_cmd_packet[i]);
    }
    encode_rs_8(&telem_cmd_packet[0], &telem_cmd_packet[rs_telem_pos], (223 - 58));
    radio.beginFSK(radioFreq, 1.2, 0.3, 14.6, 16, 140, 64, 0.5, 0);
    radio.transmit(telem_cmd_packet, 90);
    delay(2000);
    while (!(Camera.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)))
    {
    }
    //read fifo and convert to ssdv and send
    radio.beginFSK(radioFreq, 9.6, 2.4, 25, 16, 140, 64, 0.5, 0); //set to 9600 gmsk
    radio.fixedPacketLengthMode(255);                             //increases the packet size for ssdv transmissions
    uint8_t ssdv_packet[256], image_buffer[64];
    bool done = false;
    static ssdv_t ssdv;
    int r;
    uint8_t temp = 0, temp_last = 0;
    ssdv_enc_init(&ssdv, SSDV_TYPE_NORMAL, callsign_bytes, image_id++, image_quality);
    ssdv_enc_set_buffer(&ssdv, &ssdv_packet[0]);
    while (!done)
    {
        while ((r = ssdv_enc_get_packet(&ssdv)) == SSDV_FEED_ME)
        {
            Camera.CS_LOW();
            Camera.set_fifo_burst();
            int i = 0;
            while (i < 64) //take image 64 byte
            {
                temp_last = temp;
                temp = SPI.transfer(0x00);
                if ((temp == 0xD9) && (temp_last == 0xFF)) //check if it is end of the image
                {
                    image_buffer[i] = temp;
                    done = true;
                    digitalWrite(camera_cs_pin, HIGH);
                    digitalWrite(Camera_enable_pin, LOW); //shut the camera down after image is complete
                    break;
                }
                else
                {
                    image_buffer[i] = temp;
                }
                i++;
                wdt_reset();
            }
            ssdv_enc_feed(&ssdv, image_buffer, (i + 1));
            wdt_reset();
            radio.transmit(&ssdv_packet[1], 255); //we don't wont to send [0] as it is sync byte
            wdt_reset();
        }
        wdt_reset();
    }
    radio.fixedPacketLengthMode(90);
    radio.beginFSK(radioFreq, 1.2, 0.3, 14.6, 16, 140, 64, 0.5, 0);
    wdt_reset();
    radio.startReceive();
    wdt_reset();
}

// ANCHOR process_cmd();
void process_cmd()
{
    uint8_t major_part[48];
    memcpy(major_part, telem_cmd_packet, 48);
    int j = decode_rs_8(major_part, 0, 0, 223 - 16);
    if (j < 0)
    {
        return;
    } //discard uplink if the correction fails
    int n = memcmp(&major_part[7], callsign_bytes, 6);
    if (n != 0)
    {
        return;
    }                      //check if the uplink is for the same satellite if not then discard
    switch (major_part[0]) //switch as per the packet packet_type
    {
    case 0xF5: //in case it is digipeater uplink
    {
        if (currentmode != safe_mode)
        {
            //construct digipeater downlink packet
            telem_cmd_packet[packet_type_telem_pos] = 0xFF;                 //digipeater downlink packet indicator
            memcpy(&telem_cmd_packet[1], callsign_bytes, 6);                //put satellite callsign
            memcpy(&telem_cmd_packet[7], &major_part[1], 6);                //put sender callsign
            memcpy(&telem_cmd_packet[13], hi_chars, 3);                     //say hi before the message
            memcpy(&telem_cmd_packet[16], &telem_cmd_packet[48], 42);       //copy the message in place
            encode_rs_8(telem_cmd_packet, &telem_cmd_packet[58], 223 - 58); //get rs parity
            wdt_reset();
            radio.transmit(telem_cmd_packet, 90); //transmit the digipeater downlink
            delay(50);
            radio.startReceive(); //put the transceiver in reception mode again
            delay(50);
            wdt_reset();
        }
        return;
    }
    case 0xEF: //if it is internal command
    {
        //decrypting the command
        major_part[15] ^= private_key;
        major_part[14] ^= major_part[15];
        major_part[13] ^= major_part[14];
        wdt_reset();
        if (major_part[13] == 0x15 && major_part[14] == 0x12) //command to reset satellite
        {
            while (1)
            {
            } //make an infinite loop so that watchdog resets the MCU
        }
        if (major_part[13] == 0xAA && major_part[14] == 0xBD) // command for transmitting ssdv once
        {

            transmit_ssdv();
            wdt_reset();
            return;
        }
        if (major_part[13] == 0xCD && major_part[14] == 0x99) //command to shift to automatic ssdv mode
        {
            if (currentmode == safe_mode)
            {
                return;
            }
            currentmode = auto_ssdv_mode; //shift to automatic ssdv mode
            wdt_reset();
            return;
        }
        if (major_part[13] == 0xAB && major_part[14] == 0xCD) //command to shift to normal mode
        {
            currentmode = normal_mode; //shift to normal mode
            wdt_reset();
            return;
        }
        if (major_part[13] == 0xDE && major_part[14] == 0x55) //command to shift to safe mode
        {
            currentmode = safe_mode; //shift to safe mode
            wdt_reset();
            return;
        }
        if (major_part[13] == 0x55 && major_part[14] == 0xAA) //shift to silent mode
        {
            currentmode = Silent_mode;
            wdt_reset();
            flag.silent_mode = true;
            EEPROM.put(0x00, flag);
            return;
        }
        if (major_part[13] == 0xAA && major_part[14] == 0x55) //turn on transmitter
        {
            currentmode = normal_mode;
            wdt_reset();
            return;
        }
    }
    default:
        return;
    }
}

// ANCHOR VOID SETUP();
void setup()
{
    // EEPROM.get(flag, 0x00); // FIXME Error in EEPROM.get() arguments;

    SPI.begin();

    //Set EEPROM Flags
    EEPROM.get(0x00, flag);
    flag.resets += 1;
    EEPROM.put(0x00, flag);
    Wire.begin();

    //initialize the magnetometer in continuous mode
    Wire.beginTransmission(i2c_addr_magnetometer);
    Wire.write(0x02); //the mode register
    Wire.write(0x00); //continuous mode
    Wire.endTransmission();
    delay(100);

    //Wake up MPU6050 gyro
    Wire.beginTransmission(i2c_addr_gyro);
    Wire.write(0x6B); //power management register
    Wire.write(0x00); //wakes up
    Wire.endTransmission();
    delay(100);

    wdt_enable(WDTO_8S); // NOTE Fixed unresolved import in wdt library;

    radio.setCRC(0);
    radio.setSyncWord(ccsds_sync_word, 4);
    radio.fixedPacketLengthMode(90);
    radio.setDio1Action(receive_interrupt); // NOTE Fixed function call
    if (flag.silent_mode)
    {
        currentmode = Silent_mode;
    }
}

// ANCHOR VOID LOOP();
void loop()
{
    if (received_flag)
    {
        radio.readData(telem_cmd_packet, 90);
        wdt_reset();
        process_cmd();
        wdt_reset();
        received_flag = false;
    }
    switch (currentmode)
    {
    case normal_mode:
    {
        if (abs(last_telemetry - millis()) >= 30000)
        {
            wdt_reset();
            telemetry();
            wdt_reset();
            last_telemetry = millis();
            break;
        }
    }
    case safe_mode:
    {
        if (abs(last_telemetry - millis()) >= 55000)
        {
            wdt_reset();
            telemetry();
            wdt_reset();
            last_telemetry = millis();
            break;
        }
    }
    case auto_ssdv_mode:
    {
        if (abs(last_telemetry - millis()) >= 30000)
        {
            wdt_reset();
            telemetry();
            wdt_reset();
            last_telemetry = millis();
        }
        if (abs(last_ssdv - millis()) >= 600000)
        {
            wdt_reset();
            transmit_ssdv();
            wdt_reset();
            last_ssdv = millis();
            break;
        }
    }
    }
}
