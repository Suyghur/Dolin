import os
import sys
import zlib


def decompress_py2(args):
    decompressor = zlib.decompressobj(-zlib.MAX_WBITS)
    src = args[0]
    dst = src[:len(src) - 4] + "-decompress.zap"
    _buffer = []
    with open(src, "rb") as src_fp:
        _buffer = bytearray(os.path.getsize(src))
        src_fp.readinto(_buffer)
    with open(dst, "w+") as dst_fp:
        tmp = decompressor.decompress(str(_buffer))
        dst_fp.write(tmp)


def decompress_py3(args):
    decompressor = zlib.decompressobj(-zlib.MAX_WBITS)
    src = args[0]
    dst = src[:len(src) - 4] + "-decompress.zap"
    with open(src, "rb") as src_fp:
        _buffer = src_fp.read()
    with open(dst, "w+") as dst_fp:
        tmp = decompressor.decompress(_buffer)
        dst_fp.write(bytes.decode(tmp))


if __name__ == "__main__":
    decompress_py3(sys.argv[1:])
    # py2用下面的方法
    # decompress_py2(sys.argv[1:])
