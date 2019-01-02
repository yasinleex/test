#include <stdio.h>
#include <stdlib.h>

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

typedef struct
{
	u16 id;
	u32 file_size;
	u32 reserved;
	u32 bmp_data_offset;
}bmp_file_header;

typedef struct
{
	u32 this_header_size;
	u32 width;
	u32 height;
	u16 planes;
	u16 bits_per_pixel;
	u32 compression;
	u32 bmp_data_size;
	u32 h_resolution;
	u32 v_resolution;
	u32 used_colors;
	u32 important_colors;
}bmp_info_header;


void main(void)
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


printf("please input the Data Type of the source file\n");
printf("0. Exit\n");
printf("1. ARGB\n");
printf("2. RGBA\n");
printf("3. RGB_888\n");
printf("4. RGB_565\n");

u32 choise;
printf("your choise is: ");
scanf("%u", &choise);

printf("choise=%u\n", choise);

u8 path_raw[64] = {'\0'};
u8 path_bmp[64] = {'\0'};

strcpy("");
FILE *file_raw = fopen("source.raw", "rb");
if (!file) {
    printf("Can Not open: %s\n");
    return;
}


FILE *file_bmp = fopen("a.bmp", "wb");
if (!file) {
    cout << "File not found";
    return;
}




return;
}



