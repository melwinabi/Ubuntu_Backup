#include<stdio.h>
int main(){
    char a,b,n;
    printf("Enter a alphabet : ");
    scanf("%c",&n);
    a=(n=='a'||n=='e'||n=='i'||n=='o'||n=='u');
    
    b=(n=='A'||n=='E'||n=='I'||n=='O'||n=='U');
    if(a||b)
    {
        printf("vowel");
    }
    else
    {
        printf("not a vowel");
    }
}