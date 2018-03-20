#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <time.h>
#include <math.h>
#include <stdint.h>

#define WIDTH 13
#define HEIGHT 36
//#define TAIL 21
#define PIXELS (WIDTH * HEIGHT)
#define BYTES_IN (PIXELS * 3)
#define BYTES_OUT (PIXELS * 4)
#define TAIL (4 + PIXELS / 4)

// raw RGB values for a theoretical pure white pixel
#define WHITE_R 255
#define WHITE_G 210
#define WHITE_B 200

static uint8_t rgb_table[256][3];

static char out[BYTES_OUT + TAIL];

// see http://en.wikipedia.org/wiki/SRGB
static double rgbs(double x)
{
    if (x <= 0.04045) {
        return x / 12.92;
    } else {
        return pow((x + 0.055) / (1 + 0.055), 2.4);
    }
}

static void create_table(uint8_t table[][3], uint8_t white[3])
{
    int i, j;

    for (i = 0; i < 256; i++) {
        double t = i / 255.0;
        for (j = 0; j < 3; j++) {
            table[i][j] = round(rgbs(t) * white[j]);
        }
    }    
}


int main (void) {
    char in[BYTES_IN];
    int i, c;
    uint8_t r,g,b;

    uint8_t white[] = {WHITE_R, WHITE_G, WHITE_B};
    create_table(rgb_table, white);

    int spi = open("/dev/spidev0.0", O_WRONLY);

    int hz = 1000000;
    int lsb_first = 0;
    int mode = 0;
    ioctl(spi, SPI_IOC_WR_MODE, &mode);
    ioctl(spi, SPI_IOC_WR_LSB_FIRST, &lsb_first);
    ioctl(spi, SPI_IOC_WR_MAX_SPEED_HZ, &hz);
    
    while ((c = read(0, in, BYTES_IN))) {
        write(spi, "\0\0\0\0", 4);
        for (i = 0; i < (c/3); i++) {
            r = in[i * 3 + 1];
            g = in[i * 3 + 2];
            b = in[i * 3 + 0];
            
            out[i * 4 + 0] = 0xff;  // alpha
            out[i * 4 + 2] = rgb_table[r][0];
            out[i * 4 + 1] = rgb_table[g][1];
            out[i * 4 + 3] = rgb_table[b][2];
        }
        write(spi, out, sizeof(out));
    }
    close(spi);
    return 0;
}
