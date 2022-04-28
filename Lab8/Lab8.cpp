#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
	int i, N = 10, THREAD_COUNT = 3, CHUNK_SIZE = 3;
	printf("Default Scheduling\n");
#pragma omp parallel for num_threads(THREAD_COUNT)
	for (i = 0; i < N; i++)
		printf("ThreadID: %d, iteration: %d\n", omp_get_thread_num(), i);
	printf("\nStatic Scheduling\n");
#pragma omp parallel for num_threads(THREAD_COUNT) schedule(static, CHUNK_SIZE)
	for (i = 0; i < N; i++)
		printf("ThreadID: %d, iteration: %d\n", omp_get_thread_num(), i);
	printf("\nDynamic Scheduling\n");
#pragma omp parallel for num_threads(THREAD_COUNT) schedule(dynamic, CHUNK_SIZE)
	for (i = 0; i < N; i++)
		printf("ThreadID: %d, iteration: %d\n", omp_get_thread_num(), i);
	return 0;
}