#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
int fib(int n)
{
	int res;
	if (n == 0 or n == 1)
		res = n;
	else
	{
		int a, b;
#pragma omp task shared(a)
		a = fib(n - 1);
#pragma omp task shared(b)
		b = fib(n - 2);
#pragma omp taskwait
		res = a + b;
	}
	printf("%d th Fibonacci task calculated by thread %d\n", n,
		omp_get_thread_num());
	return res;
}
int main(int argc, char* argv[])
{
#pragma omp parallel
#pragma omp single
	{
		int n = atoi(argv[1]);
		printf("\nThe %d th Fibonacci Number = %d\n", n, fib(n));
	}
	return 0;
}