# "I Have No Tomatoes" Makefile (Windows)
# Run 'make DEBUGMODE=1' if you need to debug the game.
# Or run 'make PROFILE=1' if you need to profile the game.

.PHONY: all clean veryclean rebuild compress

CC = i686-w64-mingw32-g++
WINDRES = i686-w64-mingw32-windres.exe
RESFILE = resource.res
RCFILE = icon.rc
COMPRESS = upx --best
TARGET = ./bin/IHaveNoTomatoes.exe
MARCH = pentium


# Directory defines, you can use these defaults or adjust them if
# necessary. Remember to include the trailing /

# MPK directory (where 'tomatoes.mpk' is), default: ./
MPKDIR = ./

# Music directory (where the music files are), default: ./music/
MUSICDIR = ./music/

# Hiscore directory (where the hiscores are written to), default: ./
# We need read/write access!
HISCOREDIR = ./

# Config directory (where the 'config.cfg' is), default: ./
# We need read/write access!
CONFIGDIR = ./

# Override directory (unused at the moment), default: ./data/
OVERRIDEDIR = ./data/


DIR_DEFINES = -DMPK_DIR=\"$(MPKDIR)\" -DMUSIC_DIR=\"$(MUSICDIR)\" -DHISCORE_DIR=\"$(HISCOREDIR)\" -DCONFIG_DIR=\"$(CONFIGDIR)\" -DOVERRIDE_DIR=\"$(OVERRIDEDIR)\"


# SDL include directory
#SDL_INCLUDES = c:\mingw\include\SDL
SDL_INCLUDES = /mingw32/include/SDL
FMOD_INCLUDES = ./fmod3/include

# Debug mode settings
ifdef DEBUGMODE
CFLAGS = -MMD -g3 -W -Wall -mcpu=$(MARCH) -DDEBUGMODE
LDFLAGS = -L/mingw32/lib -L./fmod3/lib -mwindows -lmingw32 -lSDLmain -lSDL -lSDL_image -lopengl32 -lglu32 -lfmod
else
# Profile mode settings
ifdef PROFILE
CFLAGS = -MMD -g3 -O3 -march=$(MARCH) -Wall -pg
LDFLAGS = -L/mingw32/lib -L./fmod3/lib -mwindows -lmingw32 -lSDLmain -lSDL -lSDL_image -lopengl32 -lglu32 -lfmod -pg
else
# Normal build settings
CFLAGS = -MMD -O3 -march=$(MARCH) -Wall
LDFLAGS =-static -L/mingw32/lib -L./fmod3/lib  -mwindows -lmingw32 -lSDLmain -lSDL -lSDL_image -lopengl32 -lglu32 -lfmod -s -lSDL_gfx -lSDL_mixer  -lvorbisfile -lvorbis -lmingw32 -lbz2 -lharfbuzz -lglib-2.0 -lintl -liconv -ltiff -ljpeg -llzma -lpng16 -lstdc++ -lwebp -lwinpthread -lz -larchive -lwinmm -lgdi32 -ldxguid -lasprintf -lcharset -lcrypto -lcurl -lexpat -lffi -lFLAC++ -lFLAC -lfontconfig -lformw -lfreeglut_static -lgdbm -lgettextlib -lgettextpo -lgif -lgio-2.0 -lglew32 -lglew32mx -lgmodule-2.0 -lgmp -lgmpxx -lgnurx -lgnutls -lgnutlsxx -lgobject-2.0 -lgthread-2.0 -lhistory -lhogweed -lidn -lisl -ljansson  -ljsoncpp -llua  -llzo2  -lmenuw -lmetalink -lminizip -lmpc -lmpfr -lncurses++w -lncursesw -lnettle -lnghttp2 -logg -lopenal -lpanelw -lphysfs -lpixman-1 -lreadline -lregex -lrtmp -lssh2 -lssl -lsystre -ltasn1 -ltclstub86 -ltermcap -ltheora -ltheoradec -ltheoraenc -ltkstub86 -ltre -lturbojpeg -lvorbisenc -lwebpdecoder -lwebpdemux -lwebpmux -lole32 -lws2_32
endif
endif

# Source and object files
SOURCES = $(wildcard src/*.cpp)
OBJS = $(SOURCES:.cpp=.o)
OBJS := $(subst src/,obj/,$(OBJS))

# Include directories
INCLUDES = -I./include -I$(SDL_INCLUDES) -I$(FMOD_INCLUDES)


# Targets
all: $(TARGET)

# Check dependancies
DEPS = $(subst .o,.d,$(OBJS))
-include $(DEPS)

$(TARGET): $(OBJS) $(RESFILE)
	$(CC) -o $(TARGET) $(OBJS) $(RESFILE) $(LDFLAGS)


clean:
	rm -f $(OBJS) $(TARGET)

veryclean:
	rm -f $(OBJS) $(TARGET) $(DEPS)

rebuild: veryclean all

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $(DIR_DEFINES) -c $< -o $@

# Resources
$(RESFILE): $(RCFILE)
	$(WINDRES) -i $< -I rc -o $@ -O coff

# Compress the exe with UPX
compress: $(TARGET)
	$(COMPRESS) $(TARGET)
