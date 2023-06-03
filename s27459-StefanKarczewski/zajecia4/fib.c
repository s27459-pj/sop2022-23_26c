#include <stdio.h>
#include <stdlib.h>

int fib_rec(int n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    return fib_rec(n - 1) + fib_rec(n - 2);
}

int fib_iter(int n) {
    int i, a = 0, b = 1, c;
    for (i = 0; i < n; i++) {
        c = a + b;
        a = b;
        b = c;
    }
    return a;
}

int main(int argc, char const *argv[]) {
    int n;

    if (argc < 2) {
        printf("Usage: %s <n>\n", argv[0]);
        return 1;
    }
    n = atoi(argv[1]);

    printf("fib_rec(%d)  = %d\n", n, fib_rec(n));
    printf("fib_iter(%d) = %d\n", n, fib_iter(n));

    return 0;
}
