# Basic MIDI to CV

A versatile MIDI to CV converter firmware for the Shmøergh Moduleur Brain module. Converts incoming MIDI note messages to 1V/octave CV pitch and gate signals, with configurable MIDI channel and CV output routing.

## Features

- **MIDI to CV/Gate conversion**: Converts MIDI note-on/note-off messages to 1V/octave pitch CV and gate signals
- **Configurable MIDI channel**: Listen to any MIDI channel (1-16) with visual feedback via LEDs
- **Dual CV outputs**: Route pitch and gate to either output channel of the Brain module
- **Interactive configuration**: Use buttons and potentiometers to change settings on the fly
- **Visual feedback**: 6-LED display shows current MIDI channel or selected CV output during configuration

### Basic Operation

Once flashed, the module will:
- Listen for MIDI notes on the configured MIDI channel (default: channel 1)
- Output pitch CV (1V/octave) and gate signals on the configured CV channel (default: Channel A)
- Convert MIDI note-on messages to gate high and MIDI note-off to gate low

### Configuring MIDI Channel

1. **Press and hold Button A** (left button)
2. **Turn Pot X** (left potentiometer) to select MIDI channel 1-16
   - LEDs will display the selected channel number in binary (0-15, representing channels 1-16)
3. **Release Button A** to save the setting

### Configuring CV Output Channel

1. **Press and hold Button B** (right button)
2. **Turn Pot Y** (middle potentiometer) to select the output channel:
   - Turn fully counter-clockwise: **Channel A** (LEDs 1-3 illuminate)
   - Turn fully clockwise: **Channel B** (LEDs 4-6 illuminate)
3. **Release Button B** to save the setting

### LED Indicators

- **During MIDI channel selection**: LEDs show channel number in binary (0-15)
- **During CV channel selection**: Left 3 LEDs for Channel A, right 3 LEDs for Channel B
- **During normal operation**: LEDs turn off to conserve power

## Build

```bash
mkdir build
cd build
cmake ..
make
```

## Flash

Flash the generated `brain-basic-midi2cv.uf2` file to your Brain module by holding the BOOTSEL button while connecting it to your computer, then copy the .uf2 file to the mounted drive.

## Development

This project includes brain-sdk as a git submodule. To update the SDK:

```bash
cd brain-sdk
git pull origin main
cd ..
git add brain-sdk
git commit -m "Update brain-sdk"
```
