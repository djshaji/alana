# Proposal
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
