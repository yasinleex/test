/*
功能：测试读取触摸屏的分辨率
参考：http://www.myexception.cn/android/1392921.html
相关：关机闹钟启动时触摸坐标错误，导致本次实验。以下是关机闹钟相关资料
	源码路径：/home/apuser/mywork/4.1-3.4-new/device/sprd/common/apps/alarm/
	http://blog.csdn.net/g_salamander/article/details/8487328
kernel 参考:  evdev.c input.c
corss compiling参考：	http://blog.csdn.net/yasin_lee/article/details/5657143
编译方法：把目录放到/home/apuser/mywork/4.1-3.4-new/device/sprd/common/tools/
然后用mmm命令编译
mmm /home/apuser/mywork/4.1-3.4-new/device/sprd/common/tools/get-ctp-resolution
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/input.h>

#define LTR558_DEVICE_NAME	"/dev/ltr_558als"
#define LTR_IOCTL_MAGIC		0x1C
#define LTR_IOCTL_GET_PFLAG	_IOR(LTR_IOCTL_MAGIC, 1, int)
#define LTR_IOCTL_GET_LFLAG	_IOR(LTR_IOCTL_MAGIC, 2, int)
#define LTR_IOCTL_SET_PFLAG	_IOW(LTR_IOCTL_MAGIC, 3, int)
#define LTR_IOCTL_SET_LFLAG	_IOW(LTR_IOCTL_MAGIC, 4, int)
#define LTR_IOCTL_GET_DATA	_IOW(LTR_IOCTL_MAGIC, 5, unsigned char)
#define LTR_IOCTL_GET_LDATA	_IOR(LTR_IOCTL_MAGIC, 6, int)

int main(void)
{
	int fd;
	int l_flag = 1;
	int l_data = 0;

	fd = open(LTR558_DEVICE_NAME, O_RDWR);
	if(fd < 0) {
		printf("can not open dev\n");
		return -1;
   	}
 
	//enble l_sensor
	if(ioctl(fd, LTR_IOCTL_SET_LFLAG, &l_flag)) {
		printf("can set l_flag\n");
		return -1;
	}

	while(1){
		l_data = 0;
		if(ioctl(fd, LTR_IOCTL_GET_LDATA, &l_data)) {
			printf("can not get l_data\n");
			return -1;
		}
		printf("l_data = %d\n", l_data);
		sleep(2);
	}

	return 0;
}








