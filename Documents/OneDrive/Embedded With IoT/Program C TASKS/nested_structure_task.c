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
    printf("BE cgpa is %d\n",xy.m.BE);
     printf("SSLC mark is %d\n",xy.m.SSLC);
      printf("HSC is %d\n",xy.m.HSC);
       printf("Height is %d\n",xy.b.Height);
        printf("Weight is %d\n",xy.b.Weight);
         printf("Blood group is %s\n",xy.b.Blood_group);
}