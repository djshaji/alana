# add optimizations
GTKMM=`pkg-config --cflags --libs gtkmm-4.0` 
GTK=`pkg-config --cflags --libs gtk4`
LV2=`pkg-config --cflags lilv-0 --libs`
JACK=`pkg-config jack --libs --cflags`
SNDFILE=`pkg-config --libs sndfile --cflags`
#OPTIMIZE=-Ofast 

all: main.o rack.o presets.o SharedLibrary.o engine.o jack.o process.o util.o snd.o
	c++ $(GTKMM) *.o -o amprack $(GTK) $(LV2) $(JACK) $(OPTIMIZE) $(SNDFILE)
	
main.o: main.cc main.h rack.o presets.o
	g++ main.cc -c $(GTKMM)  $(GTK)  $(LV2) $(OPTIMIZE)

rack.o: rack.cc rack.h pluginui.cpp pluginui.h 
	g++ rack.cc pluginui.cpp -c $(GTKMM)  $(GTK)  $(LV2) $(OPTIMIZE)

presets.o: presets.cc presets.h
	g++ presets.cc -c $(GTKMM)  $(GTK) $(OPTIMIZE) $(LV2)

SharedLibrary.o: SharedLibrary.cpp SharedLibrary.h Plugin.cpp Plugin.h PluginControl.cpp PluginControl.h
	g++ SharedLibrary.cpp Plugin.cpp PluginControl.cpp lv2_ext.cpp -c $(LV2) $(OPTIMIZE)

engine.o: engine.cc engine.h snd.cc snd.h
	g++ engine.cc -c $(JACK) $(LV2) $(OPTIMIZE) $(GTKMM) $(SNDFILE)

clean:
	rm -v *.o

missing: SharedLibrary.o missing.cc
	c++ missing.cc SharedLibrary.o -o missing -std=c++17 $(LV2) -I/usr/include/lv2 

test: lv2_test.c
	cc lv2_test.c $(LV2) -I/usr/include/lv2 -o lv2_test

jack: jack.cc jack.h 
	cc jack.cc -c $(JACK)

process: process.cc process.h
	cc process.cc -c

util.o: util.c util.h
	c++ util.c -c $(GTKMM)
snd.o: snd.cc snd.h
	c++ snd.cc -c $(SNDFILE)
