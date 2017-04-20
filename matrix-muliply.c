#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
int main(int argc, char** argv)
{
	int r1, c1, r2, c2, r3, c3;
	int* mat1;
	int* mat2;
	int* mat3;
	int*recieve1;
	int*recieve2;
	int rank, size, remainder;
	MPI_Status status;
	MPI_Comm world = MPI_COMM_WORLD;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == 0) {
		printf("Enter dimension of first matrix: ");
		scanf("%d%d", &r1, &c1);
		mat1 = malloc(r1 * c1 * sizeof(int));
		for (int i = 0; i < r1 * c1; i++) {
			scanf("%d", &mat1[i]);
		}
		printf("Enter dimension of second matrix: ");
		scanf("%d%d", &r2, &c2);
		while (1) {
			if (c1 == r2) {
				break;
			}
			printf("Invalid dimesion!\nEnter dimension of second matrix: ");
			scanf("%d%d", &r2, &c2);
		}
		r3 = r1, c3 = c2;
		mat3 = malloc(r3 * c3 * sizeof(int));
		remainder = r3 - size - 1;
		if (remainder > 0) {
			int start = c1 * size - 1;
			for (int i = 1; i <= remainder; i++) {
				MPI_Send(&mat1[start], c1, MPI_INT, i, 0, world);
				start += c1;
			}
		}
	}
	MPI_Bcast(&r1, 1, MPI_INT, 0, world), MPI_Bcast(&c1, 1, MPI_INT, 0, world);
	MPI_Bcast(&r2, 1, MPI_INT, 0, world), MPI_Bcast(&c2, 1, MPI_INT, 0, world);
	MPI_Bcast(&r3, 1, MPI_INT, 0, world), MPI_Bcast(&c3, 1, MPI_INT, 0, world);
	recieve1 = malloc(c1 * sizeof(int));
	MPI_Scatter(&mat1, c1, MPI_INT, &recieve1, c1, MPI_INT, 0, world);
	mat2 = malloc(r2 * c2 * sizeof(int));
	if (rank == 0) {
		mat2 = malloc(r2 * c2 * sizeof(int));
		for (int i = 0; i < r2 * c2; i++) {
			scanf("%d", &mat2[i]);
		}
	}

	MPI_Bcast(mat2, r2 * c2, MPI_INT, 0, world);
	//recieve1 = malloc(c1 * sizeof(int));
	int* send = malloc(c2 * sizeof(int));
	for (int i = 0; i < c2; i++) {
		int sum = 0;
		for (int j = 0; j < r2; j++) {
			sum += recieve1[j] * mat2[j * c2 + i];
		}
		send[i] = sum;
	}
	MPI_Gather(&send, c2, MPI_INT, &mat3, c2, MPI_INT, 0, world);
	
	if (remainder > 0) {
		if (rank != 0)
		{
			if (rank <= remainder) {
				recieve2 = malloc(c1 * sizeof(int));
				MPI_Recv(&recieve2, c1, MPI_INT, 0, 0, world, MPI_STATUSES_IGNORE);
				int* send = malloc(c3 * sizeof(int));
				for (int i = 0; i < c2; i++) {
					int sum = 0;
					for (int j = 0; j < r2; j++) {
						sum += recieve2[j] * mat2[j * c2 + i];
					}
					send[i] = sum;
				}
				MPI_Send(&send, c3, MPI_INT, 0, 1, world);
			}
		}
	}
	if (rank == 0) {
		int start = c3 * size - 1;
		for (int i = 1; i <= remainder; i++) {
			MPI_Recv(&mat3[start], c3, MPI_INT, i, 1, world, MPI_STATUSES_IGNORE);
			start += c3;
		}
		for (int i = 0; i < r3 * c3; i++) {
			if ((i + 1) % c3 == 0) {
				printf("\n");
			}
			printf("%d ", mat3[i]);
		}
	}
	MPI_Finalize();
	return 0;
}
