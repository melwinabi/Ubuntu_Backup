#include<stdio.h>
#include<time.h>

int main()
{
    time_t t=time(NULL);
    struct tm x=*localtime(&t);
    printf("Current date : %02d-%02d-%02d\n",x.tm_mday,x.tm_mon+1,x.tm_year+1900);
    printf("Current time : %02d : %02d : %02d",x.tm_hour,x.tm_min+1,x.tm_sec);
}
/*
#include<stdio.h>
#include<time.h>

void main()
{
    time_t t = time(NULL);
    printf("\n Current date and time is : %s", ctime(&t));
}*/
