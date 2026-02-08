import sys
import struct
import os

nsectors = 256 # 128kb 
buf = bytearray(nsectors * 512) 
used = [0] * nsectors
used[1] = 1
filelist = []

def write_sector(data, sector):
    print(f'writing {len(data)} bytes into sector {sector}')
    buf[sector * 512: sector * 512 + min(len(data), 512)] = data    
    used[sector] = 1

# copy the file into the sectors starting from 'start' (0 indexed)
def write_data(bs, start):
    print(f'writing {len(bs)} bytes to sectors {start}')
    bslen = len(bs)
    off = 0
    while off < bslen:
        left = bslen - off
        write_sector(bs[off:off + min(left,512)],start)
        start += 1
        off += min(512,left)
    
def add_file(file, sector=-1, recname=""):
    with open(file, "rb") as f:
        bs = bytearray(f.read())
        bslen = len(bs) 
        if sector == -1:
            found = True
            mreqsects = (bslen+511)//512;
            for i in range(nsectors - mreqsects):
                found = True
                for j in range((bslen+511)//512):
                    if used[i + j] == 1:
                        found = False
                        break;
                if found:
                    sector = i
                    break
            if sector == -1:
                print(f"couldn't find an empty sector for file {file}:{bslen}")
        write_data(bs, sector)
        if recname == "":
            recname = os.path.splitext(os.path.basename(file))[0]
        filelist.append((recname, sector, bslen))

def write_file_table():
    print('writing file table:')
    print(filelist)
    off = 512 # second sector
    for i in filelist:
        struct.pack_into('<H', buf, off, i[1])
        off += 2
        struct.pack_into('<H', buf, off, i[2])
        off += 2
        namelen = min(12, len(i[0]))
        buf[off: off + namelen] = i[0].encode('utf-8')[0:namelen]
        off += 12

out = "bootimage"
i = 1
while i < len(sys.argv):
    arg = sys.argv[i]
    if arg == "-f": # first stage bootloader
        add_file(sys.argv[i + 1], 0)
        i += 2
    elif arg == "-s":
        add_file(sys.argv[i + 1], 2)
        i += 2
    elif arg == "-o":
        out = sys.argv[i + 1]
        i += 2 
    else:
        add_file(sys.argv[i])
        i += 1
write_file_table()

with open(out, 'wb') as f:
    f.write(buf)




