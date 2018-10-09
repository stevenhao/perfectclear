from __future__ import print_function
from outputvideo import TwitchBufferedOutputStream
import argparse
import numpy as np

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    required = parser.add_argument_group('required arguments')
    required.add_argument('-s', '--streamkey',
            help='twitch streamkey',
            required=True)
    args = parser.parse_args()

    with TwitchBufferedOutputStream(
            twitch_stream_key=args.streamkey,
            width=640,
            height=480,
            fps=30.,
            verbose=True) as videostream:
        frame = np.zeros((480, 640, 3))
        while True:
            if videostream.get_video_frame_buffer_state() < 30:
                frame = np.random.rand(480, 640, 3)
                videostream.send_video_frame(frame)
