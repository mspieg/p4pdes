#include <stdio.h>

double horner_exp_terms(double x, int n_start, int n_end) {
    if (n_end < n_start) return 0.0;
    
    // Start with the highest term: x^N/N!
    double result = 1.0;
    for (int i = n_end ; i > n_start; i--) {
        result = 1.0 + result * x / i;
    }
    
    // Multiply by x^k/k! at the end
    for (int i = 0; i < n_start; i++) {
        result *= x/(i + 1);
    }
    
    
    return result;
}

int main(int argc, char **argv) {
    double x = 1.0;
    int n_start = 0;
    int n_end = 100;
    
    printf("Sum of x^n/n! for n=%d to %d with x=%.2f: %.10f\n", 
           n_start, n_end, x, horner_exp_terms(x, n_start, n_end));
    
    return 0;
}
