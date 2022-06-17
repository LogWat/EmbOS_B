#include <stdio.h>

static double sqrt(int x) {
    if (x <= 0 || x > 10000) {
        return 0;
    }
    double x_double = (double)x;
    double left = 0.0, right = x_double, mid;
    while (right - left > 0.00001) {
        mid = (left + right) / 2.0;
        if (mid * mid <= x_double) {
            left = mid;
        } else {
            right = mid;
        }
    }
    return mid;
}

int main(void) {
    printf("%f\n", sqrt(2));
    return 0;
}