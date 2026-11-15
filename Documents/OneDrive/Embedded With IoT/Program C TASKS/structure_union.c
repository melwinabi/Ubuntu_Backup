#include<stdio.h>
union A{
    int x;
    char y;
    double z;
};
struct B{
    int x;
    char y;
    double z;
};

int main()
{
    union A a;
    struct B b;
     a.x=10;
  // a.y=20;
   printf("Size of union is %d\n",sizeof(union A));
   printf("Size of struct is %d",sizeof(struct B));
}