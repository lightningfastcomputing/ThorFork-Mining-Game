CC = g++ 
CFLAGS = -std=c++20 -Wall
LINKER_FLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_net
SRCDIR = Src
BUILDDIR = Build

INCLUDE_PATHS = -I/mingw64/include -I/mingw64/include/SDL2
LIBRARY_PATHS = -L/mingw64/lib 

SRCS = $(shell find $(SRCDIR) -name '*.cpp')
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)

TARGET = Main.exe

all: $(TARGET)

debug: CFLAGS += -ggdb -O0
debug: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(CFLAGS) -o $@ $^ $(LINKER_FLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BUILDDIR)/*.o $(TARGET)