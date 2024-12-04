#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TYHX_DBG_LEVEL 3
#define ENTER \
do{ if(TYHX_DBG_LEVEL >= 3) printf("[TYHX_DBG][%04d][%s]\n", __LINE__, __func__); }while(0)

#define PRINT_DBG(format,x...) \
do{ if(TYHX_DBG_LEVEL >= 2) printf("[TYHX_DBG][%04d][%s] " format, __LINE__, __func__, ## x); }while(0)

#define PRINT_INF(format,x...) \
do{ if(TYHX_DBG_LEVEL >= 1) printf("[TYHX_INF][%04d][%s] " format, __LINE__, __func__, ## x); }while(0)

#define PRINT_ERR(format,x...) \
do{ if(TYHX_DBG_LEVEL >= 0) printf("[TYHX_ERR][%04d][%s] " format, __LINE__, __func__, ## x); }while(0)

#define MAX_PAIRS 256
#define HEADER_SIZE 16

//Retrieve the line count from a text file
int get_record_count(char *name_txt)
{
	FILE *fp_txt;
	int i;
	int ret;
	unsigned addr;
	unsigned val;

	fp_txt = fopen(name_txt, "r");
	if (fp_txt == NULL) {
		PRINT_ERR("Error opening:%s\n", name_txt);
		return -1;
	}
	PRINT_DBG("%s opend success\n", name_txt);

	for (i = 0; i <= MAX_PAIRS; i++) {
		ret = fscanf(fp_txt, "0x%x,0x%x\n", &addr, &val);
		if (ret == EOF) {
			break;
		}

		if (i == MAX_PAIRS) {
			PRINT_ERR("%s:out of range. Max line:%d (error found at line:%d)\n", name_txt, MAX_PAIRS, i + 1);
			fclose(fp_txt);
			return -1;
		}

		if (ret != 2) {
			PRINT_ERR("%s: wrong format (error found at line:%d)\n", name_txt, i + 1);
			fclose(fp_txt);
			return -1;
		}

		if (addr > 0xFF || val > 0xFF) {
			PRINT_ERR("%s:out of range. Max val:0xFF (error found at line:%d)\n", name_txt, i + 1);
			fclose(fp_txt);
			return -1;
		}

		PRINT_DBG("line:%03d 0x%02X,0x%02X\n", i + 1, addr, val);
	}

	PRINT_DBG("%s:format check successful. total line:%d\n", name_txt, i);
	fclose(fp_txt);
	return i;
}

int bin_data_fill(char *name_txt, unsigned char *p, unsigned int num_pairs, unsigned char fw_ver, unsigned char chip_type)
{
	unsigned char *p_header = p;
	unsigned char *p_data = p_header + HEADER_SIZE;
	FILE *fp_txt;
	int ret;
	int i;
	unsigned addr;
	unsigned val;
	unsigned char checksum;

	fp_txt = fopen(name_txt, "r");
	if (fp_txt == NULL) {
		PRINT_ERR("Error opening:%s\n", name_txt);
		return -1;
	}
	PRINT_DBG("%s opend success\n", name_txt);

	checksum = 0;
	for (i = 0; i < num_pairs; i++) {
		ret = fscanf(fp_txt, "0x%x,0x%x\n", &addr, &val);
		p_data[i * 2] = addr;
		p_data[i * 2 + 1] = val;
		checksum ^= p_data[i * 2];
		checksum ^= p_data[i * 2 + 1];
		if (ret != 2) {
			PRINT_ERR("%s: wrong format (error found at line:%d)\n", name_txt, i + 1);
			fclose(fp_txt);
			return -1;
		}
		PRINT_DBG("line:%03d 0x%02X,0x%02X\n", i + 1, p_data[i * 2], p_data[i * 2 + 1]);
	}

	p_header[1] = chip_type;
	p_header[2] = fw_ver;
	p_header[3] = num_pairs >> 8;
	p_header[4] = num_pairs;
	for (i = 1; i < HEADER_SIZE; i++) {
		checksum ^= p_header[i];
	}
	p_header[0] = checksum;
	PRINT_INF("header: checksum=0x%X, num_pairs=%d, fw_ver=0x%X, chip_type=0x%X\n", checksum, num_pairs, fw_ver, chip_type);

	fclose(fp_txt);
	return 1;
}

int bin_file_write(char *name_bin, unsigned char *p, int total_size)
{
	FILE *fp_bin;
	int ret;

	fp_bin = fopen(name_bin, "wb");
	if (fp_bin == NULL) {
		PRINT_ERR("Error opening:%s\n", name_bin);
		return -1;
	}
	PRINT_DBG("%s opend success\n", name_bin);

	ret = fwrite(p, total_size, 1, fp_bin);
	fclose(fp_bin);
	if (ret == 1) {
		PRINT_INF("write bin data to %s successful\n", name_bin);
		return 1;
	} else {
		PRINT_ERR("write bin data to %s failed!!!\n", name_bin);
		return -1;
	}
}

int txt_2_bin(char *name_bin, char *name_txt, unsigned char fw_ver, unsigned char chip_type)
{
	FILE *fp_bin, *fp_txt;
	unsigned char *data;
	int total_size;
	int ret;
	unsigned int num_pairs;

	ret = get_record_count(name_txt);
	if (ret < 0) {
		PRINT_ERR("file_format_check failed\n");
		return -1;
	}
	num_pairs = ret;

	total_size = num_pairs * 2 + HEADER_SIZE;
	data = (unsigned char *)malloc(total_size);
	if (data == NULL) {
		PRINT_ERR("Memory allocation failed\n");
		return -1;
	}
	PRINT_DBG("malloc success\n");
	memset(data, 0xFF, total_size);

	ret = bin_data_fill(name_txt, data, num_pairs, fw_ver, chip_type);
	if (ret == -1) {
		PRINT_ERR("bin_data_fill failed\n");
		free(data);
		return -1;
	}

	ret = bin_file_write(name_bin, data, total_size);
	if (ret == -1) {
		PRINT_ERR("bin_file_write failed\n");
		free(data);
		return -1;
	}

	free(data);
	return 1;
}

int bin_2_txt(char *name_bin, char *name_txt)
{
	unsigned char header[HEADER_SIZE];
	unsigned char *data;
	FILE *fp_bin, *fp_txt;
	unsigned int num_pairs;
	unsigned char checksum;
	unsigned char chip_type;
	unsigned char fw_ver;
	unsigned char temp;
	int ret;
	int i;

	fp_bin = fopen(name_bin, "rb");
	if (fp_bin == NULL) {
		PRINT_ERR("Error opening %s\n", name_bin);
		goto failed_open_bin;
	}
	PRINT_DBG("%s opend success\n", name_bin);

	ret = fread(header, HEADER_SIZE, 1, fp_bin);
	if (ret != 1) {
		PRINT_ERR("Error of fread\n");
		goto failed_fread_header;
	}

	checksum = header[0];
	chip_type = header[1];
	fw_ver = header[2];
	num_pairs = (header[3] << 8) | header[4];
	PRINT_INF("get header form %s: checksum=0x%X, num_pairs=%d, fw_ver=0x%X, chip_type=0x%X\n", name_bin, checksum, num_pairs, fw_ver, chip_type);

	temp = 0;
	for (i = 1; i < HEADER_SIZE; i++) {
		temp ^= header[i];
	}

	data = (unsigned char *)malloc(num_pairs * 2);
	if (data == NULL) {
		PRINT_ERR("Memory allocation failed\n");
		goto failed_malloc;
	}
	PRINT_DBG("malloc success\n");

	ret = fread(data, num_pairs * 2, 1, fp_bin);
	if (ret != 1) {
		PRINT_ERR("Error of fread\n");
		goto failed_fread_data;
	}

	for (i = 0; i < num_pairs; i++) {
		temp ^= data[i * 2];
		temp ^= data[i * 2 + 1];
		PRINT_DBG("line:%03d 0x%02X, 0x%02X\n", i + 1, data[i * 2], data[i * 2 + 1]);
	}

	if (temp == checksum) {
		PRINT_INF("checksum OK\n");
	} else {
		PRINT_ERR("checksum failed\n");
		goto failed_checksum;
	}

	fp_txt = fopen(name_txt, "w");
	if (fp_txt == NULL) {
		PRINT_ERR("Error opening %s\n", name_txt);
		goto failed_open_txt;
	}

	for (i = 0; i < num_pairs; i++) {
		ret = fprintf(fp_txt, "0x%02X,0x%02X\n", data[i * 2], data[i * 2 + 1]);
		if (ret != 10) {
			PRINT_ERR("Error of fread\n");
			goto failed_write_data;
		}
	}

	fclose(fp_txt);
	fclose(fp_bin);
	free(data);
	PRINT_INF("%s parsing successful. please check %s\n", name_bin, name_txt);
	return 1;

failed_write_data:
	fclose(fp_txt);
failed_open_txt:
failed_checksum:
failed_fread_data:
	free(data);
failed_malloc:
failed_fread_header:
	fclose(fp_bin);
failed_open_bin:
	return -1;
}

int main(int argc, char *argv[])
{
	int ret;
	char *input_file = NULL;
	char *output_file = NULL;
	char *str_fw_ver = NULL;
	char *str_chip_type = NULL;
	unsigned int fw_ver;
	unsigned int chip_type;

	bool txt2bin = false;

	printf("How to use:\n");
	printf("convert txt to bin: tyhx-bin-maker -t file.txt -o new-file.bin\n");
	printf("convert bin to txt: tyhx-bin-maker -b file.bin -o new-file.txt\n");

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			if (i + 1 < argc) {
				str_fw_ver = argv[i + 1];
				i++;
			} else {
				PRINT_ERR("Error: missing input file\n");
				return 1;
			}
		} else if (strcmp(argv[i], "-c") == 0) {
			if (i + 1 < argc) {
				str_chip_type = argv[i + 1];
				i++;
			} else {
				PRINT_ERR("Error: missing input file\n");
				return 1;
			}
		} else if (strcmp(argv[i], "-t") == 0) {
			txt2bin = true;
			if (i + 1 < argc) {
				input_file = argv[i + 1];
				i++;
			} else {
				PRINT_ERR("Error: missing input file\n");
				return 1;
			}
		} else if (strcmp(argv[i], "-b") == 0) {
			txt2bin = false;
			if (i + 1 < argc) {
				input_file = argv[i + 1];
				i++;
			} else {
				PRINT_ERR("Error: missing input file\n");
				return 1;
			}
		} else if (strcmp(argv[i], "-o") == 0) {
			if (i + 1 < argc) {
				output_file = argv[i + 1];
				i++;
			} else {
				PRINT_ERR("Error: missing output file\n");
				return 1;
			}
		} else {
			PRINT_ERR("Unknown option: %s\n", argv[i]);
			return 1;
		}
	}

	if (input_file == NULL || output_file == NULL) {
		PRINT_ERR("Error: missing required arguments\n");
		return 1;
	}

	PRINT_DBG("txt2bin=%d\n", txt2bin);
	PRINT_DBG("input: %s\n", input_file);
	PRINT_DBG("output: %s\n", output_file);
	if (txt2bin) {
		chip_type = 0;
		if (str_chip_type != NULL) {
			if (sscanf(str_chip_type, "0x%x", &chip_type) != 1) {
				PRINT_ERR("Invalid input\n");
				chip_type = 0;
			}
		}
		PRINT_DBG("chip_type: 0x%X\n", chip_type);

		fw_ver = 0;
		if (str_fw_ver != NULL) {
			if (sscanf(str_fw_ver, "0x%x", &fw_ver) != 1) {
				PRINT_ERR("Invalid input\n");
				fw_ver = 0;
			}
		}
		PRINT_DBG("fw_ver: 0x%X\n", fw_ver);
	}

	if (txt2bin) {
		ret = txt_2_bin(output_file, input_file, fw_ver, chip_type);
		if (ret == -1) {
			PRINT_ERR("txt_2_bin failed\n");
			return 1;
		}
	} else {
		ret = bin_2_txt(input_file, output_file);
		if (ret == -1) {
			PRINT_ERR("bin_2_txt failed\n");
			return 1;
		}
	}

	return 0;
}
