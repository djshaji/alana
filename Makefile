GTKMM=`pkg-config --cflags --libs gtkmm-4.0` 
GTK=`pkg-config --cflags --libs gtk4`
LV2=`pkg-config --cflags lilv-0 --libs`
JACK=`pkg-config jack --libs --cflags`

all: main.o rack.o presets.o SharedLibrary.o engine.o jack.o
	c++ $(GTKMM) *.o -o amprack $(GTK) $(LV2) $(JACK)
	
main.o: main.cc main.h
	g++ main.cc -c $(GTKMM)  $(GTK)

rack.o: rack.cc rack.h pluginui.cpp pluginui.h 
	g++ rack.cc pluginui.cpp -c $(GTKMM)  $(GTK)

presets.o:
	g++ presets.cc -c $(GTKMM)  $(GTK)

SharedLibrary.o: SharedLibrary.cpp SharedLibrary.h Plugin.cpp Plugin.h
	g++ SharedLibrary.cpp Plugin.cpp PluginControl.cpp lv2_ext.cpp -c $(LV2)

engine.o: engine.cc engine.h
	g++ engine.cc -c $(JACK)

clean:
	rm -v *.o

missing: SharedLibrary.o missing.cc
	c++ missing.cc SharedLibrary.o -o missing -std=c++17 $(LV2) -I/usr/include/lv2

test: lv2_test.c
	cc lv2_test.c $(LV2) -I/usr/include/lv2 -o lv2_test
jack: jack.cc jack.h
	cc jack.cc -c $(JACK)
