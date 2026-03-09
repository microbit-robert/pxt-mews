/**
 * Provides access to the Mews Text To Speech System
 */
//% color=#FF8243 weight=100 icon="\uf238"
namespace textToSpeech {
  let words:string[];

  //% block="Say: $text"
  export function say(text:string): void {
    if(audio.sample_rate == -1){
      announceWord("AUDIO NOT INITIALISED", Buffer.create(0), 0, -1);
      return;
    }
    
    words = text.split(" ");
    words.forEach(word => {
      try{
        announceWord(word, Buffer.fromBase64(audio.audio_data[word]["data"]), audio.sample_rate, audio.audio_data[word]["size"]);
      }catch(TypeError){
        announceWord("KEYERR", Buffer.create(0), 0, -1);
      }
    });
  }

  //% shim=tts::announceWord
  function announceWord(display_text: string, buffer:Buffer, sample_rate:number, size:number): void{
    console.log(display_text);
  }

}


// BASEPLATE OVERWRITTEN BY EXTWRITECODE. DO NOT TOUCH. //
namespace audio{
  export let sample_rate: number;
  export let audio_data: {[key:string]: {[key:string]: any }};
}

