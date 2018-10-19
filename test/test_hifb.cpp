#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/kd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <signal.h>
#include <string.h>
#include <linux/fb.h>
#include "hifb/hifb.h"



#define HIFB_RED_1555     0xFC00
#define HIFB_BLACK_1555   0x8000


void draw_hline(char* image, int w, int h, int line_bytes, int pixelbytes, int y0, unsigned short color)
{
    int y1 = y0 + 1;
    for (int y = y0; y < y1; ++y) {
        for (int x = 0; x < w/2; ++x) {
            int index = y * line_bytes + x * pixelbytes;
            unsigned short* p = (unsigned short*)(image + index);
            *p = color;
        }
    }
}

void draw_vline(char* image, int w, int h, int line_bytes, int pixelbytes, int x0, unsigned short color)
{
    int x1 = x0 + 1;
    for (int y = 0; y < h; ++y) {
        for (int x = x0; x < x1; ++x) {
            int index = y * line_bytes + x * pixelbytes;
            unsigned short* p = (unsigned short*)(image + index);
            *p = color;
        }
    }
}

void draw_image(char* image, int w, int h, int line_bytes, int pixelbytes)
{
    int x0 = 0;
    int x1 = w / 2;
    int y0 = 0;
    int y1 = h / 2;
    for (int y = y0; y < y1; ++y) {
        for (int x = x0; x < x1; ++x) {
            int index = y * line_bytes + x * pixelbytes;
            unsigned short* p = (unsigned short*)(image + index);
            *p = HIFB_RED_1555;
        }
    }
}


int main(int argc, char* argv[])
{
    printf("hello\n");

    int vout = 0;
    const char* interface = "CVBS";
    int fd = HIFB_Open(vout, interface);
    printf("fd(%d)\n", fd);

    fb_fix_screeninfo finfo;
    fb_var_screeninfo vinfo;
    memset(&vinfo, 0, sizeof(vinfo));
    memset(&finfo, 0, sizeof(finfo));

    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) != 0) {
        perror("get fix screen info failed!\n");
    }

    if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("get virtual screen info failed!\n");
    }

    printf("finfo.line_length: %d\n", finfo.line_length);
    printf("vinfo.res        : %d, %d\n", vinfo.xres, vinfo.yres);
    printf("vinfo.offset     : %d, %d\n", vinfo.xoffset, vinfo.yoffset);

    // mmap the framebuffer
    int data_size = finfo.smem_len;
    char* data = (char *)mmap(0, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    printf("frame buffer(%p) size(%d)\n", data, data_size);

    //sleep(1);
    //memset(data, 0x80, data_size);
    //sleep(1);

    int w = vinfo.xres;
    int h = vinfo.yres;
    int pixelbytes = 2;
    int linebytes = finfo.line_length;
    //draw_image(data, w, h, linebytes, pixelbytes);
#if 1
    for (int y = 0; y < 320; ++y) {
		draw_hline(data, w, h, linebytes, pixelbytes, y, HIFB_RED_1555);
		usleep(10);
		//draw_hline(data, w, h, linebytes, pixelbytes, y, HIFB_BLACK_1555);
		//usleep(10);
	}
#endif
#if 0
    for (int x = 0; x < 320; ++x) {
		draw_vline(data, w, h, linebytes, pixelbytes, x, HIFB_RED_1555);
		usleep(10);
		draw_vline(data, w, h, linebytes, pixelbytes, x, HIFB_BLACK_1555);
		usleep(10);
	}
#endif
#if 0
    int wi = 8;
	int xoffset = 100;
	for (int bpl = 2560; bpl > 2558; bpl -= 4) {
		for (int i = 0; i < 320; ++i) {
			char* line = data + i * bpl;
			for (int x = 0; x < wi; ++ x) {
				unsigned short* p = (unsigned short*)(line + (x + xoffset) * 2);
				*p = HIFB_RED_1555;
			}
		}
		//sleep(1);
		//memset(data, 0x80, data_size);
	}
#endif
    sleep(30);

    munmap(data, data_size);
    HIFB_Close(vout, fd);
    return 0;
}

