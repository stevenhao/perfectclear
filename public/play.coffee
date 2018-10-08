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
  $hold = $('#hold')
  do makeBoard = ->
    $board.empty()
    [w, h] = getDims($board)
    cellsize = min [$board.height() / h, $board.width() / w]
    [0..w-1].forEach (x) ->
      [0..h-1].forEach (y) ->
        $el = $('<div>').attr('x', x).attr('y', y)
        $el.addClass('block').css({
          width: cellsize - 1
          height: cellsize - 1
          position: 'absolute'
          top: (h-1-y)*cellsize
          left: x*cellsize
        })
        $el.appendTo($board)

  renderBoard = (board) ->
    [w, h] = getDims($board)
    $board = $('#game-inner')
    [0..w-1].forEach (x) ->
      [0..h-1].forEach (y) ->
        color = getColor(Board.get(board, [x, y]))
        setColor($board, x, y, color)

  renderCounter = (counter) ->
    print 'counter', counter.clears, counter.pieces
    $counter = $('#counter')
    $counter.text(counter.clears + '/' + counter.pieces)

  renderBoardWithPiece = (board, piece) -> 
    print 'rendering', piece
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

  renderHold = (pieceType) ->
    [w, h] = [4, 3]
    cellsize = min [$hold.height() / 3, $hold.width() / 4]
    mid = $hold.width() / 2
    $hold.empty()
    if pieceType
      color = getColor(PieceType.getIndex(pieceType))
      PieceType.blocks(pieceType).forEach ([x, y]) ->
        $el = $('<div>').attr('x', x).attr('y', y)
        cx = mid
        cy = 2.5 * cellsize
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
        $el.appendTo($hold)

  animateMove = (board, move) ->
    nxtBoard = Board.applyMove(board, move)
    renderBoard(nxtBoard)
    nxtBoard

  randomPieceType = -> 
    print 'random piece, ', rand(7), pieces.length
    print pieces
    pieces[rand(7)]


  makeBag = ->
    bag = pieces.slice()
    [0..pieces.length-1].forEach (i) ->
      j = rand(i + 1)
      tmp = bag[j]
      bag[j] = bag[i]
      bag[i] = tmp
    bag
  do ->
    board = do ->
      [w, h] = getDims($board)
      Board.board(w, h)
    bag = makeBag()
    nextBag = makeBag()
    preview = bag.slice(0, 6)
    hold = null

    swapHold = (curPiece, board) ->
      console.log('swaphold', curPiece, board)
      cType = curPiece.pieceType
      if hold
        curPiece = Piece.create(hold, board)
      else
        curPiece = nextPiece()
      hold = cType
      console.log(curPiece)
      renderBoardWithPiece(board, curPiece)
      renderHold(hold)
      renderPreview(preview)
      curPiece

    commands = {
      ArrowLeft: Piece.moveLeft,
      ArrowRight: Piece.moveRight,
      ArrowDown: Piece.hardDrop,
      ArrowUp: Piece.rotateA,
      KeyX: Piece.rotateA,
      KeyZ: Piece.rotateB
      ShiftLeft: swapHold,
      ShiftRight: swapHold,
    }

    counter = {
      pieces: 0,
      clears: 0,
    }

    AIcommands = [Piece.rotateA, Piece.rotateB, Piece.moveLeft, Piece.moveRight, Piece.hardDrop, swapHold]

    aiMove = (callback) ->
      p = [curPiece.pieceType].concat(preview)
      if hold
        p = [curPiece.pieceType, hold].concat(preview)
      [w, h] = getDims($board)
      $.post "/ai", { board: {W: w, H: h, data: Board.encode(board)}, pieces: p }, callback

    nextPiece = ->
      ret = Piece.create(bag.shift(), board)
      if bag.length == 0
        bag = nextBag
        nextBag = makeBag()
      preview = bag.concat(nextBag).slice(0, 6)
      ret

    curPiece = nextPiece()

    prevStates = []
    nextStates = []
    addHistory = (state) ->
      prevStates.push(state)
      if prevStates.length > 40
        prevStates = prevStates.slice(20)

    curState = () ->
      ({
        board: board,
        curPiece: curPiece,
        hold: hold,
        bag: bag.slice(),
        nextBag: nextBag.slice(),
        preview: preview,
      })

    save = () ->
      addHistory(curState())
      nextStates = []

    undo = () ->
      if !prevStates.length then return
      nextStates.push(curState())

      state = prevStates.pop()
      board = state.board
      curPiece = state.curPiece
      hold = state.hold
      bag = state.bag
      nextBag = state.nextBag
      preview = state.preview
      renderHold(hold)
      renderBoardWithPiece(board, curPiece)
      renderPreview(preview)

    redo = () ->
      if !nextStates.length then return
      prevStates.push(curState())
      state = nextStates.pop()
      board = state.board
      curPiece = state.curPiece
      hold = state.hold
      bag = state.bag
      nextBag = state.nextBag
      preview = state.preview
      renderHold(hold)
      renderBoardWithPiece(board, curPiece)
      renderPreview(preview)

    $('#undo').click(undo)
    $('#redo').click(redo)

    movecount = 0
    renderBoardWithPiece(board, curPiece)
    renderPreview(preview)
    $('button').on 'focus', (e) ->
      this.blur()

    $('body').keydown (evt) ->
      code = evt.originalEvent.code
      console.log(evt, code)

      if code == 'Minus'
        undo()
      else if code == 'Equal'
        redo()
      else if code == 'Space'
        save()
        if !Board.canAddPiece(board, curPiece)
          Board.reset(board)
          bag = nextBag
          nextBag = makeBag()
          hold = null
          curPiece = nextPiece()
          renderHold(hold)
          renderBoardWithPiece(board, curPiece)
          renderPreview(preview)
        else
          curPiece = Piece.hardDrop(curPiece, board)
          board = Board.addPiece(board, curPiece)
          curPiece = nextPiece()
          renderBoardWithPiece(board, curPiece)
          renderPreview(preview)
          counter.pieces += 1
          if Board.isEmpty(board)
            counter.clears += 1
          renderCounter(counter)
        false
      else if !Board.canAddPiece(board, curPiece)
        false
      else if commands[code]?
        fn = commands[code]
        console.log('before', curPiece)
        curPiece = fn(curPiece, board)
        console.log('after', curPiece)
        renderBoardWithPiece(board, curPiece)
        false
      else
        print 'cannot do', code
        true

    stopWhenHappy = false
    $('#stop-controller').on('change', (e) ->
      stopWhenHappy = !!e.target.checked
    )
    autoplaying = false
    autoplayStep = () ->
      aiMove (data) ->
        if $('#happy-indicator').text() != '🐻'
          $('#happy-indicator').text('🤓')
        if data.happy > 0
          $('#happy-indicator').text('🐻')
          if (stopWhenHappy)
            $('#autoplay').click()
            return
        else
          $('#happy-indicator').text('😐')
          if data.happy < 0
            $('#happy-indicator').text('😵')
        print data.path
        # animate 500 ms
        path = data.path
        animationTime = 500
        save()
        interval = animationTime / (path.length + 1)
        time = 0
        curPiece = Piece.create(curPiece.pieceType, board)
        path.forEach (mv) ->
          curPiece = AIcommands[mv](curPiece, board)
          print 'drawing ', draw, 'at ', time
          draw = curPiece
          time += interval
          setTimeout ->
            renderBoardWithPiece(board, draw)
          , time
        setTimeout ->
          renderBoardWithPiece(board, curPiece)
        , animationTime
        setTimeout ->
          if !Board.canAddPiece(board, curPiece)
            Board.reset(board)
            bag = nextBag
            nextBag = makeBag()
            hold = null
            curPiece = nextPiece()
            renderHold(hold)
            renderBoardWithPiece(board, curPiece)
            renderPreview(preview)
          else
            curPiece = Piece.hardDrop(curPiece, board)
            board = Board.addPiece(board, curPiece)
            curPiece = nextPiece()
            renderBoardWithPiece(board, curPiece)
            renderPreview(preview)
            counter.pieces += 1
            if Board.isEmpty(board)
              counter.clears += 1
            renderCounter(counter)
          false
        , animationTime + 500
        if autoplaying
          setTimeout(autoplayStep, animationTime + 1000)

    $('#autoplay').click (evt) ->
      if autoplaying
        $('#autoplay').text('Autoplay')
        autoplaying = false
      else
        $('#autoplay').text('Pause')
        autoplaying = true
        autoplayStep()

    $('#piece-send').click (evt) ->
      aiMove (data) ->
        print data.path
        # animate 1000 ms
        path = data.path
        animationTime = 1000
        interval = animationTime / (path.length + 1)
        time = 0
        curPiece = Piece.create(curPiece.pieceType, board)
        path.forEach (mv) ->
          curPiece = AIcommands[mv](curPiece, board)
          print 'drawing ', draw, 'at ', time
          draw = curPiece
          time += interval
          setTimeout ->
            renderBoardWithPiece(board, draw)
          , time
        setTimeout ->
          renderBoardWithPiece(board, curPiece)
        , animationTime



