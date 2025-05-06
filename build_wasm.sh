
mkdir -p dist

emcc -O3 \
     wasm_bindings.cpp \
     -I. \
     -o dist/perfectclear.js \
     -s WASM=1 \
     -s EXPORTED_FUNCTIONS='["_wasm_get_best_move", "_wasm_init_engine", "_wasm_free_string"]' \
     -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "UTF8ToString", "allocateUTF8"]' \
     -s ALLOW_MEMORY_GROWTH=1 \
     -s MODULARIZE=1 \
     -s 'EXPORT_NAME="createPerfectClear"'

echo "WebAssembly compilation complete!"
