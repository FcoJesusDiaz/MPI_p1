#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include "app_init.h"

#define XOR(a, b) (a ^ b)

void send_by_rows(int rank, int side_length, double *min_number);
void send_by_columns(int rank, int side_length, double *min_number);
void compute_largest(int rank, int D, double *max_number);

int main(int argc, char **argv){

    int rank, size, n_numbers, num_available = 0;
    double buf[1];

    /*Start MPI Application*/
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    distribute_numbers(rank, size, &n_numbers, &num_available, buf);
    if(num_available) compute_largest(rank, (int)log2(n_numbers), buf);
    if(rank == 0) printf("\n\n*************************AFTER COMPUTING LARGEST NUMBER********************\nThe largest number is %.2f\n", buf[0]);
    MPI_Finalize();
    return EXIT_SUCCESS;
}

void compute_largest(int rank, int D, double *max_number){
    double buf[1];
    int dest, i;

    for(i = 0; i < D; i++){
        dest = XOR(rank, (int)pow(2,i));
        MPI_Send(max_number, 1, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
        MPI_Recv(buf, 1, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD, NULL);
        if(*buf > *max_number) *max_number = *buf;
    }
}