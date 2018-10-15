from __future__ import print_function
from outputvideo import TwitchBufferedOutputStream
import argparse
import random
import numpy as np
import requests
import json

def shuffled(lst):
    lst = lst[:]
    random.shuffle(lst)
    return lst

def make_empty_board(width, height):
    return [[0] * width for i in range(height)]

def make_bag():
    return shuffled(range(7))

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
            if all(x > 0 for x in self.board[i]):
                self.board[i: i + 1] = []
                self.board.append([0] * width)

    def to_text(self, piece):
        text = ''
        blocks = piece.blocks if piece else []
        for i in range(self.height):
            for j in range(self.width):
                if self.board[i][j] > 0:
                    text += str(self.board[i][j])
                elif (i, j) in blocks:
                    text += '+'
                else:
                    text += '.'
            text += '\n'
        return text

    def to_image(self, piece, width, height):
        text = self.to_text(piece)
        print(text)
        return np.random.rand(height, width, 3)

    def to_json(self):
        data = ''
        for row in self.board:
            data += ''.join(map(str, row))
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
    def __init__(self, width, height):
        self.game = Game(10, 10)
        self.width = width
        self.height = height
        self.path = None
        self.path_idx = 0

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
        return result



def main():
    parser = argparse.ArgumentParser(description=__doc__)
    required = parser.add_argument_group('required arguments')
    required.add_argument('-s', '--streamkey',
            help='twitch streamkey',
            required=True)
    args = parser.parse_args()


    width = 640
    height = 480
    stream = Stream(width, height)
    with TwitchBufferedOutputStream(
            twitch_stream_key=args.streamkey,
            width=width,
            height=height,
            fps=1.,
            verbose=True) as videostream:
        while True:
            if videostream.get_video_frame_buffer_state() < 5:
                frame = stream.get_frame()
                videostream.send_video_frame(frame)

if __name__ == "__main__":
    main()
