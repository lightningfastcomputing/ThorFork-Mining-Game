CC = g++ 
CFLAGS = -std=c++20 -Wall
LINKER_FLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_net
SRCDIR = Src
BUILDDIR = Build

INCLUDE_PATHS = -I/mingw64/include -I/mingw64/include/SDL2
LIBRARY_PATHS = -L/mingw64/lib 

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
	rm  $(BUILDDIR)/*.o $(TARGET)