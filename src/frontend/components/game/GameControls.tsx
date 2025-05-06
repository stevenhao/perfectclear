import React, { useState } from 'react';
import { requestAiMove } from '../../services/api';

interface GameControlsProps {
  wasmLoaded: boolean;
}

export function GameControls({ wasmLoaded }: GameControlsProps) {
  const [isProcessing, setIsProcessing] = useState(false);
  
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
  
  return (
    <div className="game-controls">
      <button 
        className="btn btn-primary"
        onClick={handleGenerateMove}
        disabled={!wasmLoaded || isProcessing}
      >
        {isProcessing ? 'Processing...' : 'Generate Move'}
      </button>
      
      <div className="status-indicator">
        WASM Status: {wasmLoaded ? 
          <span className="text-success">Loaded</span> : 
          <span className="text-warning">Loading...</span>
        }
      </div>
      
      <div className="controls-info">
        <p>Click "Generate Move" to have the AI suggest the next best move.</p>
      </div>
    </div>
  );
}
