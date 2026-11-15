#include<stdio.h>

struct marks{
   float BE;
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
};

int func(struct overall *ptr)
    {
    ptr->m.BE=8.53;
    ptr->m.SSLC=500;
    ptr->m.HSC=600;
    ptr->b.Height=159;
    ptr->b.Weight=50;
    ptr->b.Blood_group="B positive";
    printf("BE cgpa is %.2f\n",ptr->m.BE);
     printf("SSLC mark is %d\n",ptr->m.SSLC);
      printf("HSC is %d\n",ptr->m.HSC);
       printf("Height is %d\n",ptr->b.Height);
        printf("Weight is %d\n",ptr->b.Weight);
         printf("Blood group is %s\n",ptr->b.Blood_group);
    }


int main()
{
    struct overall xy[100];
    func(xy);
}
    