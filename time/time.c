#include <stdio.h>
#include <sys/time.h>

int main() {
struct timeval start, end;

gettimeofday( &start, NULL );
//sleep(3);
for(int i=0;i<=99;i++)
printf(" \b");
;
gettimeofday( &end, NULL );

//求出两次时间的差值，单位为us
int timeuse = 1000000 * ( end.tv_sec - start.tv_sec ) + end.tv_usec - start.tv_usec;
printf("time: %d us\n", timeuse);
return 0;
}

