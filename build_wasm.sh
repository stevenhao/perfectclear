

mkdir -p dist

cp pieces centers kicks book_100k.txt dist/

emcc -O3 \
     wasm_bindings.cpp \
     -I. \
     -o dist/perfectclear.js \
     -s WASM=1 \
     -s EXPORTED_FUNCTIONS='["_wasm_get_best_move", "_wasm_init_engine", "_wasm_free_string"]' \
     -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "UTF8ToString", "allocateUTF8"]' \
     -s ALLOW_MEMORY_GROWTH=1 \
     -s MODULARIZE=1 \
     -s 'EXPORT_NAME="createPerfectClear"' \
     --preload-file pieces@/pieces \
     --preload-file centers@/centers \
     --preload-file kicks@/kicks \
     --preload-file book_100k.txt@/book_100k.txt

mkdir -p public/dist
cp dist/perfectclear.js dist/perfectclear.wasm dist/perfectclear.data public/dist/

echo "WebAssembly compilation complete!"
