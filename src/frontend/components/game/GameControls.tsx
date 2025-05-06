import React, { useState } from 'react';
import { requestAiMove } from '../../services/api';

interface GameControlsProps {
  wasmLoaded: boolean;
}

export function GameControls({ wasmLoaded }: GameControlsProps) {
  const [isProcessing, setIsProcessing] = useState(false);
  const [isAutoplay, setIsAutoplay] = useState(false);
  const [stopWhenSolutionFound, setStopWhenSolutionFound] = useState(false);
  const [faster, setFaster] = useState(false);
  
  const handleGenerateMove = async () => {
    if (isProcessing) return;
    
    setIsProcessing(true);
    try {
      const board = {
        W: "10",
        H: "20",
        data: "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
      };
      const pieces = ["I", "O", "T"];
      
      const result = await requestAiMove({
        board,
        pieces,
        search_breadth: 200
      });
      
      console.log('AI move result:', result);
      
    } catch (error) {
      console.error('Error generating AI move:', error);
    } finally {
      setIsProcessing(false);
    }
  };
  
  const toggleAutoplay = () => {
    setIsAutoplay(!isAutoplay);
  };
  
  return (
    <div className="game-controls">
      <button 
        id="piece-send"
        tabIndex={-1}
        type="button" 
        className="btn btn-block"
        onClick={handleGenerateMove}
        disabled={!wasmLoaded || isProcessing}
      >
        Send Next Piece
      </button>
      
      <button 
        id="autoplay"
        type="button" 
        className="btn btn-block"
        onClick={toggleAutoplay}
      >
        {isAutoplay ? 'Stop Autoplay' : 'Autoplay'}
      </button>
      
      <div>
        <button className="btn" id="undo">Undo</button>
        <button className="btn" id="redo">Redo</button>
        <button className="btn" id="reset">Reset</button>
      </div>
      
      <div>
        <label style={{ cursor: 'pointer' }}>
          <input 
            type="checkbox" 
            id="stop-controller" 
            checked={stopWhenSolutionFound}
            onChange={() => setStopWhenSolutionFound(!stopWhenSolutionFound)}
          /> 
          Stop when solution is found
        </label>
      </div>
      
      <div>
        <label style={{ cursor: 'pointer' }}>
          <input 
            type="checkbox" 
            id="fast-controller"
            checked={faster}
            onChange={() => setFaster(!faster)}
          />
          Faster
        </label>
      </div>
      
      <div className="status-indicator" style={{ marginTop: '10px' }}>
        WASM Status: {wasmLoaded ? 
          <span className="text-success">Loaded</span> : 
          <span className="text-warning">Loading...</span>
        }
      </div>
    </div>
  );
}
