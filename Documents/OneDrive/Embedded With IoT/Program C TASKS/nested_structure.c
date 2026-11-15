//separate nested structure

/*#include<stdio.h>
 struct emp{
     int age;
     int salary;
 };
 struct manager{
     int age;
     int salary;
     struct emp e;
     };
   int main()  
     {
      struct manager m;
      m.e.age=30;
      m.age=40;
      printf("Age of employee : %d\n", m.e.age);
      printf("Age of manager : %d\n", m.age);
     }*/


     //embedded nested structure

#include<stdio.h>

struct manager{
    int age;
    int salary;
    
    struct employee{
        int age;
        int salary;
    }e;
};

int main()
{
    struct manager m;
 
      m.e.age=30;
      m.age=40;
      printf("Age of employee : %d\n", m.e.age);
      printf("Age of manager : %d\n", m.age);
}