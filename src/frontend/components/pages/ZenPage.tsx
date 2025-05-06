import React, { useEffect } from 'react';

export function ZenPage() {
  useEffect(() => {
    const loadZenMode = async () => {
      try {
        console.log('WASM module loaded successfully in zen mode');
        (window as any).zenMode = true;
      } catch (error) {
        console.error('Error initializing zen mode:', error);
      }
    };
    
    loadZenMode();
  }, []);

  return (
    <div className="zen-container">
      {/* Zen mode content will be populated via JavaScript similar to the original implementation */}
      <div id="zen-content"></div>
    </div>
  );
}
