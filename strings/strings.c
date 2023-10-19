#include <stdio.h>
#include <string.h>
#include <stdint.h>

void reverseString(uint8_t *str) {
    if (!str) {return;}

    uint8_t len = strlen(str);
    if (len <= 1) {return;}

    uint8_t start = 0;
    uint8_t end = len - 1;

    while (start < end) {
        uint8_t temp = str[start];
        str[start] = str[end];
        str[end] = temp;

        start++;
        end--;
    }
}

int main() {
    uint8_t buffer[1024] = {0};
    puts("Enter a valid string:");
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        printf("You entered:\n%s", buffer);
    } else {
        printf("Error reading input.\n");
    }

    reverseString(buffer);
    printf("Reversed String:%s\n", buffer);

    return 0;
}
