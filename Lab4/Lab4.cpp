#include "mpi.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace std;
#define ARRAY_SIZE 20
void swap(int* arr, int i, int j)
{
	int t = arr[i];
	arr[i] = arr[j];
	arr[j] = t;
}
void quicksort(int* arr, int start, int end)
{
	int pivot, index;
	if (end <= 1)
		return;
	pivot = arr[start + end / 2];
	swap(arr, start, start + end / 2);
	index = start;
	for (int i = start + 1; i < start + end; i++) {
		if (arr[i] < pivot) {
			index++;
			swap(arr, i, index);
		}
	}
	swap(arr, start, index);
	quicksort(arr, start, index - start);
	quicksort(arr, index + 1, start + end - index - 1);
}
int* merge(int* arr1, int n1, int* arr2, int n2)
{
	int* result = (int*)malloc((n1 + n2) * sizeof(int));
	int i = 0;
	int j = 0;
	int k;
	
		for (k = 0; k < n1 + n2; k++) {
			if (i >= n1) {
				result[k] = arr2[j];
				j++;
			}
			else if (j >= n2) {
				result[k] = arr1[i];
				i++;
			}
			else if (arr1[i] < arr2[j]) {
				result[k] = arr1[i];
				i++;
			}
			else {
				result[k] = arr2[j];
				j++;
			}
		}
	return result;
}
int main(int argc, char* argv[])
{
	int number_of_elements;
	int* data = NULL;
	int chunk_size, own_chunk_size;
	int* chunk;
	MPI_Status status;
	int number_of_process, rank_of_process;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank_of_process);
	if (rank_of_process == 0)
	{
		number_of_elements = ARRAY_SIZE;
		data = (int*)malloc(number_of_elements * sizeof(int));
		for (int i = 0; i < number_of_elements; i++)
			data[i] = ARRAY_SIZE - i;
		printf("The initial array is : \n");
		for (int i = 0; i < number_of_elements; i++)
			printf("%d ", data[i]);
		printf("\n");
	}
	
	// Blocks all process until reach this point
	MPI_Barrier(MPI_COMM_WORLD);
	// BroadCast the Size to all the process from root process
	MPI_Bcast(&number_of_elements, 1, MPI_INT, 0, MPI_COMM_WORLD);
	// Computing chunk size
	if (number_of_elements % number_of_process == 0)
		chunk_size = number_of_elements / number_of_process;
	else
		chunk_size = (number_of_elements / (number_of_process - 1));
	// chunk array
	chunk = (int*)malloc(chunk_size * sizeof(int));
	// Scatter the chuck size data to all process
	MPI_Scatter(data, chunk_size, MPI_INT, chunk, chunk_size, MPI_INT, 0,
		MPI_COMM_WORLD);
	free(data);
	data = NULL;
	// Compute size of own chunk and then sort them using quick sort
	own_chunk_size = (number_of_elements >= chunk_size * (rank_of_process + 1)) ?
		chunk_size : (number_of_elements -
			chunk_size * rank_of_process);
	printf("The process %d sorted the following array: \n", rank_of_process);
	for (int i = 0; i < own_chunk_size; i++)
		printf("%d ", chunk[i]);
	printf("\n");
	quicksort(chunk, 0, own_chunk_size);
	for (int step = 1; step < number_of_process; step = 2 * step)
	{
		if (rank_of_process % (2 * step) != 0)
		{
			MPI_Send(chunk, own_chunk_size, MPI_INT, rank_of_process - step, 0,
				MPI_COMM_WORLD);
			break;
		}
		if (rank_of_process + step < number_of_process)
		{
			int received_chunk_size
				= (number_of_elements
					>= chunk_size
					* (rank_of_process + 2 * step))
				? (chunk_size * step)
				
				: (number_of_elements
					- chunk_size
					* (rank_of_process + step));
			int* chunk_received;
			chunk_received = (int*)malloc(received_chunk_size * sizeof(int));
			MPI_Recv(chunk_received, received_chunk_size, MPI_INT, rank_of_process +
				step, 0,
				MPI_COMM_WORLD, &status);
			data = merge(chunk, own_chunk_size, chunk_received,
				received_chunk_size);
			free(chunk);
			free(chunk_received);
			chunk = data;
			own_chunk_size = own_chunk_size + received_chunk_size;
		}
	}
	if (rank_of_process == 0)
	{
		printf("The Sorted array is: \n");
		for (int i = 0; i < number_of_elements; i++)
			printf("%d ", chunk[i]);
	}
	MPI_Finalize();
	return 0;
}