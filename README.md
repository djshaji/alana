# Amp Rack Guitar Effects Processor
Amp Rack is a Guitar / Voice Audio Effects Processor for PC and Android. Amp Rack is an Open Source LV2 / LADSPA Plugins Host for Android. More than 150 high quality audio plugins are available which can be added in any order to the audio effect chain to create distinct high quality tones for your guitar!

![Capture](https://github.com/user-attachments/assets/f9377a63-20f9-4100-bf14-e405ac04d850)

## Install
Download the latest release for Linux and Windows from https://github.com/djshaji/alana/releases/
Amp Rack is also available for Android: https://github.com/djshaji/amp-rack
### Windows
Install using the setup installer.

### Linux
- Install using .deb or .rpm package
- Download the zip file and extract somewhere, then run `./amprack`

## Building from source
Both linux and win32 binaries are built on linux. mingw64 is used for win32 target.
In Makefile, set TARGET to either linux or win32.
Simply run `make`

## Dependencies
```
gtk-4.0
jack-devel
libsamplerate
libsndfile
openssl-dev
opusenc-dev
dlfcn (for mingw64)

fftw3 (needed by some plugins)
fftw3f (needed by some plugins)
zita-resampler (needed by some plugins)
zita-convolver (needed by some plugins)
```

![Screenshot_2024-08-05_15-44-15](https://github.com/user-attachments/assets/39b7e54a-e0c5-46f7-b843-52a44f1f63c3)
![Screenshot from 2024-08-05 15-44-35](https://github.com/user-attachments/assets/4cc98b17-c577-4ec5-a45f-5d2cbdd81b00)


### Introducing Amp Rack Version 5! 
#### Amp Rack is now available for Windows, Linux and Raspberry Pi! Download now:
https://github.com/djshaji/alana/releases/latest

This is the biggest update of Amp Rack till date, and contains tons of new features and under the hood bug fixes.

### An overview:
- Low Latency Audio Engine
- Distortion, Cabinet Emulations, Delay, Reverb, Flangers, Phasers, Compressors, Limiters, Low / High Pass Plugins and more!
- Neural Amp Modeler and AIDA-X Plugins
- IR Cabinet Loader
- Audio Recording to Wav, MP3 and Opus
- HD Video Recording
- Sync Presets to PC / Smart Pedal
- Fully Skinnable UI with themes
***


### Features

✓ High Quality Audio Plugins such as different Distortion, Overdrive, Cabinet Emulation, Delay, Echo, Reverb, Compressors, Limiters, Sustain, Vocoders, Tube Emulators, and even Autotune (and many more!

✓ Easy to use UI with customizable background!

✓ One click operation: add a new effect to the chain, or use presets created by professional guitarists for beautiful instant ready made tones for your tracks!

✓ Add Unlimited effects in any order to the effect chain

✓ Customize effect parameters as per your sound preference

✓ Save presets and share with the world!

✓ Supports Mic / Mic in jack / USB Interfaces. No need for expensive accessories. Just buy any generic 1/4" TRS to 3.5mm adapter for less than $1 and rock on!

✓ High performance Low Latency Native Audio Processing: No latency on even the most basic devices

✓ All new Backing Track player. Simply record some chords and use it on loop as a backing track! You can change BPM and Volume of the backing track. Perfect to practice solos or master scales

✓ All new Drum Machine. Transform yourself into a one man band, or just practice rocking out. Choose from a variety of exciting tracks. Change BPM according to your genre and preference.

✓ High performance realtime low latency code

✓ High quality floating point precision audio processing

✓ Completely Open Source App: Download the code from https://amprack.acoustixaudio.org

# The Alana Project
* Port / Clone Amp Rack for PC (Linux, Windows)
* Custom Amp Rack Pedal Board 

## PC Clone
Since the Audio Engine is written in C++, it is already cross platform. [How can it not be cross platform? All platforms are written in it.](https://www.youtube.com/watch?v=s7wLYzRJt3s)

### Development Plan
* Use the same Audio Engine (i.e amp-rack/app/src/main/cpp)
* Write a new UI using Gtk+ (3 or 4?)
* Complete Amp Rack Theme Support 
* Complete preset support (i.e firebase)
* A new Low Level™ abstraction API (Audio*) that will #ifdef to use pipewire (jack?) on Linux and Dsound(?) on Windows

## Amp Rack Pedal
This is a Crazy Idea™ and I have no clue if it can actually be done and if it would be feasible to do so, but I am `excited`

### Development Plan
* (At first) NCurses (!) interface for Amp Rack
* Write own (get this) statically linked init in C, which will mount a readonly partition, load the amp rack application off of it, and then mount a tiny read write partition to save user presets (and new plugins)
* Connect this via a cable to phone to (get this) load user presets and new plugins from the (existing) Amp Rack app (phone and PC). Plugins and presets will have a :floppy_disk: send to pedal button to, well, send it to the pedal. It will also have a manage pedal screen where the user can add / remove plugins and presets. Also copy over presets from pedal to phone / PC (A1, A2)
* (And, get this) connect this to a PC to use it (and this is nuts!) as a USB audio interface
* it will have a nice tiny LCD Display, knobs and buttons to edit presets, and of course pedals :arrow_right: up / down patches
* (And, really, get this) plug an MIDI USB keyboard into it and turn it into a syth

Unrealistic sample:
![pedal1](https://github.com/djshaji/amp-rack/assets/17184025/754c2340-3a18-4f92-bed6-a4f73fafbfa7)
