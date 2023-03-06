ticker-prototype, concering metronome tick file:
- Build a new function instead of Audio load that can properly handle .wav files with metadata and error check them
- Only load the first 1000 samples of the .wav file- the tick needs to be short
