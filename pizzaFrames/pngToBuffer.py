from PIL import Image
import os

buffer=[]

WIDTH = 128
HEIGHT = 64

outputString = "ui"

def drawPixel(x, y, colour):
    global buffer
    byteNum = (int(y / 8) * WIDTH) + x
    newByte = buffer[byteNum]
    if (colour == 1):
        newByte = newByte | (1 << (y % 8))
    else:
        newByte = newByte & (~(1 << (y % 8)))

    buffer[byteNum] = newByte


files = os.listdir("./")
print(files)



for file in files:
    buffer = []
    for i in range(0, 1024):
        buffer.append(0)
    im = Image.open(file)
    im = im.convert('RGBA')
    px = im.load()
    pix = list(im.getdata())    

    for i in range(0, 64):
        for j in range(0, 64):
            #print("Orig ", str(i), " ", str(j))
            if (px[i, j] != (255, 255, 255, 255)):
                drawPixel(i + 32, j, 1)

print(len(buffer))

# f = open("./demofile2.txt", "w")
# f.write(str(buffer))
# f.close()
