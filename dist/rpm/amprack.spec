Name:       amprack
Version:    4
Release:    90
Summary:    Amp Rack Guitar Effects Processor Pedal
License:    Apache License 2.0

%description
Amp Rack is a Guitar / Voice Audio Effects Processor, Tuner and MP3 Recorder for Android.

Amp Rack is an Open Source LV2 / LADSPA Plugins Host for Android. More than 300 high quality audio plugins are available which can be added in any order to the audio effect chain to create distinct high quality tones for your guitar!
New features:
✓ LV2 Plugin support
✓ Looper: Record rhythm loops or load a drum loop from file
✓ All new Quick Patch: Just one tap to change factory patches for finding sounds quickly
✓ Improved Presets Loading
✓ High Quality Audio Plugins such as different Distortion, Overdrive, Cabinet Emulation, Delay, Echo, Reverb, Compressors, Limiters, Sustain, Vocoders, Tube Emulators, and even Autotune (and many more!
✓ One click operation: add a new effect to the chain, or use presets created by professional guitarists for beautiful instant ready made tones for your tracks!
✓ Add Unlimited effects in any order to the effect chain
✓ Customize effect parameters as per your sound preference
✓ Save presets and share with the world!
✓ Supports Mic / Mic in jack / USB Interfaces. No need for expensive accessories. Just buy any generic 1/4" TRS to 3.5mm adapter for less than $1 and rock on!
✓ High performance Low Latency Native Audio Processing: No latency on even the most basic devices
✓ High performance realtime low latency code
✓ High quality floating point precision audio processing
✓ Completely Open Source App: Download the code from https://amprack.acoustixaudio.org

%prep
# we have no source, so nothing here

%install
mkdir -p %{buildroot}/usr/bin/
mkdir -p %{buildroot}/usr/share/amprack
cp -v ~/projects/alana/gtk4/amprack %{buildroot}/usr/bin/
cp -rv ~/projects/alana/gtk4/assets %{buildroot}/usr/share/amprack
cp -rv ~/projects/alana/gtk4/libs %{buildroot}/usr/share/amprack

%files
/usr/bin/amprack
/usr/share/amprack

%changelog
- first public beta
