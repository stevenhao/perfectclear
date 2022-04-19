
class Piece:
    def __init__(self, t, s):
        self.t = t
        self.blocks = []
        rows = s.split('\n')
        for i in range(len(rows)):
            for j in range(len(rows[i])):
                if rows[i][j] == '+':
                    self.blocks.append((i, j))

def get_color(t, active=False):
  return ['#00ffff', '#0000ff', '#ffa500', '#ffff00', '#00ff00', '#800080', '#ff0000'][t]