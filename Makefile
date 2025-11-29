# Native build (default)
CC = g++ 
CFLAGS = -std=c++20 -Wall
LINKER_FLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
INCLUDE_PATHS = -I/mingw64/include -I/mingw64/include/SDL2
LIBRARY_PATHS = -L/mingw64/lib 

# WASM build
EMCC = em++
EMCC_FLAGS = -std=c++20 -Wall -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' \
             -s USE_SDL_TTF=2 -s USE_SDL_NET=2 \
             -s ALLOW_MEMORY_GROWTH=1 -s EXIT_RUNTIME=1 --preload-file Textures --preload-file Sounds \
			

SRCDIR = Src
BUILDDIR = Build
WASMOBJDIR = Build/wasm

SRCS = $(shell find $(SRCDIR) -name '*.cpp')
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)
WASM_OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(WASMOBJDIR)/%.o)

MAIN_FILES := $(wildcard Main*)

TARGET = Main.exe
WASM_TARGET = Main.html

# Default target
all: $(TARGET)

debug: CFLAGS += -ggdb -O0
debug: $(TARGET)

# Native binary
$(TARGET): $(OBJS)
	$(CC) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(CFLAGS) -o $@ $^ $(LINKER_FLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(INCLUDE_PATHS) $(CFLAGS) -c $< -o $@

# WASM object build
$(WASMOBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(EMCC) $(EMCC_FLAGS) -c $< -o $@

# WASM link step
wasm: $(WASM_OBJS)
	$(EMCC) $(EMCC_FLAGS) -o $(WASM_TARGET) $^

# Clean
clean:
	rm -f $(BUILDDIR)/*.o $(BUILDDIR)/Entity/*.o $(WASMOBJDIR)/*.o $(WASMOBJDIR)/Entity/*.o $(MAIN_FILES)

clean-wasm:
	rm -f $(WASMOBJDIR)/*.o $(WASMOBJDIR)/Entity/*.o $(WASM_TARGET) $(WASM_TARGET:.js=.wasm) $(WASM_TARGET:.js=.data)

clean-native:
	rm -f $(BUILDDIR)/*.o $(TARGET) $(BUILDDIR)/Entity/*.o

