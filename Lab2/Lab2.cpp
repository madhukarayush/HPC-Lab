#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define n 10
int a[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
// Temporary array for other processes
int b[1000];
int main(int argc, char* argv[])
{
	int process_id, no_of_process,
		elements_per_process,
		n_elements_recieved;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
	MPI_Comm_size(MPI_COMM_WORLD, &no_of_process);
	// For process 0
	if (process_id == 0) {
		int index, i;
		elements_per_process = n / no_of_process;
		if (no_of_process > 1) {
			for (i = 1; i < no_of_process - 1; i++) {
				index = i * elements_per_process;
				MPI_Send(&elements_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(&a[index], elements_per_process, MPI_INT, i, 0,
					MPI_COMM_WORLD);
			}
			// last process adds remaining elements
			
				index = i * elements_per_process;
			int elements_left = n - index;
			MPI_Send(&elements_left, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(&a[index], elements_left, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		// sum by process 0
		int sum = 0;
		for (i = 0; i < elements_per_process; i++)
			sum += a[i];
		printf("Sum by process %d = %d\n", process_id, sum);
		// partial sums from other processes
		int tmp;
		for (i = 1; i < no_of_process; i++) {
			MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			int sender = status.MPI_SOURCE;
			sum += tmp;
		}
		// prints the final sum of array
		printf("Final Sum of array is : %d\n", sum);
	}
	// Other processes
	else {
		MPI_Recv(&n_elements_recieved, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		MPI_Recv(&b, n_elements_recieved, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		int partial_sum = 0;
		for (int i = 0; i < n_elements_recieved; i++)
			partial_sum += b[i];
		printf("Sum by process %d = %d\n", process_id, partial_sum);
		MPI_Send(&partial_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 0;
}