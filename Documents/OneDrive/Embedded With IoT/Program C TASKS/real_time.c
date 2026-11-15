#include<stdio.h>
#include<unistd.h>
#include<time.h>

int main()
{
    while(1)
{
    time_t t=time(NULL);
    struct tm *x=localtime(&t);

    system("cls");

    printf("Current date : %02d-%02d-%02d\n",x->tm_mday,x->tm_mon+1,x->tm_year+1900);
    printf("Current time : %02d : %02d : %02d",x->tm_hour,x->tm_min,x->tm_sec);
    sleep(1);
}
}
