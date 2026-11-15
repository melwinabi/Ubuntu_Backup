#include<stdio.h>

struct marks{
    int BE;
    int SSLC;
    int HSC;
};
struct bio{
    int Height;
    int Weight;
    char *Blood_group;
    
};
struct overall{
    struct marks m;
    struct bio b;
}xy;

int main()
{
    xy.m.BE=8;
    xy.m.SSLC=500;
    xy.m.HSC=600;
    xy.b.Height=159;
    xy.b.Weight=50;
    xy.b.Blood_group="B positive";
  
  printf("address of be is  %p\n",&xy.m.BE);
   printf("address of sslc is %p\n",&xy.m.SSLC);
    printf("address of HSC is %p\n",&xy.m.HSC);
     printf("address of height is %p\n",&xy.b.Height);
      printf("address of weight is %p\n",&xy.b.Weight);
       printf("address of blood group is %p\n",&xy.b.Blood_group);
}