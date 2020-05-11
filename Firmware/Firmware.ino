#include "kjs_fec.h"
#include <avr/wdt.h>
void setup()
{
    uint8_t data[100],output[200];
    for(int i=4;i<68;i++)
    {
        data[i]='a'+i;
    }
    encode_rs_8(&data[4],&data[68],223-64);
    ccsds_scrambler(&data[4],64);
    conv_encoder_1_2_7(data,output,100);
   decode_rs_8(&data[4],0,0,223-64);
}
void loop()
{}
