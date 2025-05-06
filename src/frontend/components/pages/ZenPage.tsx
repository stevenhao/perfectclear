import React, { useEffect, useRef, useState } from 'react';
import { initializeWasm } from '../../services/wasm';

export function ZenPage() {
  const zenContentRef = useRef<HTMLDivElement>(null);
  const [isFullScreen, setIsFullScreen] = useState(false);

  const toggleFullScreen = () => {
    if (!document.fullscreenElement) {
      if (zenContentRef.current?.requestFullscreen) {
        zenContentRef.current.requestFullscreen()
          .then(() => setIsFullScreen(true))
          .catch(err => console.error(`Error attempting to enable fullscreen: ${err.message}`));
      }
    } else {
      if (document.exitFullscreen) {
        document.exitFullscreen()
          .then(() => setIsFullScreen(false))
          .catch(err => console.error(`Error attempting to exit fullscreen: ${err.message}`));
      }
    }
  };

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
    
    const handleFullScreenChange = () => {
      setIsFullScreen(!!document.fullscreenElement);
    };
    
    document.addEventListener('fullscreenchange', handleFullScreenChange);
    
    return () => {
      (window as any).zenMode = false;
      document.removeEventListener('fullscreenchange', handleFullScreenChange);
    };
  }, []);

  return (
    <div className="zen-container" style={{ width: '100%', height: '100vh', overflow: 'hidden', position: 'relative' }}>
      <div id="zen-content" ref={zenContentRef}></div>
      
      <button 
        onClick={toggleFullScreen}
        style={{
          position: 'absolute',
          top: '10px',
          right: '10px',
          zIndex: 1000,
          padding: '8px 12px',
          backgroundColor: '#333',
          color: 'white',
          border: 'none',
          borderRadius: '4px',
          cursor: 'pointer',
          opacity: '0.7',
          transition: 'opacity 0.3s'
        }}
        onMouseOver={(e) => (e.currentTarget.style.opacity = '1')}
        onMouseOut={(e) => (e.currentTarget.style.opacity = '0.7')}
      >
        {isFullScreen ? 'Exit Full Screen' : 'Full Screen'}
      </button>
    </div>
  );
}
