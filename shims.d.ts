// Auto-generated. Do not edit.


    /**
     * Plays an individual audio sample through the Jiangsu Huaneng MLT-8530 Speaker.
     * @param sample The sample to play through the speaker. 
     */

declare namespace tts {

    /**
     * Plays a word through the Text-To-Speech System.
     * @param speak_text The name of the word being spoken in the data_map.
     * @param display_text How the word should be displayed on the screen
     */
    //% shim=tts::announceWord
    function announceWord(display_text: string, base64: string, sample_rate: int32): void;
}

// Auto-generated. Do not edit. Really.
