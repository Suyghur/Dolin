import zlib


def decompress_zlib(src, dst):
    src = open(src, "rb+")
    dst = open(dst, "wb+")
    handler = zlib.decompressobj(-zlib.MAX_WBITS)
    data = src.read(1024)
    while data:
        dst.write(handler.decompress(data))
        data = src.read(1024)
    dst.write(str(handler.flush))


if __name__ == "__main__":
    decompress_zlib("2021-04-15-zap","test.log")
