#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char Name[30];
char Number[20];
int Num;

void main()
{ 
  printf("...Welcome AAMN Grocery Shop...\n");
  printf("Enter the Customer Name : ");
  scanf("%s",Name);
  printf("Enter the Customer Mobile Number : +91 ");
  scanf("%s",Number);
  printf("Our Priduct List are : \n");
  printf("1. Rice\n");
  printf("2. Garlic\n");
  printf("3. Tomato\n");
  printf("4. Milk\n");
  printf("5. Bread1\n");
  printf("6. Juce\n\n");  
  printf("Enter the Product Number to Select > ");
  scanf("%d",&Num);
  if(Num == 1)
  {
    Rice();  
  }
  else if(Num == 2)
  {
    Garlic();
  }
  else if(Num == 3)
  {
    Tomato();
  }
  else if(Num == 4)
  {
    Milk();
  }
  else if(Num == 5)
  {
    Bread();
  }
  else if(Num == 6)
  {
    Juce();
  }  
}

void Rice()
{
 printf("Serial Number: 1234567890\n"); 
 printf("Product Name: Rice\n");
 printf("Manufacture Date: 01/01/2023 \n");
 printf("Expiry Date: 31/12/2025 \n");
 printf("Price: Rs 50.00 \n");
 printf("GST: 5Percent \n");
}

void Egg()
{

}

void Milk()
{

}

void Bread()
{

}

void Tomato()
{

}

void Garlic()
{

}

void Juce()
{

}