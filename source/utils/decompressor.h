#ifndef __DECOMPRESSOR_H
#define __DECOMPRESSOR_H

static const int index_table[16] = {
  -1,-1,-1,-1,2,4,6,8,
  -1,-1,-1,-1,2,4,6,8
};

static const int step_table[89] = {
  7,8,9,10,11,12,13,14,16,17,
  19,21,23,25,28,31,34,37,41,45,
  50,55,60,66,73,80,88,97,107,118,
  130,143,157,173,190,209,230,253,279,307,
  337,371,408,449,494,544,598,658,724,796,
  876,963,1060,1166,1282,1411,1552,1707,1878,2066,
  2272,2499,2749,3024,3327,3660,4026,4428,4871,5358,
  5894,6484,7132,7845,8630,9493,10442,11487,12635,13899,
  15289,16818,18500,20350,22385,24623,27086,29794,32767
};

/**
 * Decompresses IMA ADPCM audio data into a caller-provided buffer.
 * @param data         The compressed input data.
 * @param data_len     The length of the compressed data.
 * @param out          Caller-provided output buffer to write PCM into.
 * @param out_capacity The size of the output buffer in bytes.
 * @param out_len      Set to the number of bytes written on success, 0 on failure.
 * @returns true on success, false if the output buffer is too small.
 */
bool decompress(const uint8_t* data, size_t data_len, uint8_t* out, size_t out_capacity, size_t* out_len);

#endif