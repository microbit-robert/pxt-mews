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
      const buf = audio.getWord(word);

      if (buf == null) {
        announceWord("?", Buffer.create(0), 0, -1);
        continue;
      }

      // buf points into flash — no heap allocation at all
      announceWord(word, buf, audio.sample_rate, buf.length);
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

namespace audio {
  export let getWord: (word: string) => Buffer;
}
