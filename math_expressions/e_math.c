#include <stdio.h>
#include <math.h>

int main() {
    double a, z1, z2;
    
    printf("Enter 'a': ");
    scanf("%lf", &a);

    z1 = cos(a) + sin(a) + cos(3 * a) + sin(3 * a);
    z2 = 2 * sqrt(2) * cos(a) * sin(M_PI / 4 + 2 * a);

    printf("z1 = %f\n", z1);
    printf("z2 = %f\n", z2);

    return 0;
}