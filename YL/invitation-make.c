#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int high_two = 30; //30~69
	int low_two = 0; //0~15
	int h1 = 0;
	int h2 = 0;
	int l1 = 0;
	int l2 = 0;
	int s = 0;
	int m = 0;
	int new_num = 0;

	for(high_two=30; high_two<=69;high_two=high_two+1) {
		for(low_two=0; low_two<=15;low_two=low_two+1) {

			h2 = high_two / 10;
			h1 = high_two % 10;

			l2 = low_two / 10;
			l1 = low_two % 10;

			s = (h2 * 1) + (h1 * 9) + (l2 * 3) + (l1 * 7);
			m = s % 10;
			new_num = (high_two * 1000) + (m * 100) + low_two;
			printf("%d  ", new_num);
		}
	}
}

