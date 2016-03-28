
#include <fcntl.h>
#include <stdio.h>

#include <linux/ioctl.h>  //for _IOR

/*sii902xA Debug*/
/**********************************/
#define ENTER \
do{ printf("[sii902xA_DBG][%04d][%s]\n", __LINE__, __func__); }while(0)
 
#define PRINT_DBG(format,x...) \
do{ printf("[sii902xA_DBG][%04d] " format, __LINE__, ## x); }while(0)
 
#define PRINT_INFO(format,x...) \
do{ printf("[sii902xA_INFO][%04d] " format, __LINE__, ## x); }while(0)
 
#define PRINT_WARN(format,x...) \
do{ printf("[sii902xA_WARN][%04d] " format, __LINE__, ## x); }while(0)
 
#define PRINT_ERR(format,x...) \
do{ printf("[sii902xA_ERR][%04d][%s] " format, __LINE__, __func__, ## x); }while(0)
/**********************************/


//video_parameter_num
#define _defxdef_defHZ 0
#define _720x400_70HZ 1
#define _720x400_88HZ 2
#define _640x480_60HZ 3
#define _640x480_67HZ 4
#define _640x480_72HZ 5
#define _640x480_75HZ 6
#define _800x600_56HZ 7
#define _800x600_60HZ 8
#define _800x600_72HZ 9
#define _800x600_75HZ 10
#define _832x624_75HZ 11
#define _1024x768i_87HZ 12
#define _1024x768_60HZ 13
#define _1024x768_72HZ 14
#define _1024x768_75HZ 15
#define _1280x1024_75HZ 16
#define _1152x870_75HZ 17

//ioctl
#define sii902xA_IOCTL (0x99)
#define sii902xA_GET_RAW_EDID       _IOR(sii902xA_IOCTL, 0x1, unsigned long)
#define sii902xA_GET_TIMING_DESC    _IOR(sii902xA_IOCTL, 0x2, unsigned long)
#define sii902xA_GET_TIMING_BITMAP  _IOR(sii902xA_IOCTL, 0x3, unsigned long)
#define sii902xA_SET_VIDEO          _IOW(sii902xA_IOCTL, 0x4, unsigned long)
#define sii902xA_SET_AUDIO          _IOW(sii902xA_IOCTL, 0x5, unsigned long)
#define sii902xA_START              _IOW(sii902xA_IOCTL, 0x6, unsigned long)

#define DEV_NAME "/dev/sii902xA"

struct timing_desc {
    unsigned int pixel_clock;                     /*  = pclk  */
    unsigned int horizontal_active;           /*  = xres  */
    unsigned int horizontal_blanking;       /*  = hsync+hbp+hfp  */
    unsigned int vertical_active;               /*  = yres  */
    unsigned int vertical_blanking;           /*  = vsync+vbp+vfp  */
    unsigned int horizontal_sync_offset;  /*  = hfp  */
    unsigned int horizontal_sync_pulse;   /*  = hsync  */
    unsigned int vertical_sync_offset;      /*  = vfp  */
    unsigned int vertical_sync_pulse;       /*  = vsync  */
}; 

struct timing_bitmap {
    unsigned char  timing_720x400_70hz;
    unsigned char  timing_720x400_88hz;
    unsigned char  timing_640x480_60hz;
    unsigned char  timing_640x480_67hz;
    unsigned char  timing_640x480_72hz;
    unsigned char  timing_640x480_75hz;
    unsigned char  timing_800x600_56hz;
    unsigned char  timing_800x600_60hz;
    unsigned char  timing_800x600_72hz;
    unsigned char  timing_800x600_75hz;
    unsigned char  timing_832x624_75hz;
    unsigned char  timing_1024x768i_87hz;
    unsigned char  timing_1024x768_60hz;
    unsigned char  timing_1024x768_72hz;
    unsigned char  timing_1024x768_75hz;
    unsigned char  timing_1280x1024_75hz;
    unsigned char  timing_1152x870_75hz;
};

static int fd = -1;
static unsigned char  edid[128];
static struct timing_desc std_timing;
static struct timing_bitmap timing_bmp;

int16_t dev_open(void)
{
	ENTER;
	if (fd < 0) {
		if ((fd = open(DEV_NAME, O_RDWR)) < 0) {
			PRINT_ERR("open file failed! PATH=%s\n", DEV_NAME);
			return -1;
		}
	}
	PRINT_INFO("open file success! PATH=%s\n", DEV_NAME);
	return 0;
}

void dev_close(void)
{
	ENTER;
	if (fd >= 0) {
		close(fd);
		fd = -1;
	}
}

int get_raw_edid(unsigned char  *edid, int size)
{
	ENTER;
	if (fd < 0) {
		PRINT_ERR("Device file is not opened\n");
		return -1;
	}
	if (size != 128) {
		PRINT_ERR("edid data must be 128 BYTE\n");
		return -1;
	}

	if (ioctl(fd, sii902xA_GET_RAW_EDID, edid) < 0) {
		PRINT_ERR("ioctl error\n");
		return -1;
	}

	{
		int i = 0;
		PRINT_INFO("print EDID Data:\n");
		printf("=============EDID Data===============\n");
		for(i=0; i<size; i++)
		{
			printf(" %02X", edid[i]);
			if((i+1)%16 == 0)
			printf("\n");
		}
		printf("=============EDID Data===============\n");
	}

	return 0;
}

int get_timing_desc(struct timing_desc *std_timing)
{
	ENTER;
	if (fd < 0) {
		PRINT_ERR("Device file is not opened\n");
		return -1;
	}

	if (ioctl(fd, sii902xA_GET_TIMING_DESC, std_timing) < 0) {
		PRINT_ERR("ioctl error\n");
		return -1;
	}

	PRINT_INFO("std_timing->pixel_clock:%d\n", std_timing->pixel_clock);
	PRINT_INFO("std_timing->horizontal_active:%d\n", std_timing->horizontal_active);
	PRINT_INFO("std_timing->horizontal_blanking:%d\n", std_timing->horizontal_blanking);
	PRINT_INFO("std_timing->vertical_active:%d\n", std_timing->vertical_active);
	PRINT_INFO("std_timing->vertical_blanking:%d\n", std_timing->vertical_blanking);
	PRINT_INFO("std_timing->horizontal_sync_offset:%d\n", std_timing->horizontal_sync_offset);
	PRINT_INFO("std_timing->horizontal_sync_pulse:%d\n", std_timing->horizontal_sync_pulse);
	PRINT_INFO("std_timing->vertical_sync_offset:%d\n", std_timing->vertical_sync_offset);
	PRINT_INFO("std_timing->vertical_sync_pulse:%d\n", std_timing->vertical_sync_pulse);

	return 0;
}

int get_timing_bitmap(struct timing_bitmap *timing_bmp)
{
	ENTER;
	if (fd < 0) {
		PRINT_ERR("Device file is not opened\n");
		return -1;
	}

	if (ioctl(fd, sii902xA_GET_TIMING_BITMAP, timing_bmp) < 0) {
		PRINT_ERR("ioctl error\n");
		return -1;
	}

	PRINT_INFO("timing_bmp->timing_720x400_70hz  :%d\n", timing_bmp->timing_720x400_70hz  );
	PRINT_INFO("timing_bmp->timing_720x400_88hz  :%d\n", timing_bmp->timing_720x400_88hz  );
	PRINT_INFO("timing_bmp->timing_640x480_60hz  :%d\n", timing_bmp->timing_640x480_60hz  );
	PRINT_INFO("timing_bmp->timing_640x480_67hz  :%d\n", timing_bmp->timing_640x480_67hz  );
	PRINT_INFO("timing_bmp->timing_640x480_72hz  :%d\n", timing_bmp->timing_640x480_72hz  );
	PRINT_INFO("timing_bmp->timing_640x480_75hz  :%d\n", timing_bmp->timing_640x480_75hz  );
	PRINT_INFO("timing_bmp->timing_800x600_56hz  :%d\n", timing_bmp->timing_800x600_56hz  );
	PRINT_INFO("timing_bmp->timing_800x600_60hz  :%d\n", timing_bmp->timing_800x600_60hz  );
	PRINT_INFO("timing_bmp->timing_800x600_72hz  :%d\n", timing_bmp->timing_800x600_72hz  );
	PRINT_INFO("timing_bmp->timing_800x600_75hz  :%d\n", timing_bmp->timing_800x600_75hz  );
	PRINT_INFO("timing_bmp->timing_832x624_75hz  :%d\n", timing_bmp->timing_832x624_75hz  );
	PRINT_INFO("timing_bmp->timing_1024x768i_87hz:%d\n", timing_bmp->timing_1024x768i_87hz);
	PRINT_INFO("timing_bmp->timing_1024x768_60hz :%d\n", timing_bmp->timing_1024x768_60hz );
	PRINT_INFO("timing_bmp->timing_1024x768_72hz :%d\n", timing_bmp->timing_1024x768_72hz );
	PRINT_INFO("timing_bmp->timing_1024x768_75hz :%d\n", timing_bmp->timing_1024x768_75hz );
	PRINT_INFO("timing_bmp->timing_1280x1024_75hz:%d\n", timing_bmp->timing_1280x1024_75hz);
	PRINT_INFO("timing_bmp->timing_1152x870_75hz :%d\n", timing_bmp->timing_1152x870_75hz );

	return 0;
}

int set_video(int *video_mode)
{

	ENTER;
	if (fd < 0) {
		PRINT_ERR("Device file is not opened\n");
		return -1;
	}

	if (ioctl(fd, sii902xA_SET_VIDEO, video_mode) < 0) {
		PRINT_ERR("ioctl error\n");
		return -1;
	}

	return 0;
}


int hdmi_start(void)
{

	ENTER;
	if (fd < 0) {
		PRINT_ERR("Device file is not opened\n");
		return -1;
	}

	if (ioctl(fd, sii902xA_START, NULL) < 0) {
		PRINT_ERR("ioctl error\n");
		return -1;
	}

	return 0;
}

int main(void)
{
	int choice = 0;
	int ret = -1;
	int video_mode = 0;

	ret = dev_open();
	if(ret < 0) {
		PRINT_ERR("can not open \"%s\"\n", DEV_NAME);
		return -1;
	}

	while(1){
		printf("\n\n\n=====================================\n");
		printf("    0. Exit\n");
		printf("    1. sii902xA_GET_RAW_EDID\n");
		printf("    2. sii902xA_GET_TIMING_DESC\n");
		printf("    3. sii902xA_GET_TIMING_BITMAP\n");
		printf("    4. sii902xA_SET_VIDEO\n");
		printf("    5. sii902xA_SET_AUDIO\n");
		printf("    6. sii902xA_START\n");
		printf("=====================================\n");
		printf("please input your choice number: ");
		scanf("%d", &choice);
		printf("_____________________________\n");
		switch(choice) {
			case 0:
				PRINT_INFO("Exit success\n\n");
				dev_close();
				return 0;
			case 1:
				get_raw_edid(edid, 128);
				break;
			case 2:
				get_timing_desc(&std_timing);
				break;
			case 3:
				get_timing_bitmap(&timing_bmp);
				break;
			case 4:
				printf("    please input video mode number: ");
				scanf("%d", &video_mode);
				set_video(&video_mode);
				break;
			case 5:
				hdmi_start();
				break;
			default:
				break;		
		};
	
	};

	dev_close();
	PRINT_INFO("Exit success\n\n");
	return 0;
}




