/*

*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(void)
{
	int fd = -1;
	int ret = -1;
	int i = 0;
	int j = 0;
	unsigned char buf[320*240*2] = {0x00};

	fd = open("/dev/spi-fb", O_RDWR);
	if(fd < 0) {
		printf("can not open dev\n");
		return -1;
   	}

	while(1){
		unsigned char temp[3]={0x00};
		unsigned char r = 0x00;
		unsigned char g = 0x00;
		unsigned char b = 0x00;
		unsigned char rgb565[2] = {0x00};

		i++;
		temp[i%3] = 0xFF;
		r = temp[0];
		g = temp[1];
		b = temp[2];
		printf("r=0x%02X, g=0x%02X, b=0x%02X\n", r, g, b);



		rgb565[0] = (r&0xF8) | (g>>5);
		rgb565[1] = ((g<<3)&0xE0) | (b>>3);
		printf("rgb565[0]=0x%02X, rgb565[1]=0x%02X\n", rgb565[0],rgb565[1]);

		for(j=0;j<320*240*2;) {
			buf[j++] = rgb565[0];
			buf[j++] = rgb565[1];
		}

		ret = write(fd, buf, 240*320*2);
		if(ret<0)
			printf("write failed\n");

		sleep(10);
	}

	return 0;
}








