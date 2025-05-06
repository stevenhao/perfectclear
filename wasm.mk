CC = emcc
CXX = em++

# Emscripten flags
EMFLAGS = -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_RUNTIME_METHODS=['FS'] -s MODULARIZE=1 -s EXPORT_ES6=1

# Include directories
INCLUDES = 

# Compiler flags
CXXFLAGS = -O3 $(INCLUDES) $(EMFLAGS)

# Source files
SRCS = wasm_engine.cpp

# Output files
OUTPUT_DIR = public/wasm
OUTPUT_JS = $(OUTPUT_DIR)/perfectclear.js
OUTPUT_WASM = $(OUTPUT_DIR)/perfectclear.wasm

# Data files to copy
DATA_DIR = public/data
DATA_FILES = pieces centers kicks book_100k.txt

all: wasm data

wasm: $(OUTPUT_JS)

$(OUTPUT_JS): $(SRCS)
	mkdir -p $(OUTPUT_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

data: 
	mkdir -p $(DATA_DIR)
	cp $(DATA_FILES) $(DATA_DIR)/

clean:
	rm -f $(OUTPUT_JS) $(OUTPUT_WASM)
	rm -rf $(DATA_DIR)

.PHONY: all wasm data clean
