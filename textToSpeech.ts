/**
 * Provides access to the Mews Text To Speech System
 */
//% color=#FF8243 weight=100 icon="\uf238"
namespace textToSpeech {
  //% block="Say: $text"
  export function say(text: string): void {
    if (audio.sample_rate == -1) {
      announceWord("AUDIO NOT INITIALISED", Buffer.create(0), 0, -1);
      return;
    }

    const words = text.split(" ");

    for (let i = 0; i < words.length; i++) {
      const word = words[i];
      const entry = audio.audio_data[word];

      if (entry == null) {
        // Unknown word — show on display and continue, no buffer allocated
        announceWord("?", Buffer.create(0), 0, -1);
        continue;
      }

      // Decode one word at a time inside its own block scope.
      // The buffer becomes eligible for GC as soon as announceWord returns
      // before we move on to decoding the next word.
      const buf = Buffer.fromBase64(entry["data"]);
      announceWord(word, buf, audio.sample_rate, entry["size"]);
      // buf goes out of scope here — GC can reclaim it before next iteration
    }
  }

  //% shim=tts::announceWord
  function announceWord(
    display_text: string,
    buffer: Buffer,
    sample_rate: number,
    size: number,
  ): void {
    console.log(display_text);
  }
}
