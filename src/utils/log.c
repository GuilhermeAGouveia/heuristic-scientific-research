#include <stdio.h>
#include <math.h>

/**
 * @brief Retorna um valor entre a e b, dado um valor x entre c e d, de forma logarítmica.
 * log_scale: [c, d] -> [a, b]
 * log_scale(c) = a
 * log_scale(d) = b
*/
double log_scale(double x, double a, double b, double c, double d)
{
    double result = a + (b - a) * (log(x) - log(c)) / (log(d) - log(c));
    return result;
}

int main()
{
    double x = 2;
    double a = 1;
    double b = 10;
    double c = 1;
    double d = 1000;
    for (int i = c; i <= d; i++)
    {
        double result = log_scale(i, a, b, c, d);
        printf("log_scale(%d) = %f\n", i, result);
    }

    return 0;
}