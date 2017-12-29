import struct

f = open('v.txt', 'wb')
word = 'c1234c5678d2345e6789j'
f.write(struct.pack("B", 0xB8))
[f.write(struct.pack("s", x)) for x in '1234']
f.write(struct.pack("B", 0xBA))
[f.write(struct.pack("s", x)) for x in '5678']
f.write(struct.pack("B", 0xBB))
[f.write(struct.pack("s", x)) for x in '9012']
f.write(struct.pack("B", 0xBC))
[f.write(struct.pack("s", x)) for x in '3456']
f.write(struct.pack("B", 0xE9))
f.close()

linen = 0
for line in open('v.txt'):
  linen = len(line)
  
f = open('v.txt', 'ab')
f.write(struct.pack("<i", -(linen+4+0x7c00)))
f.close()
