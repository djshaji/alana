# add optimizations
#~ GTKMM=`pkg-config --cflags --libs gtkmm-4.0` 
GTK=`pkg-config --cflags --libs gtk4`
LV2=`pkg-config --cflags lilv-0 --libs`
JACK=`pkg-config jack --libs --cflags`
SNDFILE=`pkg-config --libs sndfile --cflags`
OPUS=`pkg-config libopusenc opus --libs --cflags`
LAME=`pkg-config lame --libs --cflags`
X11=`pkg-config x11 --libs --cflags`
OPTIMIZE=#-Ofast -mtune=cortex-a72 -mcpu=cortex-a72 

all: amprack

amprack: version.o FileWriter.o main.o rack.o presets.o SharedLibrary.o engine.o jack.o process.o util.o snd.o knob.o
	c++ *.o -o amprack $(GTK) $(LV2) $(JACK) $(OPTIMIZE) $(SNDFILE) $(OPUS) -l:libmp3lame.a  $(LAME) 
	
main.o: main.cc main.h rack.o presets.o
	g++ main.cc -c $(GTK)  $(LV2) $(OPTIMIZE) -Wno-deprecated-declarations

rack.o: rack.cc rack.h settings.o knob.o pluginui.o
	g++ rack.cc -c   $(GTK)  $(LV2) $(OPTIMIZE) 

settings.o: settings.cc settings.h 
	g++ settings.cc -c   $(GTK)  $(LV2) $(OPTIMIZE) 

pluginui.o: pluginui.cpp pluginui.h
	g++ pluginui.cpp -c  $(GTK) $(LV2) -Wno-deprecated-declarations
	
presets.o: presets.cc presets.h
	g++ presets.cc -c   $(GTK) $(OPTIMIZE) $(LV2) -Wno-deprecated-declarations

SharedLibrary.o: SharedLibrary.cpp SharedLibrary.h Plugin.cpp Plugin.h PluginControl.cpp PluginControl.h
	g++ SharedLibrary.cpp Plugin.cpp PluginControl.cpp lv2_ext.cpp -c $(LV2) $(OPTIMIZE)

engine.o: engine.cc engine.h snd.cc snd.h
	g++ engine.cc -c $(JACK) $(LV2) $(OPTIMIZE) $(SNDFILE) $(GTK)

clean:
	rm -v *.o amprack

missing: SharedLibrary.o missing.cc
	c++ missing.cc SharedLibrary.o -o missing -std=c++17 $(LV2) -I/usr/include/lv2 

test: lv2_test.c
	cc lv2_test.c $(LV2) -I/usr/include/lv2 -o lv2_test

jack.o: jack.cc jack.h 
	cc jack.cc -c $(JACK)

process.o: process.cc process.h
	cc process.cc -c

util.o: util.cc util.h
	c++  $(GTK) -c util.cc  -Wno-deprecated-declarations
	
snd.o: snd.cc snd.h
	c++ snd.cc -c $(SNDFILE)

knob.o: knob.cpp  cairo.cpp objects.cpp  dictionary.cpp  mem.cpp  pango.cpp
#~ 	cc -c `pkg-config gtk4 --libs --cflags` GxRegler.cpp GxControlParameter.cpp -w -lm drawingutils.cpp GxKnob.cpp GxBigKnob.cpp
	c++ `pkg-config gtk4 --libs --cflags`  -w -lm knob.cpp  cairo.cpp objects.cpp  dictionary.cpp  mem.cpp  pango.cpp -c

version.o:
	echo \#define VERSION `git rev-list --count HEAD` > version.h
	
FileWriter.o: FileWriter.cpp FileWriter.h LockFreeQueue.cpp LockFreeQueue.h vringbuffer.o
	c++   upwaker.c vringbuffer.cc FileWriter.cpp LockFreeQueue.cpp $(OPUS) $(SNDFILE) -c -w $(JACK)

vringbuffer.o: upwaker.c vringbuffer.cc
	c++ -fpermissive -c upwaker.c vringbuffer.cc 
