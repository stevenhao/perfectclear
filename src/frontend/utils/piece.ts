import { Board } from './board';

type PieceType = 'I' | 'J' | 'L' | 'O' | 'S' | 'T' | 'Z';

const PieceData = {
  pieceLocs: {
    I: [[-1.5, 0.5], [-0.5, 0.5], [0.5, 0.5], [1.5, 0.5]],
    J: [[-1, 1], [-1, 0], [0, 0], [1, 0]],
    L: [[-1, 0], [0, 0], [1, 1], [1, 0]],
    O: [[-0.5, 0.5], [-0.5, -0.5], [0.5, 0.5], [0.5, -0.5]],
    S: [[-1, 0], [0, 0], [0, 1], [1, 1]],
    T: [[-1, 0], [0, 0], [0, 1], [1, 0]],
    Z: [[-1, 1], [0, 1], [0, 0], [1, 0]]
  },
  pieceIndices: {
    I: 1,
    J: 2,
    L: 3,
    O: 4,
    S: 5,
    T: 6,
    Z: 7
  },
  SRS: {
    S: {
      A: [[[0, 0], [-1, 0], [-1, +1], [0, -2], [-1, -2]],
          [[0, 0], [+1, 0], [+1, -1], [0, +2], [+1, +2]],
          [[0, 0], [+1, 0], [+1, +1], [0, -2], [+1, -2]],
          [[0, 0], [-1, 0], [-1, -1], [0, +2], [-1, +2]]],
      B: [[[0, 0], [+1, 0], [+1, +1], [0, -2], [+1, -2]],
          [[0, 0], [+1, 0], [+1, -1], [0, +2], [+1, +2]],
          [[0, 0], [-1, 0], [-1, +1], [0, -2], [-1, -2]],
          [[0, 0], [-1, 0], [-1, -1], [0, +2], [-1, +2]]]
    },
    I: {
      A: [[[0, 0], [-2, 0], [+1, 0], [-2, -1], [+1, +2]],
          [[0, 0], [-1, 0], [+2, 0], [-1, +2], [+2, -1]],
          [[0, 0], [+2, 0], [-1, 0], [+2, +1], [-1, -2]],
          [[0, 0], [+1, 0], [-2, 0], [+1, -2], [-2, +1]]],
      B: [[[0, 0], [-1, 0], [+2, 0], [-1, +2], [+2, -1]],
          [[0, 0], [+2, 0], [-1, 0], [+2, +1], [-1, -2]],
          [[0, 0], [+1, 0], [-2, 0], [+1, -2], [-2, +1]],
          [[0, 0], [-2, 0], [+1, 0], [-2, -1], [+1, +2]]],
    }
  },
  startingLocations: {
    I: (w: number, h: number) => [Math.floor(w/2)-0.5, h-1.5],
    J: (w: number, h: number) => [Math.floor((w-1)/2), h-2],
    L: (w: number, h: number) => [Math.floor((w-1)/2), h-2],
    O: (w: number, h: number) => [Math.floor(w/2)-0.5, h-1.5],
    S: (w: number, h: number) => [Math.floor((w-1)/2), h-2],
    T: (w: number, h: number) => [Math.floor((w-1)/2), h-2],
    Z: (w: number, h: number) => [Math.floor((w-1)/2), h-2],
  }
};

export const Location = {
  translate: ([offX, offY]: [number, number]) => 
    ([locX, locY]: [number, number]): [number, number] => [offX + locX, offY + locY],

  rotate: (rot: number) => {
    rot = ((rot % 4) + 4) % 4;
    return ([locX, locY]: [number, number]): [number, number] => {
      if (rot === 0) return [locX, locY];
      else if (rot === 1) return [locY, -locX];
      else if (rot === 2) return [-locX, -locY];
      else return [-locY, locX];
    };
  }
};

export const PieceType = {
  blocks: (pieceType: PieceType, rotation = 0) => 
    PieceData.pieceLocs[pieceType].map((loc) => Location.rotate(rotation)(loc as [number, number])),

  getIndex: (pieceType: PieceType) => 
    PieceData.pieceIndices[pieceType]
};

export interface PieceInterface {
  location: [number, number];
  rotation: number;
  pieceType: PieceType;
  blocks: () => [number, number][];
}

export class Piece {
  static create(pieceType: PieceType, board: number[][]): PieceInterface {
    const [w, h] = Board.dims(board);
    const piece = {
      location: PieceData.startingLocations[pieceType](w, h) as [number, number],
      rotation: 0,
      pieceType: pieceType,
      blocks: function() {
        return PieceType.blocks(this.pieceType, this.rotation)
          .map((loc) => Location.translate(this.location as [number, number])(loc));
      }
    };
    
    return piece;
  }

  static getKicks(piece: PieceInterface, dir: 'A' | 'B') {
    if (piece.pieceType === 'I') {
      return PieceData.SRS.I[dir][piece.rotation];
    } else {
      return PieceData.SRS.S[dir][piece.rotation];
    }
  }

  static rotateA(piece: PieceInterface, board: number[][]): PieceInterface {
    const newPiece = { ...piece, rotation: (piece.rotation + 1) % 4 };
    
    for (const offset of this.getKicks(piece, 'A')) {
      const testPiece = { 
        ...newPiece, 
        location: Location.translate(offset as [number, number])(piece.location)
      };
      
      if (Board.canAddPiece(board, testPiece)) {
        return testPiece;
      }
    }
    
    return piece;
  }

  static rotateB(piece: PieceInterface, board: number[][]): PieceInterface {
    const newPiece = { ...piece, rotation: (piece.rotation + 3) % 4 };
    
    for (const offset of this.getKicks(piece, 'B')) {
      const testPiece = { 
        ...newPiece, 
        location: Location.translate(offset as [number, number])(piece.location)
      };
      
      if (Board.canAddPiece(board, testPiece)) {
        return testPiece;
      }
    }
    
    return piece;
  }

  static moveLeft(piece: PieceInterface, board: number[][]): PieceInterface {
    const newPiece = { 
      ...piece, 
      location: Location.translate([-1, 0])(piece.location)
    };
    
    if (Board.canAddPiece(board, newPiece)) {
      return newPiece;
    }
    
    return piece;
  }

  static moveRight(piece: PieceInterface, board: number[][]): PieceInterface {
    const newPiece = { 
      ...piece, 
      location: Location.translate([1, 0])(piece.location)
    };
    
    if (Board.canAddPiece(board, newPiece)) {
      return newPiece;
    }
    
    return piece;
  }

  static moveDown(piece: PieceInterface, board: number[][]): PieceInterface {
    const newPiece = { 
      ...piece, 
      location: Location.translate([0, -1])(piece.location)
    };
    
    if (Board.canAddPiece(board, newPiece)) {
      return newPiece;
    }
    
    return piece;
  }

  static hardDrop(piece: PieceInterface, board: number[][]): PieceInterface {
    let currentPiece = piece;
    let nextPiece = this.moveDown(currentPiece, board);
    
    while (nextPiece !== currentPiece) {
      currentPiece = nextPiece;
      nextPiece = this.moveDown(currentPiece, board);
    }
    
    return currentPiece;
  }
}
