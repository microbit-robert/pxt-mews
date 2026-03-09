/**
 * Provides access to the Mews Text To Speech System
 */
//% color=#FF8243 weight=100 icon="\uf238"
namespace textToSpeech {
  let words: string[];

  //% block="Say: $text"
  export function say(text: string): void {
    if (audio.sample_rate == -1) {
      announceWord("AUDIO NOT INITIALISED", "", 0);
      return;
    }

    words = text.split(" ");
    words.forEach((word) => {
      try {
        announceWord(
          word,
          audio.audio_data[word]["data"], // base64 string
          audio.sample_rate,
        );
      } catch (TypeError) {
        announceWord("KEYERR", "", 0);
      }
    });
  }

  //% shim=tts::announceWord
  function announceWord(
    display_text: string,
    base64: string,
    sample_rate: number,
  ): void {
    console.log(display_text);
  }
}

// BASEPLATE OVERWRITTEN BY EXTWRITECODE. DO NOT TOUCH. //
namespace audio {
  export let sample_rate: number;
  export let audio_data: { [key: string]: { [key: string]: any } };
}
