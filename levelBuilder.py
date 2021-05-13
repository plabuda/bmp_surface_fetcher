import math
import sys
import struct
import string
import re

floats = [0, 200,
          200, 0,
          0, 0,
          200, 200]


rotate_regex = re.compile(r'.*transform=\"rotate\((.*)(\).*)')
x_regex = re.compile(r'(.*x=\")(.*?)(\".*)')
y_regex = re.compile(r'(.*y=\")(.*?)(\".*)')
width_regex = re.compile(r'(.*width=\")(.*?)(\".*)')
height_regex = re.compile(r'(.*height=\")(.*?)(\".*)')


def wall(wstring):
    x_r = x_regex.match(wstring)
    y_r = y_regex.match(wstring)
    w_r = width_regex.match(wstring)
    h_r = height_regex.match(wstring)

    t_r = rotate_regex.match(wstring)

    if w_r is None or h_r is None:
        return None
    else:
        w = float(w_r.group(2))
        h = float(h_r.group(2))

        x = 0 if x_r is None else float(x_r.group(2))
        y = 0 if y_r is None else float(y_r.group(2))

        if t_r is None:
            x = x + w / 2
            y = y + h / 2
            angle = 0
        else:
            angle = math.radians(float(t_r.group(1)))

            offset_x = x + w / 2
            offset_y = y + h/2
            x = math.cos(angle) * offset_x - math.sin(angle) * offset_y           
            y = math.cos(angle) * offset_y + math.sin(angle) * offset_x

        return (x,y,angle,w,h)

def checkpoint(wstring):
    w = wall(wstring)
    if w is not None:
        return (w[0], w[1], w[2])
    else:
        return None

if __name__ == '__main__':
    if len(sys.argv) != 3:
        exit(1)
    else:
        with open(sys.argv[1], 'rt') as lvl_input:
            lines = lvl_input.readlines()
            rect_filter = re.compile(r'(.*<rect )(.+)(.+fill=\"#)(.{6})')
            
            
            rect_lines = filter(lambda x: x is not None, map(lambda x: rect_filter.match(x), lines))
            matches = sorted(map(lambda x: (x.group(4),x.group(2)), rect_lines), key=lambda x: x[0])
            
            #print('Matches: '+ str(list(matches))  + '\n\n\n')

            walls = []
            checkpoints = []

            for index, match in enumerate(matches):
                if not match[0].startswith('00'):
                    walls = list(filter(lambda x: x is not None, map(lambda y: wall(y[1]), matches[:index])))
                    checkpoints = list(filter(lambda x: x is not None, map(lambda y: checkpoint(y[1]), matches[index:])))
                    break

            wall_len = len(walls)
            wall_out = [wall_len] + list(sum(walls, ()))

            
            checkpoint_len = len(checkpoints)
            checkpoint_out = [checkpoint_len] + list(sum(checkpoints, ()))

            #print('checkpoints: ' + str(checkpoint_out) + '\n\n\n')
            #print('walls: ' + str(wall_out) + '\n\n\n')

        #print(sys.argv)

        format = 'f' * (5 * wall_len + 3 * checkpoint_len + 2)
        s = struct.pack(format, *wall_out, *checkpoint_out)        
        with open(sys.argv[2], 'wb') as file:
            file.write(s)