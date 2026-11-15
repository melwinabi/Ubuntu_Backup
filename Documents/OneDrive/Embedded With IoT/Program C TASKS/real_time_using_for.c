#include <stdio.h>
#include <time.h>
#include <unistd.h> 
int main() {
    for (int i = 0; i < 100000; i++) {  
        time_t t=time(NULL);
        struct tm *x=localtime(&t);

       system("cls");

       
        printf("Current date : %02d-%02d-%02d\n",x->tm_mday,x->tm_mon+1,x->tm_year+1900);
    
        printf("Current Time: %02d:%02d:%02d\n",x->tm_hour, x->tm_min, x->tm_sec);

        sleep(1);  // Wait for 1 second before the next iteration
    }
    return 0;
}
