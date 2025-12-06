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

double factorial(int n) {
    double res = 1.0;
    for (int i = 2; i <= n; ++i) res *= i;
    return res;
}

float e(int x) {
    float sum = 0.0f;
    for (int n = 0; n <= x; ++n) {
        sum += 1.0f / (float)factorial(n);
    }
    return sum;
}

char *convert(int x) {
    char *result = (char*)malloc(sizeof(char) * 65);
    if (!result) return NULL;
    
    int index = 0;
    if (x == 0) {
        strcpy(result, "0");
        return result;
    }

    int is_negative = 0;
    if (x < 0) {
        is_negative = 1;
        x = -x;
    }

    while (x != 0) {
        result[index++] = (x % 3) + '0';
        x /= 3;
    }

    if (is_negative) {
        result[index++] = '-';
    }
    result[index] = '\0';
    reverse(result);
    return result;
}
