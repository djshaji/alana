# add optimizations
GTKMM=`pkg-config --cflags --libs gtkmm-4.0` 
GTK=`pkg-config --cflags --libs gtk4`
LV2=`pkg-config --cflags lilv-0 --libs`
JACK=`pkg-config jack --libs --cflags`
SNDFILE=`pkg-config --libs sndfile --cflags`
OPUS=`pkg-config libopusenc opus --libs --cflags`
LAME=`pkg-config lame --libs --cflags`
X11=`pkg-config x11 --libs --cflags`
#OPTIMIZE=-Ofast 

all: version.o filewriter.o main.o rack.o presets.o SharedLibrary.o engine.o jack.o process.o util.o snd.o knobs.o
	c++ *.o -o amprack $(GTK) $(LV2) $(JACK) $(OPTIMIZE) $(SNDFILE) $(OPUS) -l:libmp3lame.a $(GTKMM) $(LAME) 
	
main.o: main.cc main.h rack.o presets.o
	g++ main.cc -c $(GTKMM)  $(GTK)  $(LV2) $(OPTIMIZE) 

rack.o: rack.cc rack.h pluginui.cpp pluginui.h knobs.o
	g++ rack.cc pluginui.cpp settings.cc -c $(GTKMM)  $(GTK)  $(LV2) $(OPTIMIZE)

presets.o: presets.cc presets.h
	g++ presets.cc -c $(GTKMM)  $(GTK) $(OPTIMIZE) $(LV2)

SharedLibrary.o: SharedLibrary.cpp SharedLibrary.h Plugin.cpp Plugin.h PluginControl.cpp PluginControl.h
	g++ SharedLibrary.cpp Plugin.cpp PluginControl.cpp lv2_ext.cpp -c $(LV2) $(OPTIMIZE)

engine.o: engine.cc engine.h snd.cc snd.h
	g++ engine.cc -c $(JACK) $(LV2) $(OPTIMIZE) $(GTKMM) $(SNDFILE)

clean:
	rm -v *.o amprack

missing: SharedLibrary.o missing.cc
	c++ missing.cc SharedLibrary.o -o missing -std=c++17 $(LV2) -I/usr/include/lv2 

test: lv2_test.c
	cc lv2_test.c $(LV2) -I/usr/include/lv2 -o lv2_test

jack: jack.cc jack.h 
	cc jack.cc -c $(JACK)

process: process.cc process.h
	cc process.cc -c

util.o: util.c util.h
	c++ util.c -c $(GTKMM) -fpermissive
snd.o: snd.cc snd.h
	c++ snd.cc -c $(SNDFILE)

knobs.o: knob.cpp  cairo.cpp objects.cpp  dictionary.cpp  mem.cpp  pango.cpp
#~ 	cc -c `pkg-config gtk4 --libs --cflags` GxRegler.cpp GxControlParameter.cpp -w -lm drawingutils.cpp GxKnob.cpp GxBigKnob.cpp
	c++ `pkg-config gtk4 --libs --cflags`  -w -lm -fpermissive knob.cpp  cairo.cpp objects.cpp  dictionary.cpp  mem.cpp  pango.cpp -c

version.o:
	echo \#define VERSION `git rev-list --count HEAD` > version.h
	
filewriter.o: FileWriter.cpp FileWriter.h LockFreeQueue.cpp LockFreeQueue.h
	c++  -fpermissive upwaker.c vringbuffer.c FileWriter.cpp LockFreeQueue.cpp $(OPUS) $(SNDFILE) -c -w $(JACK)
