#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "font_8x16.h"
#define FB_DEV "/dev/graphics/fb0"

int fb_open(char *fb_device);
int fb_close(int fd);
void *fb_mmap(int fd,unsigned int screensize);
int fb_munmap(void *start,size_t length);
int fb_16pixel(unsigned char *fbmem,int width,int height,int x,int y,unsigned short color);
void char_display(char ch,unsigned char *fbmem,unsigned int width,unsigned int height);

int main(int argc,char *argv[])
{
	int fbdev;
	char *fb_device;
	unsigned char *fbmem;
	unsigned int screensize;
	unsigned int fb_width;
	unsigned int fb_height;
	unsigned int fb_depth;
	unsigned int x;
	unsigned int y;

	if((fb_device = getenv("FRAMEBUFFER")) == NULL)
	{
		fb_device = FB_DEV;
		fbdev = fb_open(fb_device);
	}else printf("%s",fb_device);
	

	fb_width = 240;
	fb_height = 320;
	fb_depth = 16;

	screensize = fb_width * fb_height *(fb_depth / 8);
	fbmem = fb_mmap(fbdev,screensize);
	memset (fbmem, 0, screensize);//清屏
	char_display(*argv[1],fbmem,fb_width,fb_height);
	fb_munmap(fbmem,screensize);

	fb_close(fbdev);

	return (0);
}

void char_display(char ch,unsigned char *fbmem,unsigned int width,unsigned int height)
{
	int i,j,n;
	unsigned char list;
	n = ch * 16;
	printf("%d",n);
	for(i = 0;i < 16;i++,n++)
	{
		list = fontdata_8x16[n];
		for(j = 0;j < 8;j++)
		{
			if(list & 1 << (7 - j))
			{
				fb_16pixel(fbmem,width,height,50 + j,50 + i, 0x1C3B);
		
			}
		}
	}
}

int fb_open(char *fb_device)
{
	int fd;
	if((fd = open(fb_device,O_RDWR)) < 0)
	{
		perror(__func__);
		return (-1);
	}
	return (fd);
}

void *fb_mmap(int fd,unsigned int screensize)
{
	void*  fbmem;

	if((fbmem = mmap(0,screensize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0)) == MAP_FAILED)
	{
		perror(__func__);
		return (void *)(-1);
	}
	return (fbmem);
}

int fb_munmap(void *start,size_t length)
{
	return (munmap(start,length));
}

int fb_close(int fd)
{
	return (close(fd));
}

int fb_16pixel(unsigned char *fbmem, int width,int height,int x,int y,unsigned short color)
{
	if((x > width)||(y > height))
		return (-1);
	unsigned short *dst = ((unsigned short *)fbmem + y * width + x);

	*dst = color;
	return (0);
}
