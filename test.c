#include "pic.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

static size_t read_file(const char *name, uint8_t *out)
{
    FILE *handle = fopen(name, "rb");

    if (handle == NULL)
    {
        printf("Cannot open %s\n", name);
        exit(1);
        return 0;
    }

    size_t read = 0;

    while (true)
    {
        int chunk = fread(out + read, sizeof(uint8_t), 8192, handle);
        if (chunk == 0)
        {
            break;
        }
        read += chunk;
    }

    fclose(handle);

    return read;
}

static void test_bmp_header(u8 *data, size_t len)
{
    bmp_t pic = bmp_decode(data, len);

    assert(pic.header_magic == 0x4D42);
    assert(pic.dib_size == 40);
    assert(pic.dib_variant == BMP_DIB_BITMAPINFOHEADER);
    assert(pic.width == 200);
    assert(pic.height == 200);
    assert(pic.bits_per_pixel == 24);
    assert(pic.compression == BMP_BI_RGB);

    printf("%d %d\n", pic.width, pic.height);
    printf("(%d, %d, %d)\n", pic.data[100].r, pic.data[100].g, pic.data[100].b);

    bmp_free(pic);
}

int main()
{
    uint8_t test_bmp[1048576];
    size_t test_bmp_size = read_file("bmp_24.bmp", test_bmp);

    test_bmp_header(test_bmp, test_bmp_size);
}
