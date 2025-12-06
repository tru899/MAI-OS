#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "../include/libs.h"

void reverse(char *str) {
    int n = strlen(str);
    for (int i = 0; i < n/2; ++i) {
        char temp = str[n - i - 1];
        str[n - i - 1] = str[i];
        str[i] = temp;
    }
}

float e(int x) {
    if (x == 0) return 1.0;
    return powf(1.0f + 1.0f / (float)x, (float)x);
}

char *convert(int x) {
    char *buffer = (char *)malloc(65 * sizeof(char));
    if (!buffer) return NULL;

    if (x == 0) {
        strcpy(buffer, "0");
        return buffer;
    }

    int index = 0;
    int is_negative = 0;
    
    if (x < 0) {
        is_negative = 1;
        x = -x; 
    }

    while (x > 0) {
        buffer[index++] = (x % 2) + '0';
        x /= 2;
    }

    if (is_negative) {
        buffer[index++] = '-';
    }

    buffer[index] = '\0';
    reverse(buffer);

    return buffer;
}
