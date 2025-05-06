let wasmModule = null;

function loadWasmModule() {
  return new Promise((resolve, reject) => {
    createPerfectClear().then(function(Module) {
      wasmModule = Module;
      
      Module._wasm_init_engine();
      
      console.log('PerfectClear WASM module initialized');
      resolve(Module);
    }).catch(error => {
      console.error('Failed to load WASM module:', error);
      reject(error);
    });
  });
}

function wasmAiMove(board, pieces, searchBreadth, callback) {
  if (!wasmModule) {
    console.error('WASM module not loaded');
    return;
  }
  
  const boardJson = JSON.stringify({
    W: board.length,
    H: board[0].length,
    data: Board.encode(board)
  });
  
  const piecesJson = JSON.stringify(pieces);
  
  const boardPtr = wasmModule.allocateUTF8(boardJson);
  const piecesPtr = wasmModule.allocateUTF8(piecesJson);
  
  const resultPtr = wasmModule._wasm_get_best_move(boardPtr, piecesPtr, searchBreadth || 200);
  
  const result = JSON.parse(wasmModule.UTF8ToString(resultPtr));
  wasmModule._wasm_free_string(resultPtr);
  wasmModule._free(boardPtr);
  wasmModule._free(piecesPtr);
  
  callback(result);
}

window.loadWasmModule = loadWasmModule;
window.wasmAiMove = wasmAiMove;
