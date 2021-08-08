import struct

inf = 'x324.vlw'
outf = 'x324.prt'

with open(inf, 'rb') as f1:
    x = f1.read()

print(len(x))

gCount, ver, yAdvance, dummy, ascent, descent = struct.unpack('>llllll', x[:24])
print(gCount, ver, yAdvance, dummy, ascent, descent)

m = x[24:24+gCount*28]
bitmap = x[24+gCount*28:]

print(len(m), len(bitmap))
print(len(m)/28)

in_metrics = list(struct.iter_unpack('>lllllll', m))
print(len(in_metrics))

max_descent = 0
disp = 0
glyph_metrics = b''
for m in in_metrics:
    code, height, width, xAdvance, dY, dX, dummy = m
#    print(code, height, width, xAdvance, dY, dX)
    s = struct.pack('<lllllll', code, height, width, xAdvance, dY, dX, disp)
    glyph_metrics += s
    disp += height * width
    if ((code > 0x20 and code < 0xA0 and code != 0x7F) or code > 0xFF):
        descent1 = height - dY
        if descent1 > max_descent: max_descent = descent1

yAdvance = ascent + max_descent
font_metrics = struct.pack('<llllll', gCount, ver, yAdvance, max_descent, ascent, descent)

out_data = font_metrics + glyph_metrics + bitmap

with open(outf, 'wb') as f2:
    f2.write(out_data)
