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
#include <dirent.h>
#include <sys/poll.h>
#include <linux/input.h>

int main(void)
{
	int fd;
	struct input_absinfo absinfo_x;
	struct input_absinfo absinfo_y;

	fd = open("/dev/input/event4", O_RDWR);
	if(fd < 0) {
		printf("can not open dev\n");
		return -1;
   	}
 
	if(ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), &absinfo_x)) {
		printf("can not get absinfo\n");
		return -1;
	}
	
	if(ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), &absinfo_y)) {
		printf("can not get absinfo\n");
		return -1;
	}

	printf("absinfo_x.minimum = %d\nabsinfo_x.maximum = %d\n", absinfo_x.minimum, absinfo_x.maximum);
	printf("absinfo_y.minimum = %d\nabsinfo_y.maximum = %d\n", absinfo_y.minimum, absinfo_y.maximum);
	return 0;
}








