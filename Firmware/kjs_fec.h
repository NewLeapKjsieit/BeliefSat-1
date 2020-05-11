#include <stdint.h>
#include <stddef.h>

#define CCSDS_CONV_CODE_RATE (1/2)
#define CCSDS_CONV_CONSTRAINT_LENGTH_BITS (7)
#define CODEWORD_SIZE_BITS  255
#define CODEWORD_SIZE_BYTES 32
#define _RS8_H

#ifdef __cplusplus
  extern "C" {
#endif
void
ccsds_scrambler(uint8_t *data, size_t len);

void
ccsds_descrambler(uint8_t *data, size_t len);

void
conv_encoder_1_2_7(uint8_t *out, const uint8_t *data, size_t len);

extern void encode_rs_8(uint8_t *data, uint8_t *parity, int pad);

extern int decode_rs_8(uint8_t *data, int *eras_pos, int no_eras, int pad);

#ifdef __cplusplus
  }
#endif