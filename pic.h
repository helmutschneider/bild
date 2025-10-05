#ifndef _PIC_H_
#define _PIC_H_

#include <stdint.h>
#include <stddef.h>

typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} pixel_t;

typedef enum {
    BMP_DIB_UNKNOWN,
    BMP_DIB_BITMAPCOREHEADER,
    BMP_DIB_BITMAPINFOHEADER,
} bmp_dib_variant_t;

typedef enum {
    BMP_BI_RGB = 0,
    BMP_BI_RLE8 = 1,
    BMP_BI_RLE4 = 2,
    BMP_BI_BITFIELDS = 3,
    BMP_BI_JPEG = 4,
    BMP_BI_PNG = 5,
    BMP_BI_ALPHABITFIELDS = 6,
    BMP_BI_CMYK = 11,
    BMP_BI_CMYKRLE8 = 12,
    BMP_BI_CMYKRLE4 = 13,
} bmp_compression_t;

typedef struct {
    u16 header_magic;
    u32 dib_size;
    bmp_dib_variant_t dib_variant;
    u32 width;
    u32 height;
    u16 bits_per_pixel;
    bmp_compression_t compression;
    pixel_t *data;
} bmp_t;

typedef struct {
    u8 *data;
    size_t data_len;
    size_t offset;
} bitstream_t;

bitstream_t bitstream_init(u8 *data, size_t len);
u8 bitstream_read_u8(bitstream_t *stream, size_t bytes);
u16 bitstream_read_u16(bitstream_t *stream, size_t bytes);
u32 bitstream_read_u32(bitstream_t *stream, size_t bytes);
u64 bitstream_read_u64(bitstream_t *stream, size_t bytes);
void bitstream_skip(bitstream_t *stream, size_t bytes);

bmp_t bmp_decode(u8 *data, size_t len);
void bmp_free(bmp_t pic);

#endif
