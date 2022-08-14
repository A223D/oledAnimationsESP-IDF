from PIL import Image
import os

buffer = []

WIDTH = 128
HEIGHT = 64

outputString = "#include \"aniBuf.h\"\n\nconst unsigned char bufferAnimation["


def drawPixel(x, y, colour):
    global buffer
    byteNum = (int(y / 8) * WIDTH) + x
    newByte = buffer[byteNum]
    if (colour == 1):
        newByte = newByte | (1 << (y % 8))
    else:
        newByte = newByte & (~(1 << (y % 8)))

    buffer[byteNum] = newByte


files = os.listdir("./frames")
#print(files)

outputString += str(len(files))

outputString += "][1024]={\n"

for file in files:
    outputString += "{"
    buffer = []
    for i in range(0, 1024):
        buffer.append(0)
    im = Image.open(os.path.join("./frames/", file))
    im = im.convert('RGBA')
    px = im.load()
    pix = list(im.getdata())

    for i in range(0, 64):
        for j in range(0, 64):
            #print("Orig ", str(i), " ", str(j))
            if (px[i, j] != (255, 255, 255, 255)):
                drawPixel(i + 32, j, 1)

    for i in range(0, 1024):
        outputString += str(buffer[i])
        if (i != 1023):
            outputString += ", "
    outputString += "},\n"

outputString = outputString[:-1]  #remove comma from last one

outputString += "\n};"

f = open("./main/aniBuf.c", "w")
f.write(outputString)
f.close()
