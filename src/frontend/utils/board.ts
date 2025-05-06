export interface BoardType {
  W: string;
  H: string;
  data: string;
}

export class Board {
  static create(w: number, h: number): number[][] {
    return Array(w).fill(0).map(() => Array(h).fill(0));
  }

  static reset(board: number[][]): void {
    board.forEach((column, x) => {
      column.forEach((_, y) => {
        board[x][y] = 0;
      });
    });
  }

  static isEmpty(board: number[][]): boolean {
    return board.every(column => column.every(cell => cell === 0));
  }

  static count(board: number[][]): number {
    return board.reduce((sum, column) => 
      sum + column.reduce((colSum, cell) => colSum + (cell > 0 ? 1 : 0), 0), 0);
  }

  static get(board: number[][], [x, y]: [number, number]): number {
    return board[x][y];
  }

  static set(board: number[][], [x, y]: [number, number], value: number): void {
    board[x][y] = value;
  }

  static inBounds(board: number[][], [x, y]: [number, number]): boolean {
    return x >= 0 && x < board.length && y >= 0 && y < board[0].length;
  }

  static isOpen(board: number[][], loc: [number, number]): boolean {
    return this.inBounds(board, loc) && this.get(board, loc) === 0;
  }

  static isClosed(board: number[][], loc: [number, number]): boolean {
    return this.inBounds(board, loc) && this.get(board, loc) > 0;
  }

  static shiftDown(board: number[][], y0: number): void {
    const [w, h] = this.dims(board);
    
    for (let y = y0; y < h - 1; y++) {
      for (let x = 0; x < w; x++) {
        board[x][y] = board[x][y + 1];
      }
    }
    
    for (let x = 0; x < w; x++) {
      board[x][h - 1] = 0;
    }
  }

  static encode(board: number[][]): string {
    const [w, h] = this.dims(board);
    let result = '';
    
    for (let y = h - 1; y >= 0; y--) {
      for (let x = 0; x < w; x++) {
        result += board[x][y].toString();
      }
    }
    
    return result;
  }

  static decode(str: string): any {
    return JSON.parse(str);
  }

  static dims(board: number[][]): [number, number] {
    return [board.length, board[0].length];
  }

  static copy(board: number[][]): number[][] {
    return board.map(column => [...column]);
  }

  static addPieceNoClear(prvBoard: number[][], piece: any): number[][] {
    const board = this.copy(prvBoard);
    const blocks = piece.blocks();
    
    blocks.forEach(([x, y]: [number, number]) => {
      board[x][y] = piece.pieceType; // Set to pieceType for color
    });
    
    return board;
  }

  static canAddPiece(board: number[][], piece: any): boolean {
    return piece.blocks().every((loc: [number, number]) => this.isOpen(board, loc));
  }

  static addPiece(prvBoard: number[][], piece: any): number[][] {
    const board = this.addPieceNoClear(prvBoard, piece);
    const [w, h] = this.dims(board);
    
    for (let y = 0; y < h; y++) {
      while ([...Array(w).keys()].every(x => board[x][y])) {
        this.shiftDown(board, y);
      }
    }
    
    return board;
  }
}
