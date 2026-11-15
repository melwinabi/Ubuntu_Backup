#include <stdio.h>
#include <unistd.h> // For sleep() and usleep()

int count = 0;

void counter() {
    count++; 
    printf("Count = %d\n", count);
}

int main() {
    while (1) { // Infinite loop
        counter(); 
        sleep(); // Pause for 1 second
    }
    
    return 0; // This will never be reached
}
