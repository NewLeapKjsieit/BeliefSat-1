#define VERSION "1.1"

#include <inttypes.h>
#include <Wire.h>
#include <LM75.h>

TPempI2C_LM75 termo = TempI2C_LM75(0x48, TempI2C_LM75::nine_bits);

void setup()
{
	Serial.begin(9600);
	Serial.println("Start");
	Serial.print("Actual temp ");
	Serial.print(termo.getTemp());
	Serial.println(" oC");
	delay(2000);
}

void loop()
{
	Serial.print(termo.getTemp());
	Serial.println(" oC");
	delay(5000);
}
