#include <stdio.h>
#include <stdlib.h>

int main(void)
{
	int number = 0;
	int temp = 0;
	int s = 0;
	int m = 0;
	int a1 = 0;
	int a2 = 0;
	int a3 = 0;
	int a4 = 0;
	int a5 = 0;

	while(1) {
		printf("\nplease input a number:");
		scanf("%d", &number);
		if(number < 30000 || number > 69999) {
			printf("the input number must between 30000~69999 !!!\n");
			continue;
		}
		else
			break;
	}

	temp = number;

	a5 = temp / 10000;
	temp = temp % 10000;

	a4 = temp / 1000;
	temp = temp % 1000;

	a3 = temp / 100;
	temp = temp % 100;

	a2 = temp / 10;
	temp = temp % 10;

	a1 = temp;

	s = (a5 * 1) + (a4 * 9) + (a3 * 9) + (a2 * 3) + (a1 * 7);

	m = s % 10;

	if(0 == m)
		printf("%d is right\n", number);
	else
		printf("%d is wrong\n", number);

}

