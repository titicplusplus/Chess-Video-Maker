import os

for nfile in os.listdir():
    if nfile.find('.svg') != -1:
        print(nfile)
        os.system(f"inkscape -w 128 -h 128 {nfile} -o {nfile[:len(nfile)-4]}.png")

