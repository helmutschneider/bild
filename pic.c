#include "pic.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

bmp_t bmp_decode(uint8_t *data, size_t len)
{
    bitstream_t bs = bitstream_init(data, len);
    bmp_t bmp = {0};

    u16 magic = bitstream_read_u16(&bs, 2);
    bmp.header_magic = magic;

    bitstream_skip(&bs, 8);

    u32 pix_offset = bitstream_read_u32(&bs, 4);
    printf("pix_offset = %d\n", pix_offset);

    u32 dib_size = bitstream_read_u32(&bs, 4);

    bmp.dib_size = dib_size;

    switch (dib_size) {
        case 12:
            bmp.dib_variant = BMP_DIB_BITMAPCOREHEADER;
            break;
        case 40:
            bmp.dib_variant = BMP_DIB_BITMAPINFOHEADER;
            break;
    }

    u32 width = bitstream_read_u32(&bs, 4);
    u32 height = bitstream_read_u32(&bs, 4);
    bmp.width = width;
    bmp.height = height;

    bitstream_skip(&bs, 2);

    u16 bits_per_pixel = bitstream_read_u16(&bs, 2);
    bmp.bits_per_pixel = bits_per_pixel;

    u32 compression = bitstream_read_u32(&bs, 4);
    bmp.compression = compression;

    bitstream_skip(&bs, 12);

    u32 num_colors = bitstream_read_u32(&bs, 4);

    // no color palette.
    assert(num_colors == 0);

    bitstream_skip(&bs, 4);
    assert(bs.offset == 54);

    bmp.data = malloc(sizeof(pixel_t) * width * height);

    for (u32 i = 0; i < height; ++i)
    {
        u32 prev_offset = bs.offset;

        for (u32 k = 0; k < width; ++k)
        {
            // FIXME: support other formats than 24-bit.
            u8 b = bitstream_read_u8(&bs, 1);
            u8 g = bitstream_read_u8(&bs, 1);
            u8 r = bitstream_read_u8(&bs, 1);

            pixel_t px = {
                .r = r,
                .g = g,
                .b = b,
                .a = 255,
            };

            bmp.data[(i * width) + k] = px;
        }

        u32 padding = (bs.offset - prev_offset) % 4;
        bs.offset += padding;
    }

    return bmp;
}

void bmp_free(bmp_t pic)
{
    free(pic.data);
}

bitstream_t bitstream_init(u8 *data, size_t len)
{
    return (bitstream_t){
        .data = data,
        .data_len = len,
        .offset = 0,
    };
}

u64 bitstream_read_u64(bitstream_t *stream, size_t bytes)
{
    u64 out = 0;

    for (size_t i = 0; i < bytes; ++i)
    {
        u8 value = stream->data[stream->offset + i];
        u8 shl = i * 8;

        out |= (value << shl);
    }

    stream->offset += bytes;

    return out;
}

u8 bitstream_read_u8(bitstream_t *stream, size_t bytes)
{
    return bitstream_read_u64(stream, bytes);
}

u16 bitstream_read_u16(bitstream_t *stream, size_t bytes)
{
    return bitstream_read_u64(stream, bytes);
}

u32 bitstream_read_u32(bitstream_t *stream, size_t bytes)
{
    return bitstream_read_u64(stream, bytes);
}

void bitstream_skip(bitstream_t *stream, size_t bytes)
{
    stream->offset += bytes;
}
