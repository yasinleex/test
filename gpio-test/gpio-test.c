
#include <fcntl.h>
#include <stdio.h>
#include <linux/ioctl.h>  //for _IOR

/**********************************/
#define GPIO_TEST_DEBUG 1
#define ENTER \
do{ if(GPIO_TEST_DEBUG) printf("[GPIO_TEST_DBG][%04d][%s]\n", __LINE__, __func__); }while(0)

#define PRINT_DBG(format,x...) \
do{ if(GPIO_TEST_DEBUG) printf("[GPIO_TEST_DBG][%04d] " format, __LINE__, ## x); }while(0)

#define PRINT_INFO(format,x...) \
do{ printf("[GPIO_TEST_INFO][%04d] " format, __LINE__, ## x); }while(0)

#define PRINT_WARN(format,x...) \
do{ printf("[GPIO_TEST_WARN][%04d] " format, __LINE__, ## x); }while(0)

#define PRINT_ERR(format,x...) \
do{ printf("[GPIO_TEST_ERR][%04d][%s] " format, __LINE__, __func__, ## x); }while(0)
/**********************************/

#define GPIO_TEST_IOCTL_MAGIC 0xAC
#define GPIO_TEST_IOCTL_GPIO_REQUEST       _IOR(GPIO_TEST_IOCTL_MAGIC, 1, unsigned long)
#define GPIO_TEST_IOCTL_GPIO_FREE          _IOR(GPIO_TEST_IOCTL_MAGIC, 2, unsigned long)
#define GPIO_TEST_IOCTL_GPIO_SET_DIRECTION _IOW(GPIO_TEST_IOCTL_MAGIC, 3, unsigned long)
#define GPIO_TEST_IOCTL_GPIO_SET_VAL       _IOW(GPIO_TEST_IOCTL_MAGIC, 4, unsigned long)
#define GPIO_TEST_IOCTL_GPIO_GET_VAL       _IOW(GPIO_TEST_IOCTL_MAGIC, 5, unsigned long)
#define GPIO_TEST_IOCTL_IRQ_REQUEST        _IOW(GPIO_TEST_IOCTL_MAGIC, 6, unsigned long)
#define GPIO_TEST_IOCTL_IRQ_FREE           _IOW(GPIO_TEST_IOCTL_MAGIC, 7, unsigned long)

#define DEV_NAME "/dev/gpio_test_dev"
#define GPIO_MAX 512
#define GPIO_MIN 0
#define INPUT 1
#define OUTPUT 0

struct gpio_test {
	int num;
	int direction;
	int val;
	char name[100];
};

static int fd = -1;
static unsigned char  requested[GPIO_MAX] = {0};
static unsigned char  direction[GPIO_MAX] = {INPUT};//default value is INPUT

int dev_open(void)
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

int gpio_request(struct gpio_test *this_gpio)
{
	ENTER;
	if (fd < 0) {
		PRINT_ERR("Device file is not opened\n");
		return -1;
	}

	if (this_gpio->num >= GPIO_MAX || this_gpio->num < GPIO_MIN) {
		PRINT_ERR("the valid gpio_num should be %d~%d\n", GPIO_MIN, GPIO_MAX);
		return -1;
	}

	if(1 == requested[this_gpio->num]) {
		PRINT_ERR("the GPIO_%d is requested already, you can NOT request it again!!!\n", this_gpio->num);
		return 0;
	}

	if (ioctl(fd, GPIO_TEST_IOCTL_GPIO_REQUEST, this_gpio) < 0) {
		PRINT_ERR("ioctl error\n");
		return -1;
	}

	requested[this_gpio->num] = 1;
	return 0;
}

int gpio_free(struct gpio_test *this_gpio)
{
	ENTER;
	if (fd < 0) {
		PRINT_ERR("Device file is not opened\n");
		return -1;
	}

	if (this_gpio->num >= GPIO_MAX || this_gpio->num < GPIO_MIN) {
		PRINT_ERR("the valid gpio_num should be %d~%d\n", GPIO_MIN, GPIO_MAX);
		return -1;
	}

	if(1 != requested[this_gpio->num]) {
		PRINT_ERR("the GPIO_%d is freed already, you can NOT free it again!!!\n", this_gpio->num);
		return 0;
	}

	if (ioctl(fd, GPIO_TEST_IOCTL_GPIO_FREE, this_gpio) < 0) {
		PRINT_ERR("ioctl error\n");
		return -1;
	}

	requested[this_gpio->num] = 0;
	return 0;
}

int gpio_set_direction(struct gpio_test *this_gpio)
{
	ENTER;
	if (fd < 0) {
		PRINT_ERR("Device file is not opened\n");
		return -1;
	}

	if (this_gpio->num >= GPIO_MAX || this_gpio->num < GPIO_MIN) {
		PRINT_ERR("the valid gpio_num should be %d~%d\n", GPIO_MIN, GPIO_MAX);
		return -1;
	}

	if(INPUT != this_gpio->direction && OUTPUT != this_gpio->direction) {
		PRINT_ERR("the direction of a gpio must be \"%d\" or \"%d\" !!!\n", INPUT, OUTPUT);
		return -1;
	}

	if(1 != requested[this_gpio->num]) {
		PRINT_ERR("the GPIO_%d is unrequested, you can NOT set direction for it!!!\n", this_gpio->num);
		return -1;
	}

	if (ioctl(fd, GPIO_TEST_IOCTL_GPIO_SET_DIRECTION, this_gpio) < 0) {
		PRINT_ERR("ioctl error\n");
		return -1;
	}

	if(INPUT == this_gpio->direction)
		direction[this_gpio->num] = INPUT;
	else if(OUTPUT == this_gpio->direction)
		direction[this_gpio->num] = OUTPUT;

	return 0;
}

int gpio_set_val(struct gpio_test *this_gpio)
{
	ENTER;
	if (fd < 0) {
		PRINT_ERR("Device file is not opened\n");
		return -1;
	}

	if (this_gpio->num >= GPIO_MAX || this_gpio->num < GPIO_MIN) {
		PRINT_ERR("the valid gpio_num should be %d~%d\n", GPIO_MIN, GPIO_MAX);
		return -1;
	}

	if(1 != requested[this_gpio->num]) {
		PRINT_ERR("the GPIO_%d is unrequested, you can NOT set direction for it!!!\n", this_gpio->num);
		return -1;
	}

	if(OUTPUT != direction[this_gpio->num]) {
		PRINT_ERR("the GPIO_%d is INPUT, you can NOT set value for a input pin!!!\n", this_gpio->num);
		return -1;
	}

	if (ioctl(fd, GPIO_TEST_IOCTL_GPIO_SET_VAL, this_gpio) < 0) {
		PRINT_ERR("ioctl error\n");
		return -1;
	}

	return 0;
}

int gpio_get_val(struct gpio_test *this_gpio)
{
	ENTER;
	if (fd < 0) {
		PRINT_ERR("Device file is not opened\n");
		return -1;
	}

	if (this_gpio->num >= GPIO_MAX || this_gpio->num < GPIO_MIN) {
		PRINT_ERR("the valid gpio_num should be %d~%d\n", GPIO_MIN, GPIO_MAX);
		return -1;
	}

	if(1 != requested[this_gpio->num]) {
		PRINT_ERR("the GPIO_%d is unrequested, you can NOT set direction for it!!!\n", this_gpio->num);
		return -1;
	}
#if 0
	if(INPUT != direction[this_gpio->num]) {
		PRINT_ERR("the GPIO_%d is OUTPUT, you can NOT get value for a OUTPUT pin!!!\n", this_gpio->num);
		return -1;
	}
#endif
	if (ioctl(fd, GPIO_TEST_IOCTL_GPIO_GET_VAL, this_gpio) < 0) {
		PRINT_ERR("ioctl error\n");
		return -1;
	}

	return 0;
}

int gpio_free_all(void)
{
	int i = 0;
	int ret = -1;
	struct gpio_test this_gpio;
	ENTER;

	if (fd < 0) {
		PRINT_ERR("Device file is not opened\n");
		return -1;
	}

	for(i=0; i<GPIO_MAX; i++){
		this_gpio.num = i;
		if(1 == requested[i]) {
			ret = gpio_free(&this_gpio);
			if(0 != ret)
				PRINT_ERR("gpio_free failed: GPIO_%d\n", this_gpio.num);
			else
				PRINT_INFO("GPIO_%d freed success\n", this_gpio.num);
		}
	}

	return 0;
}


int irq_request(struct gpio_test *this_gpio)
{
	ENTER;
	if (fd < 0) {
		PRINT_ERR("Device file is not opened\n");
		return -1;
	}

	if (this_gpio->num >= GPIO_MAX || this_gpio->num < GPIO_MIN) {
		PRINT_ERR("the valid gpio_num should be %d~%d\n", GPIO_MIN, GPIO_MAX);
		return -1;
	}

	if(1 != requested[this_gpio->num]) {
		PRINT_ERR("the GPIO_%d is unrequested, you can NOT set direction for it!!!\n", this_gpio->num);
		return -1;
	}

	if(INPUT != direction[this_gpio->num]) {
		PRINT_ERR("the GPIO_%d is OUTPUT, the irq line must be a input pin!!!\n", this_gpio->num);
		return -1;
	}

	if (ioctl(fd, GPIO_TEST_IOCTL_IRQ_REQUEST, this_gpio) < 0) {
		PRINT_ERR("ioctl error\n");
		return -1;
	}

	return 0;
}

int irq_free(struct gpio_test *this_gpio)
{
	ENTER;
	if (fd < 0) {
		PRINT_ERR("Device file is not opened\n");
		return -1;
	}

	if (this_gpio->num >= GPIO_MAX || this_gpio->num < GPIO_MIN) {
		PRINT_ERR("the valid gpio_num should be %d~%d\n", GPIO_MIN, GPIO_MAX);
		return -1;
	}

	if(1 != requested[this_gpio->num]) {
		PRINT_ERR("the GPIO_%d is unrequested, you can NOT set direction for it!!!\n", this_gpio->num);
		return -1;
	}

	if(INPUT != direction[this_gpio->num]) {
		PRINT_ERR("the GPIO_%d is OUTPUT, the irq line must be a input pin!!!\n", this_gpio->num);
		return -1;
	}

	if (ioctl(fd, GPIO_TEST_IOCTL_IRQ_FREE, this_gpio) < 0) {
		PRINT_ERR("ioctl error\n");
		return -1;
	}

	return 0;
}

int main(void)
{
	int val = 0;
	int ret = -1;
	struct gpio_test this_gpio;
	ENTER;

	ret = dev_open();
	if(ret < 0) {
		PRINT_ERR("can not open \"%s\"\n", DEV_NAME);
		return -1;
	}

	memset(&this_gpio, 0, sizeof(this_gpio));
	while(1){
		printf("\n\n\n=====================================\n");
		printf("    0. Exit\n");
		printf("    1. GPIO SELECT & REQUEST\n");
		printf("    2. GPIO SET DIRECTION\n");
		printf("    3. GPIO SET VALUE\n");
		printf("    4. GPIO GET VALUE\n");
		printf("    5. GPIO FREE\n");
		printf("    6. IRQ REQUEST\n");
		printf("    7. IRQ FREE\n");
		printf("=====================================\n");
		printf("please input your choice number: ");
		scanf("%d", &val);
		printf("_____________________________\n");

		switch(val) {
			case 0:
				gpio_free_all();
				PRINT_INFO("Exit success\n\n");
				dev_close();
				return 0;
			case 1:
				printf("GPIO SELECT & REQUEST: please input gpio number(%d~%d):", GPIO_MIN, GPIO_MAX);
				scanf("%d", &val);
				if (val >= GPIO_MAX || val < GPIO_MIN) {
					PRINT_ERR("invalid input value\n");
					break;
				}
				this_gpio.num = val;
				if(1 != requested[this_gpio.num]) {
					printf("GPIO SELECT & REQUEST: please input gpio name:");
					scanf("%s", this_gpio.name);
					ret = gpio_request(&this_gpio);
					if(0 != ret) {
						PRINT_ERR("gpio_request failed\n");
						break;
					}
				}
				PRINT_INFO("GPIO SELECT & REQUEST: INFO\n");
				PRINT_INFO("this_gpio.num = %d\n", this_gpio.num);
				PRINT_INFO("this_gpio.name = %s\n", this_gpio.name);
				PRINT_INFO("this_gpio.direction = %d\n", this_gpio.direction);
				PRINT_INFO("this_gpio.val = %d\n", this_gpio.val);
				break;
			case 2:
				printf("GPIO SET DIRECTION: please input directon for GPIO_%d(%d==input, %d=output):", this_gpio.num, INPUT, OUTPUT);
				scanf("%d", &val);
				if (INPUT != val && OUTPUT != val) {
					PRINT_ERR("invalid input value\n");
					break;
				}
				this_gpio.direction = val;
				PRINT_INFO("GPIO SET DIRECTION: INFO\n");
				PRINT_INFO("this_gpio.num = %d\n", this_gpio.num);
				PRINT_INFO("this_gpio.name = %s\n", this_gpio.name);
				PRINT_INFO("this_gpio.direction = %d\n", this_gpio.direction);
				PRINT_INFO("this_gpio.val = %d\n", this_gpio.val);
				ret = gpio_set_direction(&this_gpio);
				if(0 != ret) {
					PRINT_ERR("gpio_set_direction failed\n");
					break;
				}
				break;
			case 3:
				printf("GPIO SET VALUE: please input output value for GPIO_%d(0==LOW, 1==HIGH):", this_gpio.num);
				scanf("%d", &val);
				if (0 != val && 1 != val) {
					PRINT_ERR("invalid input value\n");
					break;
				}
				this_gpio.val= val;
				PRINT_INFO("GPIO SET VALUE: INFO\n");
				PRINT_INFO("this_gpio.num = %d\n", this_gpio.num);
				PRINT_INFO("this_gpio.name = %s\n", this_gpio.name);
				PRINT_INFO("this_gpio.direction = %d\n", this_gpio.direction);
				PRINT_INFO("this_gpio.val = %d\n", this_gpio.val);
				if(OUTPUT == this_gpio.direction) {
					ret = gpio_set_val(&this_gpio);
					if(0 != ret) {
						PRINT_ERR("gpio_set_val failed\n");
						break;
					}
				}
				break;
			case 4:
				ret = gpio_get_val(&this_gpio);
				if(0 != ret) {
					PRINT_ERR("gpio_get_val failed\n");
					break;
				}
				PRINT_INFO("GPIO GET VALUE: INFO\n");
				PRINT_INFO("this_gpio.num = %d\n", this_gpio.num);
				PRINT_INFO("this_gpio.name = %s\n", this_gpio.name);
				PRINT_INFO("this_gpio.direction = %d\n", this_gpio.direction);
				PRINT_INFO("this_gpio.val = %d\n", this_gpio.val);
				break;
			case 5:
				printf("GPIO FREE: please input gpio number(%d~%d):", GPIO_MIN, GPIO_MAX);
				scanf("%d", &val);
				if (val >= GPIO_MAX || val < GPIO_MIN) {
					PRINT_ERR("invalid input value\n");
					break;
				}
				this_gpio.num = val;
				if(1 != requested[this_gpio.num]) {
					PRINT_ERR("the GPIO_%d is unrequested, you can NOT get value for it!!!\n", this_gpio.num);
					break;
				}
				ret = gpio_free(&this_gpio);
				if(0 != ret) {
					PRINT_ERR("gpio_free failed\n");
					break;
				}
				break;
			case 6:
				printf("IRQ REQUEST: please input gpio number(%d~%d):", GPIO_MIN, GPIO_MAX);
				scanf("%d", &val);
				if (val >= GPIO_MAX || val < GPIO_MIN) {
					PRINT_ERR("invalid input value\n");
					break;
				}
				this_gpio.num = val;
				if(1 != requested[this_gpio.num]) {
					PRINT_ERR("the GPIO_%d is unrequested, you can NOT use it as INT!!!\n", this_gpio.num);
					break;
				}
				ret = irq_request(&this_gpio);
				if(0 != ret) {
					PRINT_ERR("irq_request failed\n");
					break;
				}
				break;
			case 7:
				printf("IRQ FREE: please input gpio number(%d~%d):", GPIO_MIN, GPIO_MAX);
				scanf("%d", &val);
				if (val >= GPIO_MAX || val < GPIO_MIN) {
					PRINT_ERR("invalid input value\n");
					break;
				}
				this_gpio.num = val;
				if(1 != requested[this_gpio.num]) {
					PRINT_ERR("the GPIO_%d is unrequested, you can NOT free it!!!\n", this_gpio.num);
					break;
				}
				ret = irq_free(&this_gpio);
				if(0 != ret) {
					PRINT_ERR("irq_free failed\n");
					break;
				}
				break;
			default:
				break;
		};
	};

	dev_close();
	PRINT_INFO("Exit success\n\n");
	return 0;
}




