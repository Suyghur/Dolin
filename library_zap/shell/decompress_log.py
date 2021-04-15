import zlib
import os
import sys


def decompress(args):
    decompressor = zlib.decompressobj(-zlib.MAX_WBITS)
    src = args[0]
    dst = args[1]
    _buffer = []
    with open(src, "rb") as src_fp:
        _buffer = bytearray(os.path.getsize(src))
        src_fp.readinto(_buffer)
    with open(dst, "w+") as dst_fp:
        tmp = decompressor.decompress(str(_buffer))
        dst_fp.write(tmp)


if __name__ == "__main__":
    decompress(sys.args[1:])
