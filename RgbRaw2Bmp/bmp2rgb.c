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

/*
sizeof(char)=1
sizeof(short)=2
sizeof(int)=4
sizeof(long)=8
sizeof(long long)=8
sizeof(unsigned char)=1
sizeof(unsigned short)=2
sizeof(unsigned int)=4
sizeof(unsigned long)=8
sizeof(unsigned long long)=8
*/

#define UNKNOW 0
#define RGBA8888 1
#define RGB888 2
#define RGB666 3
#define RGB565 4


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef struct {
    u8 id[2];
    u8 file_size[4];
    u8 reserved[4];
    u8 bmp_data_offset[4];
} bmp_file_header;

typedef struct {
    u8 this_header_size[4];
    u8 width[4];
    u8 height[4];
    u8 planes[2];
    u8 bits_per_pixel[2];
    u8 compression[4];
    u8 bmp_data_size[4];
    u8 h_resolution[4];
    u8 v_resolution[4];
    u8 used_colors[4];
    u8 important_colors[4];
} bmp_info_header;

void type_check(void)
{
    printf("sizeof(char)=%ld\n", sizeof(char));
    printf("sizeof(short)=%ld\n", sizeof(short));
    printf("sizeof(int)=%ld\n", sizeof(int));
    printf("sizeof(long)=%ld\n", sizeof(long));
    printf("sizeof(long long)=%ld\n", sizeof(long long));

    printf("sizeof(unsigned char)=%lu\n", sizeof(unsigned char));
    printf("sizeof(unsigned short)=%lu\n", sizeof(unsigned short));
    printf("sizeof(unsigned int)=%lu\n", sizeof(unsigned int));
    printf("sizeof(unsigned long)=%lu\n", sizeof(unsigned long));
    printf("sizeof(unsigned long long)=%lu\n", sizeof(unsigned long long));

    printf("sizeof(u8)=%ld\n", sizeof(u8));
    printf("sizeof(u16)=%ld\n", sizeof(u16));
    printf("sizeof(u32)=%ld\n", sizeof(u32));
    printf("sizeof(u64)=%ld\n", sizeof(u64));
}

long get_file_size(FILE *fp)
{
    long size = 0;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    return size;
}

int set_bmp_header(
    FILE *fp_raw,
    u8 raw_rgb_type,
    u32 width, //pixels per line
    u32 height, //pixels per column
    bmp_file_header* file_header,
    bmp_info_header* info_header)
{
    u32 raw_file_size = 0;
    u32 bmp_data_size = 0;
    u32 num_pixels = 0;
    u32 storage_line_size = 0;

    raw_file_size = get_file_size(fp_raw);
    PRINT_INFO("raw_file_size=%d\n", raw_file_size);

    switch (raw_rgb_type) {
    case RGBA8888:
        if (0 == raw_file_size % 4)
            num_pixels = raw_file_size / 4;
        else
            num_pixels = 0;
        break;
    case RGB888:
    case RGB666:
        if (0 == raw_file_size % 3)
            num_pixels = raw_file_size / 3;
        else
            num_pixels = 0;
        break;
    case RGB565:
        if (0 == raw_file_size % 2)
            num_pixels = raw_file_size / 2;
        else
            num_pixels = 0;
        break;
    case UNKNOW:
        num_pixels = 0;
        PRINT_ERR("raw_rgb_type=UNKNOW\n");
        return -1;
    };

    if (num_pixels > 0) {
        PRINT_INFO("num_pixels=%d\n", num_pixels);
    } else {
        PRINT_ERR("num_pixels=%d\n", num_pixels);
        return -1;
    }

    PRINT_INFO("width=%d height=%d\n", width, height);

    if ((num_pixels) != (width * height)) {
        PRINT_ERR("(num_pixels) != (width*height)\n");
        return -1;
    }

    if (0 != ((width * 3) % 4))
        storage_line_size = (4 - ((width * 3) % 4)) + (width * 3);
    else
        storage_line_size = width * 3;

    strncpy(file_header->id , "BM", sizeof(file_header->id));
    *(u32*)file_header->file_size = sizeof(bmp_file_header)
                                    + sizeof(bmp_info_header)
                                    + (storage_line_size * height);
    *(u32*)file_header->reserved = 0;
    *(u32*)file_header->bmp_data_offset = sizeof(bmp_file_header) + sizeof(bmp_info_header);

    *(u32*)info_header->this_header_size = sizeof(bmp_info_header);
    *(u32*)info_header->width = width;
    *(u32*)info_header->height = height;
    *(u16*)info_header->planes = 0x0001;
    *(u16*)info_header->bits_per_pixel = 24;
    *(u32*)info_header->compression = 0;
    *(u32*)info_header->bmp_data_size = storage_line_size * height;
    *(u32*)info_header->h_resolution = 0;
    *(u32*)info_header->v_resolution = 0;
    *(u32*)info_header->used_colors = 0;
    *(u32*)info_header->important_colors = 0;

    return 0;
}

int get_bmp_header(
    FILE *fp_bmp,
    bmp_file_header* file_header,
    bmp_info_header* info_header)
{
    int ret = -1;
    u32 file_size = 0;
    u32 storage_line_size = 0;
    u32 actual_line_size = 0;
    u8 buff[0x36] = {0};

    ret = fseek(fp_bmp, 0, SEEK_SET);
    if (0 != ret) {
        PRINT_ERR("fseek return error! ret=%d\n", ret);
        return -1;
    }
    ret = fread(buff, sizeof(buff), 1, fp_bmp);
    if (1 != ret) {
        PRINT_ERR("fread return error! ret=%d\n", ret);
        return -1;
    }

    memcpy(file_header, buff, sizeof(*file_header));
    memcpy(info_header, buff + sizeof(*file_header), sizeof(*info_header));

    PRINT_INFO("******* file_header *******\n");
    PRINT_INFO("file_header->id: \"%c%c\" (0x%04X)\n", file_header->id[0], file_header->id[1], *(u16*)file_header->id);
    PRINT_INFO("file_header->file_size: %d (0x%08X)\n", *(u32*)file_header->file_size, *(u32*)file_header->file_size);
    PRINT_INFO("file_header->reserved: %d (0x%08X)\n", *(u32*)file_header->reserved, *(u32*)file_header->reserved);
    PRINT_INFO("file_header->bmp_data_offset: %d (0x%08X)\n", *(u32*)file_header->bmp_data_offset, *(u32*)file_header->bmp_data_offset);
    PRINT_INFO("******* info_header *******\n");
    PRINT_INFO("info_header->this_header_size: %d (0x%08X)\n", *(u32*)info_header->this_header_size, *(u32*)info_header->this_header_size);
    PRINT_INFO("info_header->width: %d (0x%08X)\n", *(u32*)info_header->width, *(u32*)info_header->width);
    PRINT_INFO("info_header->height: %d (0x%08X)\n", *(u32*)info_header->height, *(u32*)info_header->height);
    PRINT_INFO("info_header->planes: %d (0x%04X)\n", *(u16*)info_header->planes, *(u16*)info_header->planes);
    PRINT_INFO("info_header->bits_per_pixel: %d (0x%04X)\n", *(u16*)info_header->bits_per_pixel, *(u16*)info_header->bits_per_pixel);
    PRINT_INFO("info_header->compression: %d (0x%08X)\n", *(u32*)info_header->compression, *(u32*)info_header->compression);
    PRINT_INFO("info_header->bmp_data_size: %d (0x%08X)\n", *(u32*)info_header->bmp_data_size, *(u32*)info_header->bmp_data_size);
    PRINT_INFO("info_header->h_resolution: %d (0x%08X)\n", *(u32*)info_header->h_resolution, *(u32*)info_header->h_resolution);
    PRINT_INFO("info_header->v_resolution: %d (0x%08X)\n", *(u32*)info_header->v_resolution, *(u32*)info_header->v_resolution);
    PRINT_INFO("info_header->used_colors: %d (0x%08X)\n", *(u32*)info_header->used_colors, *(u32*)info_header->used_colors);
    PRINT_INFO("info_header->important_colors: %d (0x%08X)\n", *(u32*)info_header->important_colors, *(u32*)info_header->important_colors);

    storage_line_size = (*(u32*)info_header->bmp_data_size) / (*(u32*)info_header->height);
    actual_line_size = (*(u32*)info_header->width) * 3;
    PRINT_INFO("storage_line_size=%d, actual_line_size=%d\n", storage_line_size, actual_line_size);

    file_size = get_file_size(fp_bmp);
    PRINT_INFO("file_size=%d\n", file_size);

    if (file_size == (sizeof(*file_header) + sizeof(*info_header) + storage_line_size * (*(u32*)info_header->height))) {
        PRINT_INFO("file_size check OK\n");
    } else {
        PRINT_ERR("file_size check FAILED!!!\n");
        return -1;
    }

    return 0;
}

int rgb_type_parse(char* type)
{
    if (0 == strcmp("RGBA8888", type)) {
        PRINT_INFO("raw rgb file type: RGBA8888\n");
        return RGBA8888;
    }

    if (0 == strcmp("RGB888", type)) {
        PRINT_INFO("raw rgb file type: RGB888\n");
        return RGB888;
    }

    if (0 == strcmp("RGB666", type)) {
        PRINT_INFO("raw rgb file type: RGB666\n");
        return RGB666;
    }

    if (0 == strcmp("RGB565", type)) {
        PRINT_INFO("raw rgb file type: RGB565\n");
        return RGB565;
    }

    PRINT_ERR("raw rgb file type: UNKNOW\n");
    return UNKNOW;
}

//line_id: counting from 0
int get_one_line(FILE *fp , u8* buff, u32 line_id, u32 hight, u32 size_of_1_line)
{
    int ret = -1;

    PRINT_INFO("reading line_No.%d\n", line_id);
    ret = fseek(fp, -(int)((hight - line_id) * size_of_1_line), SEEK_END);
    if (0 != ret) {
        PRINT_ERR("fseek return error! ret=%d\n", ret);
        return -1;
    }
    ret = fread(buff, size_of_1_line, 1, fp);
    if (1 != ret) {
        PRINT_ERR("fread return error! ret=%d\n", ret);
        return -1;
    }
    return 0;
}

int write_one_line(FILE *fp , u8* buff, u32 size_of_1_line)
{
    int ret = -1;
    ret = fwrite(buff, size_of_1_line, 1, fp );
    if (1 != ret) {
        PRINT_ERR("fwrite return error! ret=%d\n", ret);
        return -1;
    }
    return 0;
}

int storage_type_covert(u8* buff, u8 type)
{
    int ret = -1;
    u8 buf_in[4] = {0};
    u8 buf_out[3] = {0};

    while (1) {
        switch (type) {
        case 2:
            buf_out[0] = buf_in[0] & 0xF8;
            buf_out[1] = ((buf_in[0] << 5) & 0xE0) |  ((buf_in[1] >> 3) & 0x1C);
            buf_out[2] = buf_in[1] << 3;
            break;
        case 3:
            buf_out[0] = buf_in[0];
            buf_out[1] = buf_in[1];
            buf_out[2] = buf_in[2];
            break;
        case 4:
            buf_out[0] = buf_in[0];
            buf_out[1] = buf_in[1];
            buf_out[2] = buf_in[2];
            break;
        };
    };
}

void print_how_to_use(void)
{
    printf("Help:\n");
    printf(" -i : the input file to be converted (24bit BMP file)\n");
    printf(" -o: the output file (rgb888 RAW data)\n");
    printf(" -w: the width of the output picture (pixel), ignored when -i was set\n");
    printf(" -h: the height of the output picture (pixel), ignored when -i was set\n");
    //printf(" -t: the input file type(RGBA8888, RGB888, RGB666, RGB565)\n");
    printf("eg: bmp2rgb -i ~/pic.bmp -o ~/rgb.raw\n");
    printf("eg: bmp2rgb -o ~/rgb.raw -w 240 -h 320  (generate a raw rgb file filled with random data)\n");
}

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

    print_how_to_use();
    PRINT_INFO("argc=%d\n", argc);

    while ((opt = getopt(argc, argv, "i:o:w:h:")) != -1) {
        switch (opt) {
        case 'i':
            file_bmp = optarg;
            PRINT_INFO("input file: %s\n", file_bmp);
            break;
        case 'o':
            file_raw = optarg;
            PRINT_INFO("output file: %s\n", file_raw);
            break;
        case 'w':
            width = atoi(optarg);
            PRINT_INFO("width: %u\n", width);
            break;
        case 'h':
            height = atoi(optarg);
            PRINT_INFO("height: %u\n", height);
            break;
        default:
            PRINT_INFO("set invalid option -%c\n", opt);
            print_how_to_use();
            return -EINVAL;
        }
    }

    fp_raw = fopen(file_raw, "wb");
    if (NULL == fp_raw) {
        printf("Can Not open: %s\n", file_raw);
        print_how_to_use();
        return -1;
    }

    fp_bmp = fopen(file_bmp, "rb");
    if (NULL == fp_bmp) {
        printf("Can Not open: %s\n", file_bmp);
        fclose(fp_raw);
        print_how_to_use();
        return -1;
    }

    bmp_file_header file_header;
    bmp_info_header info_header;

    PRINT_INFO("sizeof(file_header)=%ld\n", sizeof(file_header));
    PRINT_INFO("sizeof(info_header)=%ld\n", sizeof(info_header));

    ret = get_bmp_header(fp_bmp, &file_header, &info_header);
    if (0 != ret) {
        PRINT_ERR("get_bmp_header ERROR\n");
        goto err_out;
    }
    height = (*(u32*)info_header.height);
    width = (*(u32*)info_header.width);
    PRINT_INFO("BMP hight=%d, width=%d\n", height, width);

    u32 storage_line_size = 0;
    u32 actual_line_size = 0;

    storage_line_size = (*(u32*)info_header.bmp_data_size) / height;
    actual_line_size = (*(u32*)info_header.width) * 3;
    PRINT_INFO("storage_line_size=%d, actual_line_size=%d\n", storage_line_size, actual_line_size);

    u8* line_buf = NULL;
    line_buf = (u8*)malloc(storage_line_size);
    if (NULL == line_buf) {
        PRINT_ERR("malloc ERROR\n");
        goto err_out;
    }

    int line_id = 0;

    fseek(fp_raw, 0, SEEK_SET);
    for (line_id = height - 1; line_id >= 0; line_id--) {
        memset(line_buf, 0, storage_line_size);
        get_one_line(fp_bmp, line_buf, line_id, height, storage_line_size);
        write_one_line(fp_raw, line_buf, actual_line_size);
    }

err_out:
    free(line_buf);
    fclose(fp_raw);
    fclose(fp_bmp);
    return 0;
}



