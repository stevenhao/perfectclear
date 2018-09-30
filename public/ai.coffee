pieces = ['I', 'J', 'L', 'O', 'S', 'T', 'Z']
getColor = (x) ->
  ['black', 'cyan', 'blue', 'orange', 'yellow', 'lime', 'purple', 'red'][x];

ghostColor = 'dimgray'
getDims = ($well) ->
  [parseInt($well.attr('w')), parseInt($well.attr('h'))]

setColor = ($well, x, y, color) ->
  [w, h] = getDims($well)

  if y < h
    $el = $($well.children()[x * h + y])
    $el.css('background-color', color)

$ ->
  $board = $('#game-inner')
  $preview = $('#previews')
  $select = $('#piece-inner')
  $counter = $('#counter')
  do makeBoard = ->
    $board.empty()
    [w, h] = getDims($board)
    cellsize = min [$board.height() / h, $board.width() / w]
    [0..w-1].forEach (x) ->
      [0..h-1].forEach (y) ->
        $el = $('<div>').attr('x', x).attr('y', y)
        $el.appendTo($board)
        $el.addClass('block').css({
          width: cellsize - 1
          height: cellsize - 1
          position: 'absolute'
          top: (h-1-y)*cellsize 
          left: x*cellsize
        })

  renderBoard = (board) ->
    [w, h] = getDims($board)
    $board = $('#game-inner')
    [0..w-1].forEach (x) ->
      [0..h-1].forEach (y) ->
        color = getColor(Board.get(board, [x, y]))
        setColor($board, x, y, color)

  renderBoardWithPiece = (board, piece) -> 
    renderBoard(board)
    ghostPiece = Piece.hardDrop(piece, board)
    Piece.blocks(ghostPiece).forEach ([x, y]) ->
      setColor($board, x, y, ghostColor)
    Piece.blocks(piece).forEach ([x, y]) ->
      color = getColor(PieceType.getIndex(piece.pieceType))
      setColor($board, x, y, color)

  renderPreview = (preview) ->
    [w, h] = [4, 20]
    cellsize = min [$preview.height() / 20, $preview.width() / 5]
    mid = $preview.width() / 2
    $preview.empty()
    [0..preview.length - 1].forEach (i) ->
      pieceType = preview[i]
      color = getColor(PieceType.getIndex(pieceType))
      PieceType.blocks(pieceType).forEach ([x, y]) ->
        $el = $('<div>').attr('x', x).attr('y', y)
        cx = mid
        cy = (2.5 + 4 * i) * cellsize
        left = cx + x * cellsize - cellsize / 2
        top = cy - y * cellsize - cellsize / 2
        $el.addClass('block').addClass('preview').css({
          width: cellsize - 1
          height: cellsize - 1
          position: 'absolute'
          backgroundColor: color
          left: left
          top: top
        })
        $el.appendTo($preview)

  animateMove = (board, move, path, animationTime, callback) ->    
    now = Date.now()
    time = 0
    if path != null and animationTime > 0
      print 'animating'
      path.forEach (piece) ->
        print {piece, time}
        color = getColor(PieceType.getIndex(piece.pieceType))
        ntime = now + time - Date.now()
        print 'scheduled for ', ntime
        setTimeout ->
          renderBoardWithPiece(board, piece)
        , ntime
        time += animationTime / (path.length + 1)
      nextBoard = Board.addPiece(board, move)
    else
      print 'not animating'
      renderBoard(board)
      ghostPiece = Piece.hardDrop(move, board)
      Piece.blocks(ghostPiece).forEach ([x, y]) ->
        setColor($board, x, y, ghostColor)
    if callback
      setTimeout ->
        callback()
      , animationTime


  randomPieceType = -> pieces[rand(pieces.length)]
  do ->
    board = do ->
      [w, h] = getDims($board)
      Board.board(w, h)

    preview = [0..5].map -> randomPieceType()
    nextPiece = ->
      preview.push(randomPieceType())
      ret = Piece.create(preview.shift(), board)
      ret
    curPiece = nextPiece()

    renderBoardWithPiece(board, curPiece)

    $('#clear').click -> 
      [w, h] = getDims($board)
      board = Board.board(w, h + 2)
      renderBoard(board)

    aiMove = (wantPath, callback) ->
      pieces = [curPiece.pieceType].concat(preview)
      [w, h] = getDims($board)
      $.post "/ai", { board: {W: w, H: h, data: Board.encode(board)}, pieces: pieces }, callback

    playNextPiece = (animationTime) ->
      prvBoard = board
      ctime = Date.now()
      print curPiece
      aiMove animationTime > 0, (data) ->
        print {data} # should be a path
        if data.path?
          # todo: fix
          board = Board.addPiece(board, data.move)
          ppreview = clone(preview)
          curPiece = nextPiece()

          nboard = board
          npiece = curPiece
          npreview = clone(preview)

          animateMove prvBoard, data.move, data.path, animationTime, ->
            if animationTime > 0
              renderBoardWithPiece(nboard, npiece)
              renderPreview(npreview)
            else
              renderPreview(ppreview)
        else
          print('no move returned')

    $('#piece-send').click -> playNextPiece(1000)


