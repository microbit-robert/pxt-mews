// IMPORTS //
#include "pxt.h"
#include "MicroBit.h"
#include "utils/decompressor.h"
#include <string>
using namespace pxt;

// FUNCTION DECLARATIONS //
/**
 * Initialises an NRF Timer to activate at a given rate.
 * @param sample_rate The rate at which to activate the timer, matching the sample rate of the audio being played.
 */
void init_timer(int sample_rate);
/**
 * Initialises Pulse-Width-Modulation on the MicroBit.
 */
void init_pwm();
/**
 * Plays wav data through the Jiangsu Huaneng MLT-8530 Speaker.
 * @param audio_data The audio data to play through the speaker.
 * @param out_size The size of the data being played.
 */
void play_wav(uint8_t* audio_data, size_t out_size);

/**
 * Plays an individual audio sample through the Jiangsu Huaneng MLT-8530 Speaker.
 * @param sample The sample to play through the speaker. 
 */
void play_sample(uint16_t sample);

#if MICROBIT_CODAL
// FUNCTION DEFINITIONS //
void play_wav(uint8_t* audio_data, size_t out_size){
  init_pwm();
  
  for(unsigned int i=0; i<out_size/sizeof(uint8_t); i++){
    NRF_TIMER0->TASKS_CLEAR=(TIMER_TASKS_CLEAR_TASKS_CLEAR_Trigger<<TIMER_TASKS_CLEAR_TASKS_CLEAR_Pos);                         // Resets the timer
    NRF_TIMER0->EVENTS_COMPARE[0]=(TIMER_EVENTS_COMPARE_EVENTS_COMPARE_NotGenerated<<TIMER_EVENTS_COMPARE_EVENTS_COMPARE_Pos);  // Resets the comparison check
    
    //Play Sample (Not Functioning Correctly)
    play_sample((uint16_t)audio_data[i]);
    
    // Wait for timer to finish
    while(NRF_TIMER0->EVENTS_COMPARE[0] == (TIMER_EVENTS_COMPARE_EVENTS_COMPARE_NotGenerated << TIMER_EVENTS_COMPARE_EVENTS_COMPARE_Pos));
  }
  
  NRF_TIMER0->TASKS_STOP = (TIMER_TASKS_STOP_TASKS_STOP_Trigger<<TIMER_TASKS_STOP_TASKS_STOP_Pos);                              // Stops the timer
  NRF_PWM0->ENABLE = (PWM_ENABLE_ENABLE_Disabled << PWM_ENABLE_ENABLE_Pos);                                                     // Disables the timer    
}

// Potentially Deprecated (if play_sequence() works)
void play_sample(uint16_t sample){
  static volatile uint16_t sequence[1];
  sequence[0] = (sample * NRF_PWM0->COUNTERTOP) / 255;                                                                // Defines a sequence
  
  NRF_PWM0->SEQ[0].PTR = ((uint32_t)sequence << PWM_SEQ_PTR_PTR_Pos);                                                 // Finds the start of the sequence 
  NRF_PWM0->SEQ[0].CNT = (1 << PWM_SEQ_CNT_CNT_Pos);                                                                  // Defines the size of the sequence as 1 
  NRF_PWM0->TASKS_SEQSTART[0] = (PWM_TASKS_SEQSTART_TASKS_SEQSTART_Trigger << PWM_TASKS_SEQSTART_TASKS_SEQSTART_Pos); // Enables the Sequence
}

void init_timer(int sample_rate){
  // Timer Initialisation
  NRF_TIMER0->PRESCALER = (4UL << TIMER_PRESCALER_PRESCALER_Pos);                                         // Sets the prescaler to divide the clock by 4
  NRF_TIMER0->BITMODE = (TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos);                       // Sets the bitmode to 32bits
  
  NRF_TIMER0->CC[0] = 1000000/sample_rate;                                                                // Sets how long to wait for before returning True
  NRF_TIMER0->TASKS_START = (TIMER_TASKS_START_TASKS_START_Trigger << TIMER_TASKS_START_TASKS_START_Pos); // Starts the timer
}

void init_pwm(){
  NRF_PWM0->PSEL.OUT[0] = (MICROBIT_PIN_SPEAKER << PWM_PSEL_OUT_PIN_Pos) |                // Maps the first pin to the speaker
  (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);                           // Connects the pin
  
  NRF_PWM0->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);                // Enables PWM
  
  NRF_PWM0->DECODER = (PWM_DECODER_LOAD_Common << PWM_DECODER_LOAD_Pos);                  // Specifies the Common Decoder
  NRF_PWM0->MODE = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);                           // Specifies the mode to be up-only
  
  NRF_PWM0->PRESCALER = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);   // Specifies the prescaler to divide the clock by 0
  NRF_PWM0->COUNTERTOP = 1024;
  
  NRF_PWM0->SEQ[0].REFRESH  = (0 << PWM_SEQ_REFRESH_CNT_Pos);                             // Disables Refreshing
  NRF_PWM0->SEQ[0].ENDDELAY = (0 << PWM_SEQ_ENDDELAY_CNT_Pos);                            // Disables End Delay
  NRF_PWM0->LOOP = (PWM_LOOP_CNT_Disabled << PWM_LOOP_CNT_Pos);                           // Disables Looping
}

#endif

static const int8_t b64_table[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1, 0,-1,-1,
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1
};

static uint8_t* decode_base64(const char* src, size_t len, size_t* out_len) {
    size_t olen = len / 4 * 3;
    uint8_t* out = (uint8_t*)malloc(olen);
    if (!out) return nullptr;

    uint8_t* pos = out;
    int pad = 0;

    for (size_t i = 0; i < len; i += 4) {
        int8_t a = b64_table[(uint8_t)src[i]];
        int8_t b = b64_table[(uint8_t)src[i+1]];
        int8_t c = b64_table[(uint8_t)src[i+2]];
        int8_t d = b64_table[(uint8_t)src[i+3]];

        if (c == -1) pad++;
        if (d == -1) pad++;

        *pos++ = (a << 2) | (b >> 4);
        if (c != -1) *pos++ = (b << 4) | (c >> 2);
        if (d != -1) *pos++ = (c << 6) | d;
    }

    *out_len = pos - out;
    return out;
}

namespace tts {
  /**
   * Plays a word through the Text-To-Speech System.
   * @param speak_text The name of the word being spoken in the data_map.
   * @param display_text How the word should be displayed on the screen
   */
  void announceWord(String display_text);

  //%
  void announceWord(String display_text, String base64, int sample_rate) {
  #if MICROBIT_CODAL == 1

    const char* b64 = base64->getUTF8Data();
    size_t b64_len = base64->getUTF8Size();

    if (b64_len == 0) {
      uBit.display.scroll(ManagedString(display_text->getUTF8Data()), 100);
      return;
    }

    // Decode base64 → compressed audio
    size_t compressed_len = 0;
    uint8_t* compressed = decode_base64(b64, b64_len, &compressed_len);

    if (!compressed || compressed_len == 0) {
      uBit.display.scroll("ERR", 100);
      return;
    }

    // Decompress → PCM
    size_t pcm_len = 0;
    uint8_t* pcm = decompress(compressed, compressed_len, &pcm_len);
    free(compressed);

    if (!pcm || pcm_len == 0) {
      uBit.display.scroll("ERR", 100);
      return;
    }

    // Scroll + play synchronously
    uBit.display.scroll(ManagedString(display_text->getUTF8Data()), 50);

    init_timer(sample_rate);
    play_wav(pcm, pcm_len);

    free(pcm);

  #else
    uBit.display.scroll(ManagedString(display_text->getUTF8Data()), 50);
  #endif
  }

} // namespace tts
