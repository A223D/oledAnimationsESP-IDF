from types import new_class
from PIL import Image

im = Image.open('./frame_00_delay-0.03s.png')
im = im.convert('RGBA')
px = im.load()

pix = list(im.getdata())

WIDTH = 128
HEIGHT = 64


buffer = []


def drawPixel(x, y, colour):
    global buffer
    byteNum = (int(y / 8) * WIDTH) + x
    newByte = buffer[byteNum]
    if(colour ==1):
        newByte = newByte | (1 << (y%8))
    else:
        newByte = newByte & (~(1 << (y % 8)))

    buffer[byteNum] = newByte

#setup function
for i in range(0, 1024):
    buffer.append(0)



for i in range(0, 64):
    for j in range(0, 64):
        #print("Orig ", str(i), " ", str(j))
        if(px[i, j] != (255, 255, 255, 255)):
            drawPixel(i+32, j, 1)

print(len(buffer))

f = open("./demofile2.txt", "w")
f.write(str(buffer))
f.close()

