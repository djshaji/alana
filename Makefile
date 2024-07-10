GTKMM=`pkg-config --cflags --libs gtkmm-4.0` 
GTK=`pkg-config --cflags --libs gtk4`

all: main.o rack.o presets.o SharedLibrary.o engine.o
	c++ $(GTKMM) *.o -o amprack $(GTK)
	
main.o: main.cc main.h
	g++ main.cc -c $(GTKMM)  $(GTK)

rack.o: rack.cc rack.h pluginui.cpp pluginui.h
	g++ rack.cc pluginui.cpp -c $(GTKMM)  $(GTK)

presets.o:
	g++ presets.cc -c $(GTKMM)  $(GTK)

SharedLibrary.o:
	g++ SharedLibrary.cpp Plugin.cpp PluginControl.cpp lv2_ext.cpp -c

engine.o: engine.cc engine.h
	g++ engine.cc -c

clean:
	rm -v *.o
