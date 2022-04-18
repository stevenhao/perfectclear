from typing import List, Union
from PIL import Image, ImageDraw, ImageColor, ImageFont
from piece import Piece, get_color

pieces_blocks = open('pieces', 'r').read().split('\n\n')
loaded_pieces = [Piece(i, '\n'.join(pb.split('\n')[1:])) for i, pb in enumerate(pieces_blocks)]
def get_piece(pt):
    res = loaded_pieces[pt]
    mx = min(x for y, x in res.blocks)
    my = min(y for y, x in res.blocks)
    res.blocks = [(y - my, x - mx) for y, x in res.blocks]
    return res
    

def draw_game(
    hold: int, # = stream.game.queue[0]
    cur: int, # = stream.game.queue[1]
    orig_cur_piece: Union[Piece, None], # = stream.path[stream.path_idx]
    preview: List[int], # = stream.game.queue[2:]
    orig_board: List[List[Union[int, None]]], # = stream.game.board
    img_width,
    img_height
):
    board = [[None] * 10] * 10 + orig_board
    cur_piece_blocks = list(orig_cur_piece.blocks) if orig_cur_piece is not None else []
    if all(y > 1 for y, x in cur_piece_blocks):
        cur_piece_blocks = [(y + 10, x) for y, x in cur_piece_blocks]
    image = Image.new('RGB', (img_width, img_height))
    draw = ImageDraw.Draw(image)
    def rect(r, color):
        ((x1, y1), (x2, y2)) = r
        return draw.rectangle(((x1, y1), (x2 - 1, y2 - 1)), color)
        
    game_width = len(board[0])
    game_height = len(board)

    c1 = 1 # board size
    c2 = 0.7 # preview & hold cell size
    c3 = 0.2 # margin between panels
    c4 = 0.8 # text height
    c5 = 2.0 # padding around main image
    w = game_width * c1 + 10 * c2 + 4 * c3 + 2 * c5
    h = game_height * c1 + 2 * c3 + 2 * c5
    size = round(min(img_width / w, img_height / h))
    print('drawing size', size)
    c1 *= size
    c2 *= size
    c3 *= size
    c4 *= size
    c5 *= size
    w *= size
    h *= size

    black_color = (0, 0, 0)
    panel_color = (114, 229, 194)
    line_color = (41, 41, 41)

    tx = (img_width - w) / 2
    ty = (img_height - h) / 2
    bx = tx + 5 * c2 + 2 * c3 + c5
    by = ty + c3 + c5
    gw = game_width * c1
    gh = game_height * c1

    board_padded = ((bx - c3, by - c3), (bx + gw + c3, by + gh + c3))
    board_inside = ((bx, by), (bx + gw, by + gh))
    rect(board_padded, panel_color)
    rect(board_inside, black_color)

    hold_padded = ((bx - 2 * c3 - 5 * c2, by - c3), (bx, by + 4 * c2 + c4))
    hold_inside = ((bx - c3 - 5 * c2, by - c3 + c4), (bx - c3, by - c3 + c4 + 4 * c2))

    rect(hold_padded, panel_color)
    rect(hold_inside, black_color)

    preview_padded = ((bx + gw, by - c3), (bx + gw + 2 * c3 + 5 * c2, by + 6 * 3 * c2 + c4))
    preview_inside = ((bx + gw + c3, by + c4 - c3), (bx + gw + c3 + 5 * c2, by + 6 * 3 * c2 + c4 - c3))

    rect(preview_padded, panel_color)
    rect(preview_inside, black_color)

    # fnt = ImageFont.truetype("Pillow/Tests/fonts/FreeMono.ttf", 40)
    preview_text = 'NEXT'
    preview_text_size = draw.textsize(preview_text)
    draw.text((bx + gw + c3 + 2.5 * c2 - 0.5 * preview_text_size[0], by - c3 + 0.5 * c4 - 0.5 * preview_text_size[1]), preview_text, fill=(0, 0, 0), anchor="mm", color=line_color)

    hold_text = 'HOLD'
    hold_text_size = draw.textsize(hold_text)
    draw.text((bx - c3 - 5 * c2 + 2.5 * c2 - 0.5 * hold_text_size[0], by - c3 + 0.5 * c4 - 0.5 * hold_text_size[1]), hold_text, fill=(0, 0, 0), anchor="mm", color=line_color)

    def draw_piece(box, piece_type, c):
        color = get_color(piece_type)
        piece = get_piece(piece_type)
        # center in box
        pw = max([x for y, x in piece.blocks]) + 1
        ph = max([y for y, x in piece.blocks]) + 1
        (bx1, by1), (bx2, by2) = box
        tx = bx1 + ((bx2 - bx1) - pw * c2) / 2
        ty = by1 + ((by2 - by1) - ph * c2) / 2
        print(tx, ty, pw, ph)
        for y, x in piece.blocks:
            rect(((tx + x * c2, ty + y * c2), (tx + (x + 1) * c2, ty + (y + 1) * c2)), color)
        
    draw_piece(hold_inside, hold, c2)
    for i, p in enumerate(preview):
        preview_slot = ((bx + gw + c3, by + c4 + i * 3 * c2), (bx + gw + c3 + 5 * c2, by + (i + 1) * 3 * c2 + c4 - c3))
        draw_piece(preview_slot, p, c2)
        


    # draw board
    for i in range(game_height):
        for j in range(game_width):
            if board[i][j] != None:
                color = get_color(board[i][j])
            elif (i, j) in cur_piece_blocks:
                color = get_color(orig_cur_piece.t, True)
                # print(xy, color)
            else:
                continue
            rect(((bx + j * c1, by + i * c1), (bx + (j + 1) * c1, by + (i + 1) * c1)), color)    
    return image