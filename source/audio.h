#ifndef AUDIO_H
#define AUDIO_H

#include "pxt.h"
#include <map>
#include <string>

struct Audio_Data{
  uint8_t* data;
  size_t size;
  int sample_rate;
};

namespace audio{
  extern std::map<std::string, Audio_Data> data_map;
  void add_audio(String name, Buffer data, int data_len, int sample_rate);
}

#endif