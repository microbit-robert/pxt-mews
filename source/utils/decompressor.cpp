#include "pxt.h"
#include "decompressor.h"

// FUNCTION DEFINITIONS //
#include "pxt.h"
#include "decompressor.h"

bool decompress(const uint8_t* data, size_t data_len, uint8_t* out, size_t out_capacity, size_t* out_len){
  *out_len = 0;

  // IMA ADPCM always expands to exactly 2 output bytes per input byte
  // (each byte contains 2 nibbles, each nibble decodes to 1 u8 PCM sample)
  size_t required = data_len * 2;
  if(required > out_capacity){
    // Caller's buffer is too small — refuse rather than overflow
    return false;
  }

  int predictor = 0;
  int index = 0;
  int step = step_table[index];
  size_t pcm_pos = 0;

  for(size_t i = 0; i < data_len; i++){
    uint8_t byte = data[i];
    for(int nibble = 0; nibble < 2; nibble++){
      uint8_t code = (nibble == 0) ? (byte & 0x0F) : (byte >> 4);

      int diff = step >> 3;
      if(code & 1) diff += step >> 2;
      if(code & 2) diff += step >> 1;
      if(code & 4) diff += step;
      if(code & 8) diff = -diff;

      predictor += diff;
      if(predictor >  32767) predictor =  32767;
      if(predictor < -32768) predictor = -32768;

      index += index_table[code];
      if(index <  0) index =  0;
      if(index > 88) index = 88;
      step = step_table[index];

      // Convert signed 16-bit to unsigned 8-bit PCM
      out[pcm_pos++] = (uint8_t)((predictor + 32768) >> 8);
    }
  }

  *out_len = required;
  return true;
}
