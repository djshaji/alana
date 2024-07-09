GTKMM=`pkg-config --cflags --libs gtkmm-4.0`

all: main.o rack.o presets.o SharedLibrary.o engine.o
	c++ $(GTKMM) *.o -o amprack
	
main.o: main.cc main.h
	g++ main.cc -c $(GTKMM) 

rack.o: rack.cc rack.h pluginui.cpp pluginui.h
	g++ rack.cc pluginui.cpp -c $(GTKMM) 

presets.o:
	g++ presets.cc -c $(GTKMM) 

SharedLibrary.o:
	g++ SharedLibrary.cpp Plugin.cpp PluginControl.cpp lv2_ext.cpp -c

engine.o: engine.cc engine.h
	g++ engine.cc -c

clean:
	rm -v *.o
