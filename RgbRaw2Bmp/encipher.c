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
    if (NULL == fp) {
        PRINT_ERR("fp is NULL\n");
        return size;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return size;
}

int get_bmp_header(
    FILE *fp_b,
    bmp_file_header* file_header,
    bmp_info_header* info_header)
{
    int ret = -1;
    u32 file_size = 0;
    u32 storage_line_size = 0;
    u32 actual_line_size = 0;
    u8 buff[0x36] = {0};

    ret = fseek(fp_b, 0, SEEK_SET);
    if (0 != ret) {
        PRINT_ERR("fseek return error! ret=%d\n", ret);
        return -1;
    }
    ret = fread(buff, sizeof(buff), 1, fp_b);
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

    file_size = get_file_size(fp_b);
    PRINT_INFO("file_size=%d\n", file_size);

    if (file_size == (sizeof(*file_header) + sizeof(*info_header) + storage_line_size * (*(u32*)info_header->height))) {
        PRINT_INFO("file_size check OK\n");
    } else {
        PRINT_ERR("file_size check FAILED!!!\n");
        return -1;
    }

    return 0;
}

static int get_insert_coordinates(FILE *fp,
                                  u32 header_offset,
                                  u32 storage_line_size,
                                  u32 actual_line_size,
                                  u32 line,
                                  u32 insert_bytes_per_line,
                                  u32 magic_val)
{
    int ret = -1;
    u32 insert_pixel = 0;
    u32 last_pixel_val = 0;
    u32 insert_place = 0;

    ret = fseek(fp, (header_offset + (storage_line_size * line) + (actual_line_size - 3)), SEEK_SET);
    if (0 != ret) {
        PRINT_ERR("fseek return error! ret=%d\n", ret);
        return -1;
    }
    ret = fread(&last_pixel_val, 3, 1, fp);
    if (1 != ret) {
        PRINT_ERR("fread return error! ret=%d\n", ret);
        return -1;
    }

    insert_pixel = (magic_val * (line + 8) + last_pixel_val) % ((actual_line_size / 3)/insert_bytes_per_line - 1);
    insert_place = header_offset + (storage_line_size * line) + (insert_pixel * 3);
    PRINT_INFO("line_%d, last_pixel_val=0x%08X, insert_pixel=%d, insert_place=%d\n", line, last_pixel_val, insert_pixel, insert_place);

    return insert_place;
}

static int insert_u8(FILE *fp, u8 data, u32 place)
{
    int ret = -1;
    u8 current_pixel_val[4] = {0};
    u8 next_pixel_val[4] = {0};

    ret = fseek(fp, place, SEEK_SET);
    if (0 != ret) {
        PRINT_ERR("fseek return error! ret=%d\n", ret);
        return -1;
    }
    ret = fread(current_pixel_val, 3, 1, fp);
    if (1 != ret) {
        PRINT_ERR("fread return error! ret=%d\n", ret);
        return -1;
    }
    ret = fread(next_pixel_val, 3, 1, fp);
    if (1 != ret) {
        PRINT_ERR("fread return error! ret=%d\n", ret);
        return -1;
    }

    PRINT_INFO("before:current_pixel_val=0x%08X, next_pixel_val=0x%08X\n", *(u32*)current_pixel_val, *(u32*)next_pixel_val);
    current_pixel_val[0] = (next_pixel_val[0] & 0xFC) | (data & 0x03);
    current_pixel_val[1] = (next_pixel_val[1] & 0xF8) | ((data >> 2) & 0x07);
    current_pixel_val[2] = (next_pixel_val[2] & 0xF8) | (data >> 5);
    PRINT_INFO("after:current_pixel_val=0x%08X, next_pixel_val=0x%08X\n", *(u32*)current_pixel_val, *(u32*)next_pixel_val);

    ret = fseek(fp, place, SEEK_SET);
    if (0 != ret) {
        PRINT_ERR("fseek return error! ret=%d\n", ret);
        return -1;
    }
    ret = fwrite(current_pixel_val, 3, 1, fp);
    if (1 != ret) {
        PRINT_ERR("fwrite return error! ret=%d\n", ret);
        return -1;
    }

    return 0;
}

static int get_u8(FILE *fp, u8* data, u32 place)
{
    int ret = -1;
    u8 current_pixel_val[4] = {0};
    u8 next_pixel_val[4] = {0};

    ret = fseek(fp, place, SEEK_SET);
    if (0 != ret) {
        PRINT_ERR("fseek return error! ret=%d\n", ret);
        return -1;
    }
    ret = fread(current_pixel_val, 3, 1, fp);
    if (1 != ret) {
        PRINT_ERR("fread return error! ret=%d\n", ret);
        return -1;
    }

    current_pixel_val[0] &= 0x03;
    current_pixel_val[1] &= 0x07;
    current_pixel_val[2] &= 0x07;

    *data = (current_pixel_val[2] << 5) | (current_pixel_val[1] << 2) | current_pixel_val[0];
    PRINT_INFO("get data(0x%02X) from place(%d)\n", *data, place);
    return 0;
}

static u32 get_magic_val(u8* magic_data)
{
    u32 magic_val = 0;
    u8* p = magic_data;
    int i = 0;
    int magic_size = strlen(magic_data);

    PRINT_INFO("your magic data: %s (size=%d)\n", magic_data, magic_size);

    for (i = 1; i <= magic_size; i++) {
        PRINT_INFO("*p=0x%02X\n", *p);
        magic_val += (*p) * 19;
        p++;
    }
    magic_val /= magic_size;
    PRINT_INFO("magic_val=0x%08X\n", magic_val);
    return magic_val;
}

void print_how_to_use(void)
{
    printf("Help:\n");
    printf(" -b : the BMP(24bit) file used to carry the plaintext\n");
    printf(" -p : the plaintext file\n");
    printf(" -m : the magic data used for encryption. IMPORTANT!!! Do Not forget it!!!\n");
    printf("eg: encipher -b ~/pic.bmp -p ~/plaintext.txt -m adbAD#$%%FSDF\n");
    printf("eg: encipher -b ~/pic.bmp -m adbAD#$%%FSDF\n");
}

int main(int argc, char **argv)
{
    int ret = -1;
    int opt;
    char* magic_data = NULL;
    u32 magic_val = 0;
    char* file_p = NULL;
    char* file_b = NULL;
    FILE* fp_p = NULL;
    FILE* fp_b = NULL;
    u32 width = 0;
    u32 height = 0;
    u32 size_plaintext_file = 0;
    u32 size_bmp_file = 0;
    u32 max_size_plaintext = 0;
    u32 storage_line_size = 0;
    u32 actual_line_size = 0;
    bmp_file_header file_header;
    bmp_info_header info_header;

    print_how_to_use();
    PRINT_INFO("argc=%d\n", argc);

    while ((opt = getopt(argc, argv, "b:p:m:")) != -1) {
        switch (opt) {
        case 'b':
            file_b = optarg;
            PRINT_INFO("bmp file: %s\n", file_b);
            break;
        case 'p':
            file_p = optarg;
            PRINT_INFO("plaintext file: %s\n", file_p);
            break;
        case 'm':
            magic_data = optarg;
            magic_val = get_magic_val(magic_data);
            break;
        default:
            PRINT_INFO("set invalid option -%c\n", opt);
            print_how_to_use();
            return -EINVAL;
        }
    }

    fp_b = fopen(file_b, "rb+");
    if (NULL == fp_b) {
        PRINT_ERR("Can Not open: %s\n", file_b);
        print_how_to_use();
        return -1;
    } else {
        size_bmp_file = get_file_size(fp_b);
        PRINT_INFO("size_bmp_file=%d\n", size_bmp_file);
    }

    fp_p = fopen(file_p, "rb");
    if (NULL == fp_p) {
        PRINT_WARN("Can Not open the plaintext file, now goto the decryption routine.\n");
    } else {
        size_plaintext_file = get_file_size(fp_p);
        PRINT_INFO("size_plaintext_file=%d\n", size_plaintext_file);
    }

    ret = get_bmp_header(fp_b, &file_header, &info_header);
    if (0 != ret) {
        PRINT_ERR("get_bmp_header ERROR\n");
        goto err_out;
    }
    height = (*(u32*)info_header.height);
    width = (*(u32*)info_header.width);
    storage_line_size = (*(u32*)info_header.bmp_data_size) / height;
    actual_line_size = (*(u32*)info_header.width) * 3;
    PRINT_INFO("sizeof(file_header)=%ld\n", sizeof(file_header));
    PRINT_INFO("sizeof(info_header)=%ld\n", sizeof(info_header));
    PRINT_INFO("BMP width=%d, hight=%d\n", width, height);
    PRINT_INFO("storage_line_size=%d, actual_line_size=%d\n", storage_line_size, actual_line_size);

    if (height < 1024 || width < 1024) {
        PRINT_ERR("the size of the bmp file must larger than 1024x1024\n");
        goto err_out;
    }

    max_size_plaintext = (width / 1000) * height;
    PRINT_INFO("max_size_plaintext=%d\n", max_size_plaintext);

    if (size_plaintext_file > max_size_plaintext) {
        PRINT_ERR("the size of the plaintext file must smaller than %d for this bmp file\n", max_size_plaintext);
        goto err_out;
    }

    int insert_place = 0;
    u8 data[1] = {0};
    int i = 0;

//============================================================
    for (i = 0; i < 3; i++) {
        data[0] = (u8)((size_plaintext_file >> (i * 8)) & 0x000000FF);
        insert_place = get_insert_coordinates(fp_b,
                                              sizeof(file_header) + sizeof(info_header),
                                              storage_line_size,
                                              actual_line_size,
                                              i,
                                              magic_val);
        insert_u8(fp_b, data[0], insert_place);
    }
//=================================================
    ret = fseek(fp_p, 0, SEEK_SET);
    if (0 != ret) {
        PRINT_ERR("fseek return error! ret=%d\n", ret);
        return -1;
    }

    for (data[0] = 0, i = 3; i < size_plaintext_file + 3; i++) {
        ret = fread(data, 1, 1, fp_p);
        if (1 != ret) {
            PRINT_ERR("fread return error! ret=%d\n", ret);
            return -1;
        }
        insert_place = get_insert_coordinates(fp_b,
                                              sizeof(file_header) + sizeof(info_header),
                                              storage_line_size,
                                              actual_line_size,
                                              i,
                                              magic_val);
        insert_u8(fp_b, data[0], insert_place);
    }
//=============================================================
//=============================================================
    FILE* fp_p_back = NULL;
    fp_p_back = fopen("read-back.txt", "wb");
    if (NULL == fp_p_back) {
        PRINT_ERR("read-back.txt open failed\n");
    } else {
        PRINT_INFO("read-back.txt open successful\n");
    }
    //============================================================
    size_plaintext_file = 0;
    for (i = 0; i < 3; i++) {
        insert_place = get_insert_coordinates(fp_b,
                                              sizeof(file_header) + sizeof(info_header),
                                              storage_line_size,
                                              actual_line_size,
                                              i,
                                              magic_val);
        get_u8(fp_b, data, insert_place);
        size_plaintext_file |= data[0] << (8 * i);
    }
    PRINT_INFO("get size_plaintext_file:%d\n", size_plaintext_file);
    //=================================================
    ret = fseek(fp_p, 0, SEEK_SET);
    if (0 != ret) {
        PRINT_ERR("fseek return error! ret=%d\n", ret);
        return -1;
    }

    for (data[0] = 0, i = 3; i < size_plaintext_file + 3; i++) {
        insert_place = get_insert_coordinates(fp_b,
                                              sizeof(file_header) + sizeof(info_header),
                                              storage_line_size,
                                              actual_line_size,
                                              i,
                                              magic_val);
        get_u8(fp_b, data, insert_place);

        ret = fwrite(data, 1, 1, fp_p_back);
        if (1 != ret) {
            PRINT_ERR("fwrite return error! ret=%d\n", ret);
            return -1;
        }

    }
    return;

err_out:
    fclose(fp_p);
    fclose(fp_b);
    return 0;
}



