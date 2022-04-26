from __future__ import print_function
import time
from draw import draw_game, draw_snapshot
from outputvideo import TwitchBufferedOutputStream
import argparse
import random
import numpy as np
import requests
import json
from PIL import Image, ImageDraw, ImageColor

from piece import Piece, get_color

global_frame_num = 1
PREVIEW_SIZE = 6
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

background_color = '#666666'

piece_names = ['I', 'J', 'L', 'O', 'S', 'T', 'Z']

def clone_deep(obj):
    return [clone_deep(x) for x in obj] if isinstance(obj, list) else obj

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

    def to_image2(self, piece, img_width, img_height):
        hold = self.queue[0]
        cur = None
        if piece is None:
            preview = self.queue[1:-1]
        else:
            cur = self.queue[1]
            preview = self.queue[2:]
            if hold == piece.t:
                hold, cur = cur, hold
        board = self.board
        return draw_game(hold, piece.blocks if piece is not None else [], piece.t if piece is not None else None, preview, board, img_width, img_height)

    def snapshot(self, piece):
        hold = self.queue[0]
        cur = None
        if piece is None:
            preview = self.queue[1:-1]
        else:
            cur = self.queue[1]
            preview = self.queue[2:]
            if hold == piece.t:
                hold, cur = cur, hold
        board = self.board
        return (hold, piece.blocks if piece is not None else [], piece.t if piece is not None else None, clone_deep(preview), clone_deep(board))
        
    def to_json(self):
        data = ''
        for row in self.board:
            data += ''.join([str(t + 1 if t != None else 0) for t in row])
        # print(self.width, self.height, data)
        return {
            'board': {
                'W': str(self.width),
                'H': str(self.height),
                'data': data,
                },
            'pieces': [
                piece_names[i]
                for i in self.queue[:PREVIEW_SIZE + 2]
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
        self.snapshots = []

    def get_path(self, search_breadth):
        data = self.game.to_json()
        data['search_breadth'] = search_breadth
        # print('POST', data)
        r = requests.post('http://127.0.0.1:4444/ai', json = data)
        result = r.json()
        path_strings = result['pathStrings']
        idx = 1 if result['path'][0] == 5 else 0
        t = self.game.queue[idx]
        self.path = [Piece(t, s) for s in path_strings]
        self.path_idx = 0
        # print(self.path)

    def get_snapshot(self, search_breadth=300):
        if not self.path:
            result = self.game.snapshot(None)
            self.get_path(search_breadth) # for next round
        elif self.path_idx == len(self.path):
            self.game.add_piece(self.path[-1])
            result = self.game.snapshot(None)
            self.path = None
        else:
            piece = self.path[self.path_idx]
            self.path_idx += 1
            result = self.game.snapshot(piece)
        return result

    def get_frame(self):
        if not(self.last_frame is None) and self.counter < self.video_fps / self.fps:
            self.counter += 1
            return self.last_frame
        if len(self.snapshots) == 0:
            self.prebuffer()
        snapshot = self.snapshots[0]
        self.snapshots = self.snapshots[1:]
        frame_image = draw_snapshot(snapshot, self.width, self.height)
        self.last_frame = np.asarray(frame_image) / 256
        self.counter = 0
        return self.last_frame
    
    def prebuffer(self):
        MAX_SNAPSHOT_BUFFER = 20000
        if len(self.snapshots) >= MAX_SNAPSHOT_BUFFER:
            self.buffering = False
            return
        if len(self.snapshots) >= MAX_SNAPSHOT_BUFFER / 2 and not self.buffering:
            # print('Waiting...', len(self.snapshots))
            return
        print('Buffering...', len(self.snapshots))
        self.buffering = True
        search_breadth = 25 if len(self.snapshots) < 100 else 50
        self.snapshots.append(self.get_snapshot(search_breadth))

#         ..........                                                                                                                     │    464 root       20   0  235M  4220  3080 S  0.0  0.1  0:00.68 /usr/lib/accountsservice/accounts-daemon
# ....++++..                                                                                                                     │    844 root       20   0 1278M  9312  2824 S  0.0  0.2  0:00.10 /snap/amazon-ssm-agent/5163/amazon-ssm-agent
# {"board":{"H":"10","W":"10","data":"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000│    298 root       RT   0  273M 18000  8208 S  0.0  0.4  0:00.08 /sbin/multipathd -d -s
# 000111100"},"pieces":["Z","T","O","L","J","Z","S","T"],"reqid":3137,"search_breadth":500}                                      │    482 root       20   0  852M 25136  8040 S  0.0  0.6  0:14.74 /usr/lib/snapd/snapd
# Beam search limit is now 500                                                                                                   │   5145 root       20   0 1278M  9312  2824 S  0.0  0.2  0:00.17 /snap/amazon-ssm-agent/5163/amazon-ssm-agent
# global cache hit rate 0.433259                                                                                                 │    842 root       20   0 1290M 16412  6064 S  0.0  0.4  0:06.24 /snap/amazon-ssm-agent/5163/ssm-agent-worker
# hardQueries: 1872, cacheMisses: 3242827                                                                                        │    458 root       20   0  235M  4220  3080 S  0.0  0.1  0:00.73 /usr/lib/accountsservice/accounts-daemon
# wins: 316, totalQueries: 3137; ratio: 0.100733                                                                                 │    648 postgres   20   0  212M 17100 15068 S  0.0  0.4  0:00.22 /usr/lib/postgresql/12/bin/postgres -D /var/lib/postgresql/12
# BAD                                                       
# 
# Beam search limit is now 200                                                                                                   │    298 root       RT   0  273M 18000  8208 S  0.0  0.4  0:00.08 /sbin/multipathd -d -s
# global cache hit rate 0.140264                                                                                                 │    482 root       20   0  852M 25136  8040 S  0.0  0.6  0:14.74 /usr/lib/snapd/snapd
# hardQueries: 31, cacheMisses: 48018                                                                                            │   5145 root       20   0 1278M  9312  2824 S  0.0  0.2  0:00.17 /snap/amazon-ssm-agent/5163/amazon-ssm-agent
# wins: 5, totalQueries: 51; ratio: 0.098039                                                                                     │    842 root       20   0 1290M 16412  6064 S  0.0  0.4  0:06.24 /snap/amazon-ssm-agent/5163/ssm-agent-worker
# Win indexes:0 10 47 110 169                                                                                                    │    458 root       20   0  235M  4220  3080 S  0.0  0.1  0:00.73 /usr/lib/accountsservice/accounts-daemon
# BAD                                                                                                                            │    648 postgres   20   0  212M 17100 15068 S  0.0  0.4  0:00.22 /usr/lib/postgresql/12/bin/postgres -D /var/lib/postgresql/12
# mostPopular: 3.695614, 0.369561                                                                                                │    836 root       20   0  852M 25136  8040 S  0.0  0.6  0:04.58 /usr/lib/snapd/snapd
# Board with piece:                                                                     │    836 root       20   0  852M 25136  8040 S  0.0  0.6  0:04.58 /usr/lib/snapd/snapd
# mostPopular: 3.367799, 0.336780                                                                                                │    361 systemd-t  20   0 89972  4408  3632 S  0.0  0.1  0:00.08 /lib/systemd/systemd-timesyncd
# mostPopular: 3.367799, 0.336780                                                                                                │    775 root       20   0  852M 25136  8040 S  0.0  0.6  0:00.16 /usr/lib/snapd/snapd
# Board with piece:                                                                                                              │    671 postgres   20   0  212M  3696  1632 S  0.0  0.1  0:00.35 postgres: 12/main: background writer
# ..........

# 100: wins: 8, totalQueries: 81; ratio: 0.098765
# 100: wins: 25, totalQueries: 266; ratio: 0.093985
# 300: wins: 26, totalQueries: 269; ratio: 0.096654
# 500: wins: 17, totalQueries: 269; ratio: 0.063197
# 500: wins: 27, totalQueries: 269; ratio: 0.100372 1:27.22 total
# 200: wins: 20, totalQueries: 263; ratio: 0.076046 1:01.87 tota


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    required = parser.add_argument_group('required arguments')
    required.add_argument('-s', '--streamkey',
            help='twitch streamkey',
            required=True)
    required.add_argument('-f', '--fps',
            help='moves per second',
            default=5)
    args = parser.parse_args()


    width = 640
    height = 480

    fps = int(args.fps)
    video_fps = fps * 2
    print('FPS =', fps)
    print('VIDEO FPS =', video_fps)
    stream = Stream(width, height, video_fps, fps)
    # if True:
    #     for _ in range(100):
    #         stream.prebuffer()
        # return

    dropped_conn_cnt = 0
    with TwitchBufferedOutputStream(
            twitch_stream_key=args.streamkey,
            width=width,
            height=height,
            fps=video_fps,
            crf=29,
            verbose=True) as videostream:
        while True:
            buffer_state = videostream.get_video_frame_buffer_state()
            if buffer_state < 20 * video_fps: # always have 10s worth of content buffered
                videostream.send_video_frame(stream.get_frame())
                dropped_conn_cnt = 0
            else:
                stream.prebuffer()
                time.sleep(0.001)
                dropped_conn_cnt += 1
                if dropped_conn_cnt > 1000 * 10: # 10 seconds without sending a frame
                    break

if __name__ == "__main__":
    main()
