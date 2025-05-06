import React, { useEffect, useRef } from 'react';
import { initializeWasm } from '../../services/wasm';

export function ZenPage() {
  const zenContentRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    const loadZenMode = async () => {
      try {
        await initializeWasm();
        
        (window as any).zenMode = true;
        console.log('WASM module loaded successfully in zen mode');
        
        if (zenContentRef.current) {
          const gameContainer = document.createElement('div');
          gameContainer.id = 'game-container';
          gameContainer.style.width = '100%';
          gameContainer.style.height = '100vh';
          gameContainer.style.display = 'flex';
          gameContainer.style.justifyContent = 'center';
          gameContainer.style.alignItems = 'center';
          
          const gameBoard = document.createElement('div');
          gameBoard.id = 'game-board';
          gameBoard.style.width = '400px';
          gameBoard.style.height = '600px';
          gameBoard.style.backgroundColor = '#333';
          gameBoard.style.position = 'relative';
          
          gameContainer.appendChild(gameBoard);
          zenContentRef.current.appendChild(gameContainer);
          
          console.log('Zen mode initialized');
        }
      } catch (error) {
        console.error('Error initializing zen mode:', error);
      }
    };
    
    loadZenMode();
    
    return () => {
      (window as any).zenMode = false;
    };
  }, []);

  return (
    <div className="zen-container" style={{ width: '100%', height: '100vh', overflow: 'hidden' }}>
      <div id="zen-content" ref={zenContentRef}></div>
    </div>
  );
}
