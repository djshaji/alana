# add optimizations
#~ GTKMM=`pkg-config --cflags --libs gtkmm-4.0` 

TARGET=linux
TARGET=win32

ifeq ($(TARGET),linux)
GTK=`pkg-config --cflags --libs gtk4`
LV2=`pkg-config --cflags lilv-0 --libs`
JACK=`pkg-config jack --libs --cflags`
SNDFILE=`pkg-config --libs sndfile --cflags`
OPUS=`pkg-config libopusenc opus --libs --cflags`
LAME=`pkg-config lame --libs --cflags`
X11=`pkg-config x11 --libs --cflags`
OPTIMIZE=#-Ofast -mtune=cortex-a72 -mcpu=cortex-a72 
CC=cc
CPP=c++
else ifeq ($(TARGET),win32)
GTK=`x86_64-w64-mingw32-pkg-config --cflags --libs gtk4`
LV2=`pkg-config --cflags lilv-0 --libs`
JACK=`pkg-config jack --libs --cflags`
SNDFILE=`pkg-config --libs sndfile --cflags`
OPUS=`pkg-config libopusenc opus --libs --cflags`
LAME=`pkg-config lame --libs --cflags`
X11=`pkg-config x11 --libs --cflags`
OPTIMIZE=#-Ofast -mtune=cortex-a72 -mcpu=cortex-a72 
CC=x86_64-w64-mingw32-gcc
CPP=x86_64-w64-mingw32-g++
endif
all: amprack

amprack: version.o FileWriter.o main.o rack.o presets.o SharedLibrary.o engine.o jack.o process.o util.o snd.o knob.o
	$(CPP) *.o -o amprack $(GTK) $(LV2) $(JACK) $(OPTIMIZE) $(SNDFILE) $(OPUS) -l:libmp3lame.a  $(LAME) 
	
main.o: main.cc main.h rack.o presets.o
	$(CPP) main.cc -c $(GTK)  $(LV2) $(OPTIMIZE) -Wno-deprecated-declarations

rack.o: rack.cc rack.h settings.o knob.o pluginui.o
	$(CPP) rack.cc -c   $(GTK)  $(LV2) $(OPTIMIZE) 

settings.o: settings.cc settings.h 
	$(CPP) settings.cc -c   $(GTK)  $(LV2) $(OPTIMIZE) 

pluginui.o: pluginui.cpp pluginui.h
	$(CPP) pluginui.cpp -c  $(GTK) $(LV2) -Wno-deprecated-declarations
	
presets.o: presets.cc presets.h
	$(CPP) presets.cc -c   $(GTK) $(OPTIMIZE) $(LV2) -Wno-deprecated-declarations

SharedLibrary.o: SharedLibrary.cpp SharedLibrary.h Plugin.cpp Plugin.h PluginControl.cpp PluginControl.h
	$(CPP) SharedLibrary.cpp Plugin.cpp PluginControl.cpp lv2_ext.cpp -c $(LV2) $(OPTIMIZE)

engine.o: engine.cc engine.h snd.cc snd.h
	$(CPP) engine.cc -c $(JACK) $(LV2) $(OPTIMIZE) $(SNDFILE) $(GTK)

clean:
	rm -v *.o amprack

missing: SharedLibrary.o missing.cc
	$(CPP) missing.cc SharedLibrary.o -o missing -std=$(CPP)17 $(LV2) -I/usr/include/lv2 

test: lv2_test.c
	$(CC) lv2_test.c $(LV2) -I/usr/include/lv2 -o lv2_test

jack.o: jack.cc jack.h 
	$(CC) jack.cc -c $(JACK)

process.o: process.cc process.h
	$(CC) process.cc -c

util.o: util.cc util.h
	$(CPP)  $(GTK) -c util.cc  -Wno-deprecated-declarations
	
snd.o: snd.cc snd.h
	$(CPP) snd.cc -c $(SNDFILE)

knob.o: knob.cpp  cairo.cpp objects.cpp  dictionary.cpp  mem.cpp  pango.cpp
#~ 	cc -c `pkg-config gtk4 --libs --cflags` GxRegler.cpp GxControlParameter.cpp -w -lm drawingutils.cpp GxKnob.cpp GxBigKnob.cpp
	$(CPP) `pkg-config gtk4 --libs --cflags`  -w -lm knob.cpp  cairo.cpp objects.cpp  dictionary.cpp  mem.cpp  pango.cpp -c

version.o:
	echo \#define VERSION `git rev-list --count HEAD` > version.h
	
FileWriter.o: FileWriter.cpp FileWriter.h LockFreeQueue.cpp LockFreeQueue.h vringbuffer.o
	$(CPP)   upwaker.c vringbuffer.cc FileWriter.cpp LockFreeQueue.cpp $(OPUS) $(SNDFILE) -c -w $(JACK)

vringbuffer.o: upwaker.c vringbuffer.cc
	$(CPP) -fpermissive -c upwaker.c vringbuffer.cc 
