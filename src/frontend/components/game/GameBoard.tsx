import React, { useEffect, useRef } from 'react';

export function GameBoard() {
  return (
    <div>
      <div id="hold" style={{ float: 'left', marginLeft: '1em', position: 'relative', width: '6em', height: '6em', backgroundColor: 'black' }}>
      </div>

      <div id="game-outer" style={{ float: 'left', position: 'relative', width: '20em', height: '20em' }}>
        <div id="game-inner" data-h="10" data-w="10" className="grid">
        </div>
      </div>

      <div id="previews" style={{ float: 'left', marginLeft: '1em', position: 'relative', width: '4em', height: '20em', backgroundColor: 'black' }}>
      </div>
    </div>
  );
}
