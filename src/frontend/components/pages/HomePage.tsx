import React, { useEffect, useState } from 'react';
import { initializeWasm } from '../../services/wasm';
import { GameBoard } from '../game/GameBoard';
import { GameControls } from '../game/GameControls';

export function HomePage() {
  const [wasmLoaded, setWasmLoaded] = useState(false);
  
  useEffect(() => {
    const loadWasm = async () => {
      try {
        const loaded = await initializeWasm();
        setWasmLoaded(loaded);
      } catch (error) {
        console.error('Error loading WASM:', error);
      }
    };
    
    loadWasm();
  }, []);

  return (
    <div className="row">
      <div className="col-md-6">
        <div className="panel panel-default">
          <div className="panel-heading">
            <h3 className="panel-title">💯💯💯💯💯</h3>
          </div>
          <div className="panel-body" style={{ position: 'relative', textAlign: 'center' }}>
            <GameControls wasmLoaded={wasmLoaded} />
          </div>
        </div>
      </div>
      <div className="col-md-6">
        <div className="panel panel-default">
          <div className="panel-heading">
            <h3 className="panel-title" id="happy-indicator">🤓</h3>
          </div>
          <div className="panel-body" style={{ position: 'relative' }}>
            <GameBoard />
          </div>
          <div id="counter" style={{ padding: '20px', fontSize: '14pt' }}>
            0
          </div>
        </div>
      </div>
    </div>
  );
}
