#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>

/**********************************/
#define UART_TEST_DEBUG 1
#define ENTER \
do{ if(UART_TEST_DEBUG) printf("[UART_TEST_DBG][%04d][%s]\n", __LINE__, __func__); }while(0)

#define PRINT_DBG(format,x...) \
do{ if(UART_TEST_DEBUG) printf("[UART_TEST_DBG][%04d] " format, __LINE__, ## x); }while(0)

#define PRINT_INFO(format,x...) \
do{ printf("[UART_TEST_INFO][%04d] " format, __LINE__, ## x); }while(0)

#define PRINT_WARN(format,x...) \
do{ printf("[UART_TEST_WARN][%04d] " format, __LINE__, ## x); }while(0)

#define PRINT_ERR(format,x...) \
do{ printf("[UART_TEST_ERR][%04d][%s] " format, __LINE__, __func__, ## x); }while(0)
/**********************************/

struct speed_map {
    const char *string;      /* ASCII representation. */
    speed_t speed;           /* Internal form. */
    unsigned long int value; /* Numeric value. */
};

static struct speed_map speeds[] = {
    {"0", B0, 0},
    {"50", B50, 50},
    {"75", B75, 75},
    {"110", B110, 110},
    {"134", B134, 134},
    {"134.5", B134, 134},
    {"150", B150, 150},
    {"200", B200, 200},
    {"300", B300, 300},
    {"600", B600, 600},
    {"1200", B1200, 1200},
    {"1800", B1800, 1800},
    {"2400", B2400, 2400},
    {"4800", B4800, 4800},
    {"9600", B9600, 9600},
    {"19200", B19200, 19200},
    {"38400", B38400, 38400},
    {"exta", B19200, 19200},
    {"extb", B38400, 38400},
#ifdef B57600
    {"57600", B57600, 57600},
#endif
#ifdef B115200
    {"115200", B115200, 115200},
#endif
#ifdef B230400
    {"230400", B230400, 230400},
#endif
#ifdef B460800
    {"460800", B460800, 460800},
#endif
#ifdef B500000
    {"500000", B500000, 500000},
#endif
#ifdef B576000
    {"576000", B576000, 576000},
#endif
#ifdef B921600
    {"921600", B921600, 921600},
#endif
#ifdef B1000000
    {"1000000", B1000000, 1000000},
#endif
#ifdef B1152000
    {"1152000", B1152000, 1152000},
#endif
#ifdef B1500000
    {"1500000", B1500000, 1500000},
#endif
#ifdef B2000000
    {"2000000", B2000000, 2000000},
#endif
#ifdef B2500000
    {"2500000", B2500000, 2500000},
#endif
#ifdef B3000000
    {"3000000", B3000000, 3000000},
#endif
#ifdef B3500000
    {"3500000", B3500000, 3500000},
#endif
#ifdef B4000000
    {"4000000", B4000000, 4000000},
#endif
    {NULL, 0, 0}
};

static unsigned long int baud_to_value(speed_t speed)
{
    int i;

    for (i = 0; speeds[i].string != NULL; ++i)
        if (speed == speeds[i].speed)
            return speeds[i].value;

    return 0;
}

static int uart_open_port(const char *dev_name)
{
    int fd; /* File descriptor for the port */
    fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == fd) {
        PRINT_ERR("Unable to open uart port %s.\n", dev_name);
        return -1;
    }

    if (isatty(fd) == 0) {
        PRINT_ERR("This is not a tty device.\n");
    }

    PRINT_INFO("\"%s\" opened (fd=%d)\n", dev_name, fd);
    return fd;
}

static int uart_set_port(int fd, int baud, int databits, char* parity,
                         int stopbits, char* flowcontrol)
{
    struct termios new_ios, old_ios;

    ENTER;
    if (tcgetattr(fd, &new_ios) != 0) {
        PRINT_ERR("Save the terminal error.\n");
        return -1;
    }

    bzero(&old_ios, sizeof(struct termios));
    old_ios = new_ios;

    tcflush(fd, TCIOFLUSH);
    new_ios.c_cflag |= CLOCAL | CREAD;
    new_ios.c_cflag &= ~CSIZE;

    if(0 != databits) {
        switch (databits) {
        case 5:
            new_ios.c_cflag |= CS5;
            break;
        case 6:
            new_ios.c_cflag |= CS6;
            break;
        case 7:
            new_ios.c_cflag |= CS7;
            break;
        case 8:
            new_ios.c_cflag |= CS8;
            break;
        default:
            PRINT_INFO("databits set invalid option -%d\n", databits);
            break;
        }
        PRINT_INFO("databits set - %d\n", databits);
    }

    if(0 != baud) {
        switch (baud) {
        case 2400:
            cfsetispeed(&new_ios, B2400);
            cfsetospeed(&new_ios, B2400);
            break;
        case 4800:
            cfsetispeed(&new_ios, B4800);
            cfsetospeed(&new_ios, B4800);
            break;
        case 9600:
            cfsetispeed(&new_ios, B9600);
            cfsetospeed(&new_ios, B9600);
            break;
        case 19200:
            cfsetispeed(&new_ios, B19200);
            cfsetospeed(&new_ios, B19200);
            break;
        case 115200:
            cfsetispeed(&new_ios, B115200);
            cfsetospeed(&new_ios, B115200);
            break;
        case 460800:
            cfsetispeed(&new_ios, B460800);
            cfsetospeed(&new_ios, B460800);
            break;
        default:
            PRINT_INFO("parity set invalid option -%d\n", baud);
            break;
        }
        PRINT_INFO("speed set - %d\n", baud);
    }

    if(NULL != parity) {
        switch (*parity) {
        case 'o':
        case 'O':
            new_ios.c_cflag |= PARENB;
            new_ios.c_cflag |= PARODD;
            new_ios.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'e':
        case 'E':
            new_ios.c_iflag |= (INPCK | ISTRIP);
            new_ios.c_cflag |= PARENB;
            new_ios.c_cflag &= ~PARODD;
            break;
        case 'n':
        case 'N':
            new_ios.c_cflag &= ~PARENB;
            new_ios.c_iflag &= ~INPCK;
            break;
        default:
            PRINT_INFO("parity set invalid option -%c\n", *parity);
            break;
        }
        PRINT_INFO("parity set - %c\n", *parity);
    }

    if(NULL != flowcontrol) {
        switch(*flowcontrol) {
        case 'y':
            new_ios.c_iflag = IXON | IXOFF | IXANY;
            break;
        case 'n':
            new_ios.c_iflag &= ~(IXON | IXOFF | IXANY);
            break;
        default:
            PRINT_INFO("flow control set invalid option -%c\n", *flowcontrol);
            break;
        }
        PRINT_INFO("flow control set - %c\n", *flowcontrol);
    }

    if (stopbits == 1)
        new_ios.c_cflag &= ~CSTOPB;
    else if (stopbits == 2)
        new_ios.c_cflag |= CSTOPB;

    /*No hardware control*/
    new_ios.c_cflag &= ~CRTSCTS;

    /*delay time set */
    new_ios.c_cc[VTIME] = 0;
    new_ios.c_cc[VMIN] = 0;

    /*raw model*/
    new_ios.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    new_ios.c_oflag &= ~OPOST;

    new_ios.c_iflag &= ~(INLCR | IGNCR | ICRNL);
    new_ios.c_iflag &= ~(ONLCR | OCRNL);

    new_ios.c_oflag &= ~(INLCR | IGNCR | ICRNL);
    new_ios.c_oflag &= ~(ONLCR | OCRNL);

    tcflush(fd, TCIOFLUSH);
    if (tcsetattr(fd, TCSANOW, &new_ios) != 0) {
        PRINT_ERR("Set the terminal error.\n");
        tcsetattr(fd, TCSANOW, &old_ios);
        return -1;
    }

    return 0;
}

#define BUF_SIZE 512
#define SELECT_DO_INTERVAL 50000
#define SELECT_BLOCK_US 0

static int uart_read_port(int fd)
{
    unsigned char buf[BUF_SIZE];
    int ret = -1;
    int i = 0;
    fd_set rfds;
    struct timeval timeout ;

    ENTER;
    timeout.tv_sec = 0;
    timeout.tv_usec = SELECT_BLOCK_US;

    while(1) {
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        ret = select(fd+1, &rfds, NULL, NULL, &timeout);
        if(ret == -1) {
            PRINT_ERR("select function return error!!!\n");
            return -EINVAL;
        } else if(ret) {
            memset(buf, '\0', BUF_SIZE);
            ret = read(fd, buf, BUF_SIZE);
            if(ret > 0) {
                for(i=0; i<ret; i++) {
                    putchar(buf[i]);
                }
            } else {
                PRINT_INFO("\n\nread return error, ret=%d\n\n", ret);
            }
        } else {
            usleep(SELECT_DO_INTERVAL);
        }
    }
    return 0;
}

static int uart_write_port(int fd)
{
    unsigned char buf[BUF_SIZE];
    unsigned char a = 'A' - 1;
    int i = 0;
    int pos = 0;
    int count = 0;
    int ret = -1;
    fd_set wfds;
    struct timeval timeout ;

    ENTER;
    timeout.tv_sec = 0;
    timeout.tv_usec = SELECT_BLOCK_US;

    while(1) {
        FD_ZERO(&wfds);
        FD_SET(fd, &wfds);
        ret = select(fd+1, NULL, &wfds, NULL, &timeout);
        if(ret == -1) {
            PRINT_ERR("select function return error!!!\n");
            return -EINVAL;
        } else if(ret) {
            if(0 == pos) {
                a++;
                if(a > 'Z')
                    a = 'A';
                memset(buf, a, BUF_SIZE);
                count++;
                sprintf(buf, "---count=%08d---", count);
                buf[BUF_SIZE-2] = '\r';
                buf[BUF_SIZE-1] = '\n';
            }
            ret = write(fd, buf+pos, BUF_SIZE-pos);
            if(ret<0) {
                PRINT_ERR("\n\nwrite return error, ret=%d\n\n", ret);
            } else {
                pos += ret;
                if(pos >= BUF_SIZE) {
                    PRINT_INFO("write success, count=%d\n", count);
                    pos = 0;
                } else {
                    PRINT_INFO("write pending, count=%d pos=%d\n", count, pos);
                }
            }
        } else {
            usleep(SELECT_DO_INTERVAL);
        }
    }
    return 0;
}

static int do_read(int argc, char **argv)
{
    int opt, err, fd;
    int baud = 0, databits = 0, stopbits = 0;
    char *parity = NULL, *flowcontrol = NULL, *uartname = NULL;

    ENTER;
    while ((opt = getopt(argc, argv, "n:b:p:d:s:f:")) != -1) {
        switch (opt) {
        case 'n':
            uartname = optarg;
            PRINT_INFO("%s\n", uartname);
            break;
        case 'b':
            baud = atoi(optarg);
            break;
        case 'p':
            parity = optarg;
            break;
        case 'd':
            databits = atoi(optarg);
            break;
        case 's':
            stopbits = atoi(optarg);
            break;
        case 'f':
            flowcontrol = optarg;
            break;
        default:
            PRINT_INFO("utest_uart set invalid option -%c\n", opt);
            return -EINVAL;
        }
    }

    fd = uart_open_port(uartname);
    if(-1 == fd)
        return -1;

    err = uart_set_port(fd, baud, databits, parity, stopbits, flowcontrol);
    if (0 != err) {
        PRINT_ERR("uart_set_port error!\n");
        close(fd);
        return -EINVAL;
    }

    err = uart_read_port(fd);
    if (0 != err) {
        PRINT_ERR("uart_read_port error!\n");
        close(fd);
        return -EINVAL;
    }

    close(fd);
    return 0;
}

static int do_write(int argc, char **argv)
{
    int opt, err, fd;
    int baud = 0, databits = 0, stopbits = 0;
    char *parity = NULL, *flowcontrol = NULL, *uartname = NULL;

    ENTER;
    while ((opt = getopt(argc, argv, "n:b:p:d:s:f:")) != -1) {
        switch (opt) {
        case 'n':
            uartname = optarg;
            PRINT_INFO("%s\n", uartname);
            break;
        case 'b':
            baud = atoi(optarg);
            break;
        case 'p':
            parity = optarg;
            break;
        case 'd':
            databits = atoi(optarg);
            break;
        case 's':
            stopbits = atoi(optarg);
            break;
        case 'f':
            flowcontrol = optarg;
            break;
        default:
            PRINT_INFO("utest_uart set invalid option -%c\n", opt);
            return -EINVAL;
        }
    }

    fd = uart_open_port(uartname);
    if(-1 == fd)
        return -1;

    err = uart_set_port(fd, baud, databits, parity, stopbits, flowcontrol);
    if (0 != err) {
        PRINT_ERR("uart_set_port error!\n");
        close(fd);
        return -EINVAL;
    }

    err = uart_write_port(fd);
    if (0 != err) {
        PRINT_ERR("uart_write_port error!\n");
        close(fd);
        return -EINVAL;
    }

    close(fd);
    return 0;
}

static void usage(void)
{
    printf("Usage:\n");
    printf("utest_uart read  -n DeviceName [-b Baud][-p Parity][-d DataBits][-s StopBits][-f FlowControl]\n");
    printf("utest_uart write -n DeviceName [-b Baud][-p Parity][-d DataBits][-s StopBits][-f FlowControl]\n");
    printf("example for read:   \" sudo ./uart-test  read  -n /dev/ttyS3 -b 115200 -d 8 \"\n");
    printf("example for write:  \" sudo ./uart-test  write -n /dev/ttyS3 -b 115200 -d 8 \"\n");
}

int main(int argc, char **argv)
{
    char *cmd;
    int ret = -EINVAL;
    printf("----------------------------utest_uart begin-----------------------\n");
    if (argc < 2) {
        usage();
        return ret;
    }

    cmd = argv[1];
    argc--;
    argv++;

    PRINT_INFO("utest_uart -- %s \n", cmd);
    if (strcmp(cmd, "read") == 0)
        ret = do_read(argc, argv);
    else if (strcmp(cmd, "write") == 0)
        ret = do_write(argc, argv);
    else
        usage();

    if (ret == -EINVAL)
        usage();

    printf("----------------------------utest_uart end-------------------------\n");
    return ret;
}

