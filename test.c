#include "pic.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

static void test_bmp_header(u8 *data, size_t len)
{
    pic_t pic = pic_decode(data, len);

    assert(pic.format == PIC_FORMAT_BMP);

    bmp_t bmp = pic.bmp;

    printf("dib = %d\n", bmp.dib_size);

    assert(bmp.header_magic == 0x4D42);
    assert(bmp.dib_size == 40);
    assert(bmp.dib_variant == BMP_DIB_BITMAPINFOHEADER);
    assert(bmp.width == 200);
    assert(bmp.height == 200);
    assert(bmp.bits_per_pixel == 24);
    assert(bmp.compression == BMP_BI_RGB);

    // middle of the first row.
    pixel_t px = pic.data[100];

    assert(px.a == 255);
    assert(px.r == 0);
    assert(px.g == 255);
    assert(px.b == 0);

    printf("%d %d\n", pic.width, pic.height);
    printf("(%d, %d, %d)\n", pic.data[100].r, pic.data[100].g, pic.data[100].b);

    pic_free(pic);
}

int main(void)
{
    u8 *test_bmp = malloc(sizeof(u8) * 10 * 1024 * 1024);
    size_t test_bmp_size = read_file("bmp_24.bmp", test_bmp);

    test_bmp_header(test_bmp, test_bmp_size);

    free(test_bmp);

    return 0;
}
