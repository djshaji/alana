# add optimizations
#~ GTKMM=`pkg-config --cflags --libs gtkmm-4.0` 

TARGET=linux
#TARGET=win32

VERSION=`git rev-list --count HEAD`

ifeq ($(TARGET),linux)
GTK=`pkg-config --cflags --libs gtk4`  -lssl -lcrypto
LV2=`pkg-config --cflags lilv-0 --libs zix-0`
JACK=`pkg-config jack --libs --cflags`
SNDFILE=`pkg-config --libs sndfile --cflags`
OPUS=`pkg-config libopusenc opus --libs --cflags`
LAME=`pkg-config lame --libs --cflags` -l:libmp3lame.a
GLIB=`pkg-config glib-2.0 --libs --cflags`
X11=`pkg-config x11 --libs --cflags`
OPTIMIZE=#-Ofast -mtune=cortex-a72 -mcpu=cortex-a72 
CC=cc -g -ggdb 
CPP=c++ -g -ggdb -std=c++23
else ifeq ($(TARGET),win32)
GTK=`x86_64-w64-mingw32-pkg-config --cflags --libs gtk4 gtk4-win32` -I/usr/x86_64-w64-mingw32/sys-root/mingw/include/gtk-4.0/
LV2=
JACK=-ljack64 -L.
SNDFILE=
OPUS=`x86_64-w64-mingw32-pkg-config --cflags --libs opus opusfile`
LAME=-llibmp3lame
X11=
GLIB=`mingw64-pkg-config glib-2.0 --libs --cflags`
OPTIMIZE=-Ofast
CC=x86_64-w64-mingw32-gcc -g -mwindows -mconsole
CPP=x86_64-w64-mingw32-g++ -std=c++17 -g -mwindows -mconsole 
DLFCN=-llibdl -lws2_32 -lwsock32 -lssl -lcrypto -lcrypt32 -liphlpapi
endif
all: amprack

amprack: version.o FileWriter.o main.o rack.o presets.o SharedLibrary.o engine.o jack.o process.o util.o snd.o knob.o
	$(CPP) *.o -o amprack $(GTK) $(LV2) $(JACK) $(OPTIMIZE) $(SNDFILE) $(OPUS) $(LAME)  $(DLFCN)
	
main.o: main.cc main.h rack.o presets.o log.o sync.o
	$(CPP) main.cc -c $(GTK)  $(LV2) $(OPTIMIZE) -Wno-deprecated-declarations

log.o: log.c log.h
	$(CC) log.c -c  $(GTK) 
	
rack.o: rack.cc rack.h settings.o knob.o pluginui.o 
	$(CPP) rack.cc -c   $(GTK)  $(LV2) $(OPTIMIZE) 

settings.o: settings.cc settings.h 
	$(CPP) settings.cc -c   $(GTK)  $(LV2) $(OPTIMIZE) 

pluginui.o: pluginui.cpp pluginui.h
	$(CPP) pluginui.cpp -c  $(GTK) $(LV2) -Wno-deprecated-declarations
	
presets.o: presets.cc presets.h
	$(CPP) presets.cc -c   $(GTK) $(OPTIMIZE) $(LV2) -Wno-deprecated-declarations

SharedLibrary.o: SharedLibrary.cpp SharedLibrary.h Plugin.cpp Plugin.h PluginControl.cpp PluginControl.h
	$(CPP) SharedLibrary.cpp Plugin.cpp PluginControl.cpp lv2_ext.cpp symap.c atom.cpp -c $(LV2) $(OPTIMIZE) $(GTK) 	

engine.o: engine.cc engine.h snd.cc snd.h
	$(CPP) engine.cc -c $(JACK) $(LV2) $(OPTIMIZE) $(SNDFILE) $(GTK)

clean:
	rm -v *.o

missing: SharedLibrary.o missing.cc
	$(CPP) missing.cc SharedLibrary.o log.o -o missing -std=c++17 $(LV2) -I/usr/include/lv2  $(GTK) $(DLFCN) 

test: lv2_test.c
	$(CC) lv2_test.c $(LV2) -I/usr/include/lv2 -o lv2_test

# DEV
#~ ifeq ($(TARGET),linux1)
jack.o: jack.cc jack.h 
	$(CC) jack.cc -c $(JACK) $(GTK) 
#~ else
#~ jack.o: pa.cc pa.h
#~ 	$(CPP) pa.cc -c  $(GTK) $(JACK) -o jack.o $(GLIB)
#~ endif	

process.o: process.cc process.h
	$(CC) process.cc -c $(GTK) 

util.o: util.cc util.h
	$(CPP)  $(GTK) -c util.cc  -Wno-deprecated-declarations
	
snd.o: snd.cc snd.h
	$(CPP) snd.cc -c $(SNDFILE) $(GTK) 

knob.o: knob.cpp  cairo.cpp objects.cpp  dictionary.cpp  mem.cpp  pango.cpp
#~ 	cc -c `pkg-config gtk4 --libs --cflags` GxRegler.cpp GxControlParameter.cpp -w -lm drawingutils.cpp GxKnob.cpp GxBigKnob.cpp
	$(CPP) $(GTK)  -w -lm knob.cpp  cairo.cpp objects.cpp  dictionary.cpp  mem.cpp  pango.cpp -c

version.o:
	echo \#define VERSION `git rev-list --count HEAD` > version.h
	
FileWriter.o: FileWriter.cpp FileWriter.h LockFreeQueue.cpp LockFreeQueue.h vringbuffer.o
	$(CPP)   $(GTK)  upwaker.c vringbuffer.c FileWriter.cpp LockFreeQueue.cpp $(OPUS) $(SNDFILE) -c -w $(JACK)

vringbuffer.o: upwaker.c vringbuffer.c
	$(CPP) -fpermissive -c upwaker.c vringbuffer.c $(GTK) 	

win32-release:
	export VER=$(VERSION) ; cd .. ; zip -r releases/amprack-$$VER.zip win/

sync.o: sync.cc sync.h server.o
	$(CPP) -c sync.cc $(GTK)

server.o: server.cc server.h client.cc client.h winserver.cc winserver.h
	$(CPP) -c winserver.cc server.cc client.cc $(GTK) -Wall 

echo-client: echo-client.cc server.o
	$(CPP) -o echo-client echo-client.cc client.cc $(GTK) -Wall

win-net: win_net.cc
	$(CPP) -o win-net win_net.cc -lws2_32 -lwsock32

win32-installer:
	makensis nsis.nsh
	mv -v ../releases/amprack-setup.exe ../releases/amprack-$(VERSION).exe 

rpm:
	rpmbuild -bb ~/rpmbuild/SPECS/amprack.spec
	mv -v ~/rpmbuild/RPMS/x86_64/amprack-5-${VERSION}.* ~/projects/alana/releases/

win32-ip: win_ip.cc
	$(CPP) -o win-ip win_ip.cc -lws2_32 -lwsock32 -liphlpapi
