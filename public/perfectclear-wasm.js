let wasmModule = null;

async function fetchFile(url) {
  try {
    const response = await fetch(url);
    if (!response.ok) {
      throw new Error(`Failed to fetch ${url}: ${response.status} ${response.statusText}`);
    }
    return await response.text();
  } catch (error) {
    console.error(`Error fetching ${url}:`, error);
    throw error;
  }
}

function loadWasmModule() {
  return new Promise((resolve, reject) => {
    createPerfectClear().then(async function(Module) {
      wasmModule = Module;
      
      try {
        console.log('Loading data files...');
        
        Module._wasm_init_engine();
        
        console.log('PerfectClear WASM module initialized');
        resolve(Module);
      } catch (error) {
        console.error('Error during initialization:', error);
        reject(error);
      }
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
  
  try {
    console.log('Board dimensions:', board.length, 'x', board[0].length);
    console.log('Pieces:', pieces);
    
    const boardJson = JSON.stringify({
      W: board.length,
      H: board[0].length,
      data: Board.encode(board)
    });
    
    const piecesJson = JSON.stringify(pieces);
    
    console.log('Board JSON:', boardJson);
    console.log('Pieces JSON:', piecesJson);
    
    const boardPtr = wasmModule.allocateUTF8(boardJson);
    const piecesPtr = wasmModule.allocateUTF8(piecesJson);
    
    try {
      const resultPtr = wasmModule._wasm_get_best_move(boardPtr, piecesPtr, searchBreadth || 200);
      
      if (!resultPtr) {
        throw new Error('Null result pointer returned from WASM function');
      }
      
      const resultStr = wasmModule.UTF8ToString(resultPtr);
      console.log('Raw result from WASM:', resultStr);
      
      const result = JSON.parse(resultStr);
      wasmModule._wasm_free_string(resultPtr);
      
      if (result.error) {
        console.error('WASM error:', result.error);
        callback({ error: result.error, details: result });
      } else {
        callback(result);
      }
    } catch (error) {
      console.error('Error in WASM execution:', error);
      callback({ error: 'WASM execution error: ' + error.message });
    } finally {
      try {
        if (boardPtr) wasmModule.UTF8ToString(boardPtr); // Use the string before freeing
        if (piecesPtr) wasmModule.UTF8ToString(piecesPtr); // Use the string before freeing
      } catch (e) {
        console.error('Error accessing memory before cleanup:', e);
      }
    }
  } catch (error) {
    console.error('Error in wasmAiMove:', error);
    callback({ error: 'JavaScript error: ' + error.message });
  }
}

window.loadWasmModule = loadWasmModule;
window.wasmAiMove = wasmAiMove;
