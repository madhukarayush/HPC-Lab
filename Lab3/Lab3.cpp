#include<iostream>
#include "mpi.h"
#define NUM_ROWS_A 8
#define NUM_COLUMNS_A 10
#define NUM_ROWS_B 10
#define NUM_COLUMNS_B 8
#define MASTER_TO_SLAVE_TAG 1 //tag for messages sent from master to slaves
#define SLAVE_TO_MASTER_TAG 4 //tag for messages sent from slaves to master

void create_matrix();
void printArray();

int rank;
int size;
int i, j, k;
double A[NUM_ROWS_A][NUM_COLUMNS_A];
double B[NUM_ROWS_B][NUM_COLUMNS_B];
double result[NUM_ROWS_A][NUM_COLUMNS_B];

int low_bound; //low bound of the number of rows of [A] allocated to a slave
int upper_bound; //upper bound of the number of rows of [A] allocated to a slave
int portion; //portion of the number of rows of [A] allocated to a slave
MPI_Status status; // store status of a MPI_Recv
MPI_Request request; //capture request of a MPI_Send

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	if (rank == 0)
	{ // master process
		create_matrix();
		for (i = 1; i < size; i++)
		{
			portion = (NUM_ROWS_A / (size - 1)); // portion without master
			low_bound = (i - 1) * portion;
			if (((i + 1) == size) && ((NUM_ROWS_A % (size - 1)) != 0))
			{//if rows of [A] cannot be equally divided among slaves
				upper_bound = NUM_ROWS_A; //last slave gets all the remaining rows
			}
			else {
				
				upper_bound = low_bound + portion; //rows of [A] are equally divisable among slaves
			}
			MPI_Send(&low_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG,
				MPI_COMM_WORLD);
			MPI_Send(&upper_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG + 1,
				MPI_COMM_WORLD);
			MPI_Send(&A[low_bound][0], (upper_bound - low_bound) * NUM_COLUMNS_A,
				MPI_DOUBLE, i, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD);
		}
	}
	//broadcast [B] to all the slaves
	MPI_Bcast(&B, NUM_ROWS_B * NUM_COLUMNS_B, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	/* Slave process*/
	if (rank > 0)
	{
		MPI_Recv(&low_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD,
			&status);
		MPI_Recv(&upper_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG + 1,
			MPI_COMM_WORLD, &status);
		MPI_Recv(&A[low_bound][0], (upper_bound - low_bound) * NUM_COLUMNS_A,
			MPI_DOUBLE, 0, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD, &status);
		printf("Process %d calculating for rows %d to %d of Matrix A\n", rank,
			low_bound, upper_bound);
		for (i = low_bound; i < upper_bound; i++)
		{
			for (j = 0; j < NUM_COLUMNS_B; j++)
			{
				for (k = 0; k < NUM_ROWS_B; k++)
				{
					result[i][j] += (A[i][k] * B[k][j]);
				}
			}
		}
		MPI_Send(&low_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD);
		MPI_Send(&upper_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG + 1,
			MPI_COMM_WORLD);
		MPI_Send(&result[low_bound][0], (upper_bound - low_bound) * NUM_COLUMNS_B,
			MPI_DOUBLE, 0, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD);
	}
	/* master gathers processed work*/
	if (rank == 0) {
		for (i = 1; i < size; i++) {
			MPI_Recv(&low_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD,
				&status);
			MPI_Recv(&upper_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG + 1,
				MPI_COMM_WORLD, &status);
			
				MPI_Recv(&result[low_bound][0], (upper_bound - low_bound) *
				NUM_COLUMNS_B, MPI_DOUBLE, i, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD, &status);
		}
		printArray();
	}
	MPI_Finalize();
	return 0;
}
void create_matrix()
{
	for (i = 0; i < NUM_ROWS_A; i++) {
		for (j = 0; j < NUM_COLUMNS_A; j++) {
			A[i][j] = i + j;
		}
	}
	for (i = 0; i < NUM_ROWS_B; i++) {
		for (j = 0; j < NUM_COLUMNS_B; j++) {
			B[i][j] = i * j;
		}
	}
}
void printArray()
{
	printf("The matrix A is: \n");
	for (i = 0; i < NUM_ROWS_A; i++) {
		printf("\n");
		for (j = 0; j < NUM_COLUMNS_A; j++)
			printf("%8.2f ", A[i][j]);
	}
	printf("\n\n\n");
	printf("The matrix B is: \n");
	for (i = 0; i < NUM_ROWS_B; i++) {
		printf("\n");
		for (j = 0; j < NUM_COLUMNS_B; j++)
			printf("%8.2f ", B[i][j]);
	}
	printf("\n\n\n");
	printf("The result matrix is: \n");
	for (i = 0; i < NUM_ROWS_A; i++) {
		printf("\n");
		for (j = 0; j < NUM_COLUMNS_B; j++)
			printf("%8.2f ", result[i][j]);
	}
	printf("\n\n");
}