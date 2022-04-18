from __future__ import print_function
import time
from outputvideo import TwitchBufferedOutputStream
import argparse
import random
import numpy as np
import requests
import json
from PIL import Image, ImageDraw, ImageColor

global_frame_num = 1

def shuffled(lst):
    lst = list(lst[:])
    random.shuffle(lst)
    return lst

def make_empty_board(width, height):
    return [[None] * width for i in range(height)]

def make_bag():
    return shuffled(range(7))

def to_coord(i, j, img_width, img_height, board_width, board_height):
    size = 40
    pad_left = (img_width - board_width * size) / 2
    pad_top = (img_height - board_height * size) / 2
    return (
            pad_left + j * size,
            pad_top + i * size
        )

def get_color(t, active=False):
  return ['#00ffff', '#0000ff', '#ffa500', '#ffff00', '#00ff00', '#800080', '#ff0000'][t]

background_color = '#666666'

piece_names = ['I', 'J', 'L', 'O', 'S', 'T', 'Z']

class Piece:
    def __init__(self, t, s):
        self.t = t
        self.blocks = []
        rows = s.split('\n')
        for i in range(len(rows)):
            for j in range(len(rows[i])):
                if rows[i][j] == '+':
                    self.blocks.append((i, j))


class Game:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.board = make_empty_board(width, height)
        self.bag = []
        self.queue = []
        # queue: [cur, hold, <6 previews>]
        for i in range(8):
            self.push_to_queue()

    def push_to_queue(self):
        if not len(self.bag):
            self.bag = make_bag()
        self.queue.append(self.bag.pop())

    def add_piece(self, piece):
        idx = 1 if self.queue[0] == piece.t else 0
        self.queue = [self.queue[idx]] + self.queue[2:]
        self.push_to_queue()

        for (i, j) in piece.blocks:
            self.board[i][j] = piece.t
        for i in range(self.height):
            if all(x != None for x in self.board[i]):
                self.board[i: i + 1] = []
                self.board = [[None] * self.width] + self.board

    def to_text(self, piece):
        text = ''
        blocks = piece.blocks if piece else []
        for i in range(self.height):
            for j in range(self.width):
                if self.board[i][j] != None:
                    text += str(self.board[i][j])
                elif (i, j) in blocks:
                    text += '+'
                else:
                    text += '.'
            text += '\n'
        return text

    def to_image(self, piece, width, height):
        global global_frame_num
        text = self.to_text(piece)
        blocks = piece.blocks if piece else []
        print(text)
        image = Image.new('RGB', (width, height))
        draw = ImageDraw.Draw(image)
        tc = lambda i, j: to_coord(i, j, width, height, self.width, self.height)
        board_boundary = [tc(0, 0), tc(self.height, self.width)]
        print(board_boundary, background_color)
        draw.rectangle(board_boundary, background_color)
        for i in range(self.height):
            for j in range(self.width):
                xy = [tc(i, j), tc(i + 1, j + 1)]
                if self.board[i][j] != None:
                    color = get_color(self.board[i][j])
                    draw.rectangle(xy, color)
                elif (i, j) in blocks:
                    color = get_color(piece.t, True)
                    print(xy, color)
                    draw.rectangle(xy, color)
        del draw
        return image
        #  d = global_frame_num
        #  global_frame_num += 1

        #  path = 'tmpimg%s.png' % d
        #  image.save(open(path, 'w'))
        #  image = Image.open(open(path, 'r'))
        #  image.show()

    def to_json(self):
        data = ''
        for row in self.board:
            data += ''.join([str(t + 1 if t != None else 0) for t in row])
        print(self.width, self.height, data)
        return {
            'board': {
                'W': str(self.width),
                'H': str(self.height),
                'data': data,
                },
            'pieces': [
                piece_names[i]
                for i in self.queue
                ]
            }

class Stream:
    def __init__(self, width, height, video_fps, fps):
        self.game = Game(10, 10)
        self.width = width
        self.height = height
        self.path = None
        self.path_idx = 0
        self.video_fps = video_fps
        self.fps = fps
        self.last_frame = None
        self.counter = 0

    def get_path(self):
        data = self.game.to_json()
        print('POST', data)
        r = requests.post('http://127.0.0.1:4444/ai', json = data)
        result = r.json()
        path_strings = result['pathStrings']
        idx = 1 if result['path'][0] == 5 else 0
        t = self.game.queue[idx]
        self.path = [Piece(t, s) for s in path_strings]
        self.path_idx = 0
        print(self.path)

    def get_frame(self):
        if not(self.last_frame is None) and self.counter < self.video_fps / self.fps:
            self.counter += 1
            return self.last_frame
        if not self.path:
            self.get_path()
        if self.path_idx == len(self.path):
            self.game.add_piece(self.path[-1])
            result = self.game.to_image(None, self.width, self.height)
            self.path = None
            #  TODO: async-ly self.get_path()
        else:
            piece = self.path[self.path_idx]
            result = self.game.to_image(piece, self.width, self.height)
            self.path_idx += 1

        #  print(np.random.rand(self.height, self.width, 3))
        result = np.asarray(result) / 256.
        self.last_frame = result
        self.counter = 0
        return result
        #  return np.random.rand(self.height, self.width, 3)



def main():
    parser = argparse.ArgumentParser(description=__doc__)
    required = parser.add_argument_group('required arguments')
    required.add_argument('-s', '--streamkey',
            help='twitch streamkey',
            required=True)
    args = parser.parse_args()


    width = 480
    height = 640

    video_fps = 15
    fps = 5
    stream = Stream(width, height, video_fps, fps)
    with TwitchBufferedOutputStream(
            twitch_stream_key=args.streamkey,
            width=width,
            height=height,
            fps=video_fps,
            crf=29,
            verbose=True) as videostream:
        while True:
            if videostream.get_video_frame_buffer_state() < 1000:
                videostream.send_video_frame(stream.get_frame())
            else:
                time.sleep(0.001)

if __name__ == "__main__":
    main()
