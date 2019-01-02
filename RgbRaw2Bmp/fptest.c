#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>




/**********************************/
static int debug_level = 1;

#define ENTER \
do{ if (debug_level >= 1) {printf("[%s_DBG][%04d] [%s]\n", __FILE__, __LINE__, __func__);} }while(0)

#define PRINT_DBG(format,x...) \
do{ if (debug_level >= 1) {printf("[%s_DBG][%04d] " format, __FILE__, __LINE__, ## x);} }while(0)

#define PRINT_INFO(format,x...) \
do{ printf("[%s_INFO][%04d] " format, __FILE__, __LINE__, ## x); }while(0)

#define PRINT_WARN(format,x...) \
do{ printf("[%s_WARN][%04d] " format, __FILE__, __LINE__, ## x); }while(0)

#define PRINT_ERR(format,x...) \
do{ printf("[%s_ERR][%04d] [%s] " format, __FILE__, __LINE__, __func__, ## x); }while(0)
/**********************************/
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

int main(int argc, char **argv)
{
    int ret = -1;
    int opt;
    char* file_raw = NULL;
    char* file_bmp = NULL;
    FILE* fp_raw = NULL;
    FILE* fp_bmp = NULL;

    u32 width = 0;
    u32 height = 0;
    u8 raw_rgb_type = 0;

    ENTER;
    PRINT_INFO("argc=%d\n", argc);
    while ((opt = getopt(argc, argv, "i:o:w:h:t:")) != -1) {
        switch (opt) {
        case 'i':
            file_raw = optarg;
            PRINT_INFO("input file: %s\n", file_raw);
            break;
        case 'o':
            file_bmp = optarg;
            PRINT_INFO("output file: %s\n", file_bmp);
            break;
        case 'w':
            width = atoi(optarg);
            PRINT_INFO("width: %u\n", width);
            break;
        case 'h':
            height = atoi(optarg);
            PRINT_INFO("height: %u\n", height);
            break;
        case 't':
            raw_rgb_type = (u8)atoi(optarg);
            PRINT_INFO("raw_rgb_type: %u\n", raw_rgb_type);
            break;
        default:
            PRINT_INFO("set invalid option -%c\n", opt);
            return -EINVAL;
        }
    }

    fp_raw = fopen(file_raw, "rb");
    if (NULL == fp_raw) {
        printf("Can Not open: %s\n", file_raw);
        return;
    }

    fp_bmp = fopen(file_bmp, "wb");
    if (NULL == fp_bmp) {
        printf("Can Not open: %s\n", file_bmp);
        return;
    }


    u8 buf_in[4] = {0};
    u8 buf_out[3] = {0};
    u8 read_size = 0;


u8 buf_t[3] = {0};

    fseek(fp_raw, 0, SEEK_SET);
    fseek(fp_bmp, 0, SEEK_SET);

    ret = fread(buf_t, 3, 1, fp_raw);
    PRINT_INFO("ret=%d\n", ret);

    ret = fwrite(buf_t, 3, 1, fp_bmp);
    PRINT_INFO("ret=%d\n", ret);


ret = fread(buf_t, 3, 1, fp_raw);
PRINT_INFO("ret=%d\n", ret);


ret = fwrite(buf_t, 3, 1, fp_bmp);
PRINT_INFO("ret=%d\n", ret);


fclose(fp_raw);
	fclose(fp_bmp);
    return 0;
}



