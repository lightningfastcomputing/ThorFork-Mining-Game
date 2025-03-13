CC = g++ 
CFLAGS = -std=c++20 -Wall
LINKER_FLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
SRCDIR = Src
BUILDDIR = Build

INCLUDE_PATHS = -I C:/dev/SDL2-2.32.2/i686-w64-mingw32/include
LIBRARY_PATHS = -L C:/dev/SDL2-2.32.2/i686-w64-mingw32/lib 

# $(shell mkdir $(BUILDDIR))

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SRCS))

TARGET = Main.exe

all: $(TARGET)

debug: CFLAGS += -ggdb -O0 
debug: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(CFLAGS) -o $@ $^ $(LINKER_FLAGS) 

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

clean:
	del /Q $(BUILDDIR)\*.o $(TARGET)