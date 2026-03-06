namespace tts{
  void announceWord(String speak_text, String display_text);

  //%
  void announceWord(String speak_text, String display_text){
    #if MICROBIT_CODAL
    Audio_Data audio_obj = audio::data_map[speak_text->getUTF8Data()];
    if(audio_obj.data == nullptr){
      uBit.display.scroll("NO KEY");
      return;
    }
    size_t out_size = 0;
    uint8_t* audio_data = decompress(audio_obj.data, audio_obj.size, &out_size);
    if(out_size == 0 || audio_data == nullptr){
      uBit.display.scroll("ERR");
      return;
    }
    init_timer(audio_obj.sample_rate);
    uBit.display.scrollAsync(ManagedString(display_text->getUTF8Data()));
    play_wav(audio_data, out_size);
    free(audio_data);
    #else
    uBit.display.scroll(ManagedString(display_text->getUTF8Data()));
    #endif
  }
}