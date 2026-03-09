#include "pxt.h"
#include "MicroBit.h"
#include "utils/decompressor.h"
#include <string>
using namespace pxt;

// --- function declarations unchanged ---
void init_timer(int sample_rate);
void init_pwm();
void play_wav(uint8_t* audio_data, size_t out_size);
void play_sample(uint16_t sample);

#if MICROBIT_CODAL

// Single static PCM buffer — lives in BSS, never touches the heap.
// sized for the largest word you'll ever have: data_len * 2.
// Your sample data shows sizes up to 11264 compressed bytes → 22528 PCM bytes.
#define PCM_BUFFER_CAPACITY 32768
static uint8_t pcm_buffer[PCM_BUFFER_CAPACITY];

void play_wav(uint8_t* audio_data, size_t out_size){
  init_pwm();
  for(unsigned int i = 0; i < out_size; i++){
    NRF_TIMER0->TASKS_CLEAR=(TIMER_TASKS_CLEAR_TASKS_CLEAR_Trigger<<TIMER_TASKS_CLEAR_TASKS_CLEAR_Pos);
    NRF_TIMER0->EVENTS_COMPARE[0]=(TIMER_EVENTS_COMPARE_EVENTS_COMPARE_NotGenerated<<TIMER_EVENTS_COMPARE_EVENTS_COMPARE_Pos);
    play_sample((uint16_t)audio_data[i]);
    while(NRF_TIMER0->EVENTS_COMPARE[0]==(TIMER_EVENTS_COMPARE_EVENTS_COMPARE_NotGenerated<<TIMER_EVENTS_COMPARE_EVENTS_COMPARE_Pos));
  }
  NRF_TIMER0->TASKS_STOP=(TIMER_TASKS_STOP_TASKS_STOP_Trigger<<TIMER_TASKS_STOP_TASKS_STOP_Pos);
  NRF_PWM0->ENABLE=(PWM_ENABLE_ENABLE_Disabled<<PWM_ENABLE_ENABLE_Pos);
}

void play_sample(uint16_t sample){
  static volatile uint16_t sequence[1];
  sequence[0] = (sample * NRF_PWM0->COUNTERTOP) / 255;
  NRF_PWM0->SEQ[0].PTR = ((uint32_t)sequence << PWM_SEQ_PTR_PTR_Pos);
  NRF_PWM0->SEQ[0].CNT = (1 << PWM_SEQ_CNT_CNT_Pos);
  NRF_PWM0->TASKS_SEQSTART[0] = (PWM_TASKS_SEQSTART_TASKS_SEQSTART_Trigger<<PWM_TASKS_SEQSTART_TASKS_SEQSTART_Pos);
}

void init_timer(int sample_rate){
  NRF_TIMER0->PRESCALER=(4UL<<TIMER_PRESCALER_PRESCALER_Pos);
  NRF_TIMER0->BITMODE=(TIMER_BITMODE_BITMODE_32Bit<<TIMER_BITMODE_BITMODE_Pos);
  NRF_TIMER0->CC[0]=1000000/sample_rate;
  NRF_TIMER0->TASKS_START=(TIMER_TASKS_START_TASKS_START_Trigger<<TIMER_TASKS_START_TASKS_START_Pos);
}

void init_pwm(){
  NRF_PWM0->PSEL.OUT[0]=(MICROBIT_PIN_SPEAKER<<PWM_PSEL_OUT_PIN_Pos)|
  (PWM_PSEL_OUT_CONNECT_Connected<<PWM_PSEL_OUT_CONNECT_Pos);
  NRF_PWM0->ENABLE=(PWM_ENABLE_ENABLE_Enabled<<PWM_ENABLE_ENABLE_Pos);
  NRF_PWM0->DECODER=(PWM_DECODER_LOAD_Common<<PWM_DECODER_LOAD_Pos);
  NRF_PWM0->MODE=(PWM_MODE_UPDOWN_Up<<PWM_MODE_UPDOWN_Pos);
  NRF_PWM0->PRESCALER=(PWM_PRESCALER_PRESCALER_DIV_1<<PWM_PRESCALER_PRESCALER_Pos);
  NRF_PWM0->COUNTERTOP=1024;
  NRF_PWM0->SEQ[0].REFRESH=(0<<PWM_SEQ_REFRESH_CNT_Pos);
  NRF_PWM0->SEQ[0].ENDDELAY=(0<<PWM_SEQ_ENDDELAY_CNT_Pos);
  NRF_PWM0->LOOP=(PWM_LOOP_CNT_Disabled<<PWM_LOOP_CNT_Pos);
}

#endif

namespace tts {
  //%
  void announceWord(String display_text, Buffer audio_data, int sample_rate, int size){
    #if MICROBIT_CODAL == 1

    if(size == -1){
      uBit.display.scroll(ManagedString(display_text->getUTF8Data()), 100);
      return;
    }

    // DEBUG: show what size and buffer length we actually received
    uBit.display.scroll(ManagedString("SZ:"), 80);
    uBit.display.scroll(ManagedString(size), 80);
    uBit.display.scroll(ManagedString("BL:"), 80);
    uBit.display.scroll(ManagedString((int)audio_data->length), 80);

    size_t out_len = 0;
    bool ok = decompress(audio_data->data, (size_t)size, pcm_buffer, PCM_BUFFER_CAPACITY, &out_len);

    // DEBUG: show decompressor result
    uBit.display.scroll(ManagedString("OK:"), 80);
    uBit.display.scroll(ManagedString((int)ok), 80);
    uBit.display.scroll(ManagedString("OL:"), 80);
    uBit.display.scroll(ManagedString((int)out_len), 80);

    if(!ok || out_len == 0){
      uBit.display.scroll("ERR", 100);
      return;
    }

    init_timer(sample_rate);
    volatile int display_status = -1066;
    do{
      display_status = uBit.display.scrollAsync(ManagedString(display_text->getUTF8Data()), 50);
    }while(display_status != 0);

    play_wav(pcm_buffer, out_len);
    // No free() — static buffer needs no cleanup

    #else
    uBit.display.scroll(ManagedString(display_text->getUTF8Data()), 50);
    #endif
  }
}