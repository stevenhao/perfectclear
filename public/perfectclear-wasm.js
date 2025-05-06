let wasmModule = null;
let wasmSupported = false;
let wasmInitialized = false;

function checkWasmSupport() {
  try {
    if (typeof WebAssembly === 'object' && 
        typeof WebAssembly.instantiate === 'function') {
      const module = new WebAssembly.Module(new Uint8Array([
        0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00
      ]));
      if (module instanceof WebAssembly.Module) {
        const instance = new WebAssembly.Instance(module);
        wasmSupported = instance instanceof WebAssembly.Instance;
        return wasmSupported;
      }
    }
  } catch (e) {
    console.error('WebAssembly not supported:', e);
  }
  wasmSupported = false;
  return false;
}

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
    if (!checkWasmSupport()) {
      console.error('WebAssembly is not supported in this browser');
      updateWasmStatus(false);
      reject(new Error('WebAssembly not supported'));
      return;
    }

    createPerfectClear().then(async function(Module) {
      wasmModule = Module;
      
      try {
        console.log('Loading data files...');
        
        Module._wasm_init_engine();
        
        console.log('PerfectClear WASM module initialized');
        wasmInitialized = true;
        updateWasmStatus(true);
        resolve(Module);
      } catch (error) {
        console.error('Error during initialization:', error);
        updateWasmStatus(false);
        reject(error);
      }
    }).catch(error => {
      console.error('Failed to load WASM module:', error);
      updateWasmStatus(false);
      reject(error);
    });
  });
}

function updateWasmStatus(isOnline) {
  const $statusLight = $('#wasm-status-light');
  const $statusText = $('#wasm-status-text');
  
  if ($statusLight.length && $statusText.length) {
    if (isOnline) {
      $statusLight.text('●').removeClass('offline').addClass('online');
      $statusText.text('AI Engine: Online (WebAssembly)');
    } else {
      $statusLight.text('●').removeClass('online').addClass('offline');
      $statusText.text('AI Engine: Offline (WebAssembly not supported)');
    }
  }
}

function wasmAiMove(board, pieces, searchBreadth, callback) {
  if (!wasmModule) {
    console.error('WASM module not loaded');
    callback({ error: 'WASM module not loaded' });
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
        if (boardPtr) wasmModule._free(boardPtr);
        if (piecesPtr) wasmModule._free(piecesPtr);
      } catch (e) {
        console.error('Error freeing memory:', e);
      }
    }
  } catch (error) {
    console.error('Error in wasmAiMove:', error);
    callback({ error: 'JavaScript error: ' + error.message });
  }
}

function autoplayStep() {
  if (!window.autoplaying) {
    return;
  }
  
  const board = window.board;
  const curPiece = window.curPiece;
  const hold = window.hold;
  const preview = window.preview;
  const bag = window.bag;
  
  let p = [curPiece.pieceType].concat(preview);
  if (hold) {
    p = [curPiece.pieceType, hold].concat(preview);
  }
  
  wasmAiMove(board, p, 200, function(data) {
    if (!window.autoplaying) {
      return;
    }
    
    console.log('AI move result:', data);
    
    if (data.error) {
      console.error('Error in AI move:', data.error);
      return;
    }
    
    const path = data.path;
    const animationTime = window.zenMode ? 300 : 500;
    const interval = animationTime / (path.length + 1);
    let time = 0;
    
    if (window.save) {
      window.save();
    }
    
    let piece = window.Piece.create(curPiece.pieceType, board);
    
    path.forEach(function(mv) {
      piece = window.AIcommands[mv](piece, board);
      const draw = piece;
      time += interval;
      
      setTimeout(function() {
        window.renderBoardWithPiece(board, draw);
      }, time);
    });
    
    setTimeout(function() {
      window.renderBoardWithPiece(board, piece);
    }, animationTime);
    
    setTimeout(function() {
      if (!window.Board.canAddPiece(board, piece)) {
        window.Board.reset(board);
        window.bag = window.nextBag;
        window.nextBag = window.makeBag();
        window.hold = null;
        window.curPiece = window.nextPiece();
        window.renderHold(window.hold);
        window.renderBoardWithPiece(board, window.curPiece);
        window.renderPreview(window.preview);
      } else {
        piece = window.Piece.hardDrop(piece, board);
        window.board = window.Board.addPiece(board, piece);
        window.curPiece = window.nextPiece();
        window.renderBoardWithPiece(window.board, window.curPiece);
        window.renderPreview(window.preview);
        
        if (window.counter) {
          window.counter.pieces += 1;
          if (window.Board.isEmpty(window.board)) {
            window.counter.clears += 1;
          }
          window.renderCounter(window.counter);
        }
      }
    }, animationTime * 1.1);
    
    if (window.autoplaying) {
      setTimeout(window.autoplayStep, animationTime * 1.2);
    }
  });
}

window.loadWasmModule = loadWasmModule;
window.wasmAiMove = wasmAiMove;
window.autoplayStep = autoplayStep;
window.checkWasmSupport = checkWasmSupport;
window.updateWasmStatus = updateWasmStatus;
window.isWasmSupported = function() { return wasmSupported; };
window.isWasmInitialized = function() { return wasmInitialized; };
