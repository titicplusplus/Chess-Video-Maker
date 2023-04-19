import os
import sys

SIZE = 1080

directories = os.listdir(sys.argv[1])

for f in directories:
    print(f)
    if f == "board.svg":
        f = sys.argv[1] + f
        os.system(f"inkscape --without-gui {f} -w {SIZE} -h {SIZE} -o {f[:len(f)-4]}.png")
    else:
        f = sys.argv[1] + f
        os.system(f"inkscape --without-gui {f} -w {int(SIZE/8)} -h {int(SIZE/8)} -o {f[:len(f)-4]}.png")
