###
references
SRS rotation system: http://tetris.wikia.com/wiki/SRS
  http://web.archive.org/web/20081103193118/http://www.the-shell.net/img/i_piece.html
  http://harddrop.com/wiki/SRS
Piece spawn: http://harddrop.com/wiki/Spawn_Location
###
root = exports ? this
clone = (obj) ->
  if not obj? or typeof obj isnt 'object'
    return obj

  if obj instanceof Date
    return new Date(obj.getTime()) 

  if obj instanceof RegExp
    flags = ''
    flags += 'g' if obj.global?
    flags += 'i' if obj.ignoreCase?
    flags += 'm' if obj.multiline?
    flags += 'y' if obj.sticky?
    return new RegExp(obj.source, flags) 

  newInstance = new obj.constructor()

  for key of obj
    newInstance[key] = clone obj[key]

  return newInstance

Data = {
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
    I: 1
    J: 2
    L: 3
    O: 4
    S: 5
    T: 6
    Z: 7
  },
  SRS: {
    A: {
      I: [[0, -2, 1, 6, -15], 
          [0, -1, 2, 15, -6], 
          [0, 2, -1, 10, -9],
          [0, -2, 1, 6, -15]]
      J: [[0, -1, 7, -16, -17], 
          [0, 1, -7, 16, 17], 
          [0, 1, 9, -16, -15], 
          [0, -1, -9, 16, 15]]
      L: [[0, -1, 7, -16, -17], 
          [0, 1, -7, 16, 17], 
          [0, 1, 9, -16, -15], 
          [0, -1, -9, 16, 15]]
      O: [[0],
          [0], 
          [0], 
          [0]]
      S: [[0, -1, 7, -16, -17], 
          [0, 1, -7, 16, 17], 
          [0, 1, 9, -16, -15], 
          [0, -1, -9, 16, 15]]
      T: [[0, -1, 7, -16, -17], 
          [0, 1, -7, 16, 17], 
          [0, 1, 9, -16, -15], 
          [0, -1, -9, 16, 15]]
      Z: [[0, -1, 7, -16, -17], 
          [0, -1, -9, 16, 15], 
          [0, 1, 9, -16, -15], 
          [0, 1, -7, 16, 17]]
    }
    B: {
      I: [[0, 2, -1, 15, -6], 
          [0, 2, -1, 10, -17], 
          [0, -2, 1, 6, -7],
          [0, 1, -2, 17, -10]]
      J: [[0, 1, 9, -16, -15],
          [0, 1, -7, 16, 17],
          [0, -1, 7, -16, -17],
          [0, -1, -9, 16, 15]]
      L: [[0, 1, 9, -16, -15],
          [0, 1, -7, 16, 17],
          [0, -1, 7, -16, -17],
          [0, -1, -9, 16, 15]] 
      O: [[0],
          [0], 
          [0], 
          [0]]
      S: [[0, 1, 9, -16, -15],
          [0, 1, -7, 16, 17],
          [0, -1, 7, -16, -17],
          [0, -1, -9, 16, 15]] 
      T: [[0, 1, 9, -16, -15],
          [0, 1, -7, 16, 17],
          [0, -1, 7, -16, -17],
          [0, -1, -9, 16, 15]]
      Z: [[0, 1, 9, -16, -15],
          [0, 1, -7, 16, 17],
          [0, -1, 7, -16, -17],
          [0, -1, -9, 16, 15]]
    }
  },
  startingLocations: {
    I: (w, h) -> [w//2-.5, h-1.5],
    J: (w, h) -> [w//2, h-2],
    L: (w, h) -> [w//2, h-2],
    O: (w, h) -> [w//2-.5, h-1.5],
    S: (w, h) -> [w//2, h-2],
    T: (w, h) -> [w//2, h-2],
    Z: (w, h) -> [w//2, h-2],
  }
}

Board = {
  board: (w, h) -> [0..w-1].map -> [0..h-1].map -> 0
  empty: (board) ->
    board.every (r) -> r.every (el) -> el == 0
  count: (board) ->
    sum (board.map (r) -> sum (r.map (el) -> el > 0))
  get: (board, [x, y]) -> board[x][y]

  set: (board, [x, y], val) -> board[x][y] = val

  inBounds: (board, [x, y]) -> 0 <= x < board.length and 0 <= y < board[0].length

  open: (board, loc) -> Board.inBounds(board, loc) and Board.get(board, loc) == 0
  closed: (board, loc) -> Board.inBounds(board, loc) and Board.get(board, loc) > 0

  shiftDown: (board, y0) ->
    [w, h] = Board.dims(board)

    [y0..h-2].forEach (y) ->
      [0..w-1].forEach (x) ->
        board[x][y] = board[x][y + 1]
    [0..w-1].forEach (x) ->
      board[x][h-1] = 0

  encode: (board) -> 
    [w, h] = Board.dims(board)
    ret = ''
    [h-1..0].forEach (y) -> 
      [0..w-1].forEach (x) ->
        ret += board[x][y]
    ret

  decode: (str) -> JSON.parse(str)

  dims: (board) -> [board.length, board[0].length]

  copy: (board) -> board.map (col) -> col.map (el) -> el

  addPieceNoClear: (prvBoard, piece) ->
    board = Board.copy(prvBoard)
    Piece.blocks(piece).forEach ([x, y]) ->
      board[x][y] = PieceType.getIndex(piece.pieceType) # set to pieceType for color
    board

  canAddPiece: (board, piece) ->
    Piece.blocks(piece).every (loc) -> Board.open(board, loc)

  addPiece: (prvBoard, piece) ->
    board = Board.addPieceNoClear(prvBoard, piece)
    [w, h] = Board.dims(board)
    [0..h-1].forEach (y) ->
      while ([0..w-1].every (x) -> board[x][y])
        Board.shiftDown(board, y)
    board
}

PieceType = {
  blocks: (pieceType, rotation=0) ->
    Data.pieceLocs[pieceType].map(Location.rotate(rotation))

  getIndex: (pieceType) ->
    Data.pieceIndices[pieceType]
}

Location = {
  translate: ([offX, offY]) ->
    ([locX, locY]) -> [offX + locX, offY + locY]

  rotate: (rot) ->
    rot = ((rot % 4) + 4) % 4
    ([locX, locY]) ->
      if rot == 0
        [locX, locY]
      else if rot == 1
        [locY, -locX]
      else if rot == 2
        [-locX, -locY]
      else
        [-locY, locX]
}



kick = (transform, offset) -> (oldPiece) ->
  [offX, offY] = [offset % 8, (offset - (offset % 8)) / 8]
  if offX > 4
    offX -= 8
    offY += 1
  if offX < -4
    offX += 8
    offY -= 1
  piece = transform(oldPiece)
  piece.location = Location.translate([offX, offY])(piece.location)
  piece

rot = (turns) -> (oldPiece) ->
  piece = clone(oldPiece)
  piece.rotation = (piece.rotation + turns) % 4
  piece

mov = ([offX, offY]) -> (oldPiece) ->
  piece = clone(oldPiece)
  piece.location = Location.translate([offX, offY])(piece.location)
  piece

Piece = {
  create: (pieceType, board) ->
    [w, h] = Board.dims(board)
    {
      location: Data.startingLocations[pieceType](w, h)
      rotation: 0
      pieceType: pieceType     
    }

  blocks: (piece) ->
    PieceType.blocks(piece.pieceType, piece.rotation).map(Location.translate(piece.location))

  rotateA: (piece, board) ->
    r = rot(1)
    for offset in Data.SRS.A[piece.pieceType][piece.rotation]
      p = kick(r, offset)(piece)
      if Board.canAddPiece(board, p)
        return p
    return piece

  rotateB: (piece, board) ->
    r = rot(3)
    for offset in Data.SRS.B[piece.pieceType][piece.rotation]
      p = kick(r, offset)(piece)
      if Board.canAddPiece(board, p)
        return p
    return piece

  moveLeft: (piece, board) ->
    m = mov([-1, 0])
    p = m(piece)
    if Board.canAddPiece(board, p)
      p
    else
      piece

  moveRight: (piece, board) ->
    m = mov([1, 0])
    p = m(piece)
    if Board.canAddPiece(board, p)
      p
    else
      piece

  moveDown: (piece, board) ->
    m = mov([0, -1])
    p = m(piece)
    if Board.canAddPiece(board, p)
      p
    else
      piece    

  hardDrop: (piece, board) ->
    m = mov([0, -1])
    p = piece
    while Board.canAddPiece(board, m(p))
      p = m(p)
    p

  sym: (piece) ->
    t = piece.pieceType
    r = piece.rotation
    if t == 'I' or t == 'S' or t == 'Z'
      if r == 2
        mov([0, -1])(rot(2)(piece))
      else if r == 3
        mov([-1, 0])(rot(2)(piece))
      else
        piece
    else if t == 'O'
      rot((4 - r)%4)(piece)
    else
      piece

  sameBlocks: (pieceA, pieceB) ->
    a = Piece.sym(pieceA)
    b = Piece.sym(pieceB)
    a.rotation == b.rotation and a.location[0] == b.location[0] and a.location[1] == b.location[1]

}

root.PieceType = PieceType
root.Piece = Piece
root.Board = Board