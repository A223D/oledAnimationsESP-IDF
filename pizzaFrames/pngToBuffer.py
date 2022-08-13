from PIL import Image

im = Image.open('./frame_40_delay-3.03s.png')
im = im.convert('RGBA')
px = im.load()

pix = list(im.getdata())


buffer = []



def drawPixel:
    global buffer


for i in range(0, 64):
    buffer.append([])

for 


for i in range(0, 64):
    for j in range(0, 64):
        px[i, j]

