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
    updateWasmStatus('loading');
    
    if (!checkWasmSupport()) {
      console.error('WebAssembly is not supported in this browser');
      updateWasmStatus('offline');
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
        updateWasmStatus('online');
        resolve(Module);
      } catch (error) {
        console.error('Error during initialization:', error);
        updateWasmStatus('offline');
        reject(error);
      }
    }).catch(error => {
      console.error('Failed to load WASM module:', error);
      updateWasmStatus('offline');
      reject(error);
    });
  });
}

function updateWasmStatus(status) {
  const $statusLight = $('#wasm-status-light');
  const $statusText = $('#wasm-status-text');
  const $autoplay = $('#autoplay');
  const $loadingOverlay = $('#loading-overlay');
  
  if ($statusLight.length && $statusText.length) {
    if (status === 'online') {
      $statusLight.text('●').removeClass('offline loading').addClass('online');
      $statusText.text('AI Engine: Online (WebAssembly)');
      $autoplay.prop('disabled', false).css('opacity', '1');
      
      if ($loadingOverlay.length) {
        $loadingOverlay.removeClass('active');
      }
      
      if (window.zenMode && !window.autoplaying) {
        setTimeout(function() {
          $autoplay.click();
        }, 500);
      }
    } else if (status === 'loading') {
      $statusLight.text('●').removeClass('offline online').addClass('loading');
      $statusText.text('AI Engine: Loading...');
      $autoplay.prop('disabled', true).css('opacity', '0.5');
      
      if ($loadingOverlay.length) {
        $loadingOverlay.addClass('active');
      }
    } else {
      $statusLight.text('●').removeClass('online loading').addClass('offline');
      $statusText.text('AI Engine: Offline (WebAssembly not supported)');
      $autoplay.prop('disabled', false).css('opacity', '1');
      
      if ($loadingOverlay.length) {
        $loadingOverlay.removeClass('active');
      }
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
    
    const sanitizedPieces = pieces.map(p => {
      if (typeof p === 'string') return p;
      if (p && typeof p === 'object') {
        if (p.nodeType || p.jquery) return null;
        if (p.pieceType) return p.pieceType;
      }
      return null;
    }).filter(p => p !== null);
    
    const piecesJson = JSON.stringify(sanitizedPieces);
    
    console.log('Board JSON:', boardJson);
    console.log('Sanitized Pieces:', sanitizedPieces);
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
      
      if (wasmModule._wasm_free_string) {
        wasmModule._wasm_free_string(resultPtr);
      }
      
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
        if (boardPtr && typeof wasmModule._free === 'function') {
          wasmModule._free(boardPtr);
        }
        if (piecesPtr && typeof wasmModule._free === 'function') {
          wasmModule._free(piecesPtr);
        }
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
  
  if (window.zenMode && (!window.board || !window.curPiece || !window.preview)) {
    console.log('Initializing game state for zen mode');
    
    if (!window.board) {
      try {
        if (window.Board && typeof window.Board.create === 'function') {
          window.board = window.Board.create(10, 10);
        } else {
          console.log('Board.create not available, creating empty board array');
          window.board = Array(10).fill().map(() => Array(10).fill(0));
        }
      } catch (e) {
        console.error('Error creating board:', e);
        window.board = Array(10).fill().map(() => Array(10).fill(0));
      }
    }
    
    if (!window.bag) {
      window.bag = window.makeBag ? window.makeBag() : ['I', 'O', 'T', 'L', 'J', 'S', 'Z'];
    }
    
    if (!window.nextBag) {
      window.nextBag = window.makeBag ? window.makeBag() : ['I', 'O', 'T', 'L', 'J', 'S', 'Z'];
    }
    
    if (!window.preview) {
      window.preview = window.bag.slice(0, 6);
    }
    
    if (!window.curPiece) {
      const pieceType = window.bag.shift();
      window.curPiece = window.Piece ? window.Piece.create(pieceType, window.board) : { pieceType: pieceType, location: [4, 0], rotation: 0 };
      
      if (window.preview && window.preview.length < 6) {
        window.preview.push(window.nextBag.shift());
      }
    }
    
    if (window.hold === undefined) {
      window.hold = null;
    }
    
    if (!window.counter) {
      window.counter = { pieces: 0, clears: 0 };
    }
    
    if (window.renderBoardWithPiece) {
      window.renderBoardWithPiece(window.board, window.curPiece);
    }
    if (window.renderPreview) {
      window.renderPreview(window.preview);
    }
    if (window.renderHold) {
      window.renderHold(window.hold);
    }
    if (window.renderCounter) {
      window.renderCounter(window.counter);
    }
    
    setTimeout(window.autoplayStep, 500);
    return;
  }
  
  if (!window.board || !window.curPiece || !window.preview) {
    console.error('Game state not initialized for autoplay');
    setTimeout(window.autoplayStep, 500); // Try again after a delay
    return;
  }
  
  try {
    const board = window.board;
    const curPiece = window.curPiece;
    const hold = window.hold;
    const preview = window.preview || [];
    const bag = window.bag || [];
    
    console.log('Autoplay step with piece:', curPiece);
    
    if (!curPiece.pieceType) {
      console.error('Current piece has no pieceType:', curPiece);
      if (window.Piece && window.bag && window.bag.length > 0) {
        window.curPiece = window.Piece.create(window.bag.shift(), board);
        setTimeout(window.autoplayStep, 500); // Try again after a delay
      }
      return;
    }
    
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
        setTimeout(window.autoplayStep, 1000); // Try again after a delay
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
  } catch (error) {
    console.error('Error in autoplayStep:', error);
    setTimeout(window.autoplayStep, 1000); // Try again after a delay
  }
}

window.loadWasmModule = loadWasmModule;
window.wasmAiMove = wasmAiMove;
window.autoplayStep = autoplayStep;
window.checkWasmSupport = checkWasmSupport;
window.updateWasmStatus = updateWasmStatus;
window.isWasmSupported = function() { return wasmSupported; };
window.isWasmInitialized = function() { return wasmInitialized; };
