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

void send_by_rows(int rank, int side_length, double *min_number);
void send_by_columns(int rank, int side_length, double *min_number);

int main(int argc, char **argv){

    int rank, size, n_numbers, num_available = 0;
    double buf[1];

    /*Start MPI Application*/
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    distribute_numbers(rank, size, &n_numbers, &num_available, buf);
    compute_smaller(rank, n_numbers, buf, num_available);
    if(rank == 0) printf("\n\n*************************AFTER COMPUTING SMALLER NUMBER********************\nThe smallest number is %.2f\n", buf[0]);
    MPI_Finalize();
    return EXIT_SUCCESS;
}

void compute_smaller(int rank, int n_numbers, double buf[], int num_available){
    print_message("*************PROPAGATION BETWEEN ROWS****************", rank);
    if (num_available) send_by_rows((rank+1), sqrt(n_numbers), &buf[0]);
    print_message("*************PROPAGATION BETWEEN COLUMNS****************", rank);
    if(num_available) send_by_columns((rank+1), sqrt(n_numbers), &buf[0]);
    MPI_Barrier(MPI_COMM_WORLD);
}

void send_by_rows(int rank, int side_length, double *min_number){
    double buf[1];
    int source = rank - 1, dest = rank + 1;
    MPI_Status status;
    
    if((rank % side_length) == 1 || rank == 1) source = rank + side_length - 1;
    
    if((rank % side_length) == 0) dest = rank - side_length + 1;

    printf("Rank: %d, Source: %d, Dest: %d\n",rank, source, dest);

    for(int i = 1; i <= side_length; i++){
        MPI_Send(min_number, 1, MPI_DOUBLE, (dest-1), i, MPI_COMM_WORLD);
        
        MPI_Recv(buf, 1, MPI_DOUBLE, (source-1), MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if(buf[0] < *min_number) *min_number = buf[0];
    }
}

void send_by_columns(int rank, int side_length, double *min_number){
    double buf[1];
    int source = rank + side_length, dest = rank - side_length;
    MPI_Status status;
    
    if(rank > (side_length * (side_length - 1))) source = rank - (side_length * (side_length - 1));
    
    if(rank <= side_length) dest = rank + (side_length * (side_length - 1));

    printf("Rank: %d, Source: %d, Dest: %d\n", rank, source, dest);

    for(int i = 1; i <= side_length; i++){
        MPI_Send(min_number, 1, MPI_DOUBLE, (dest-1), i, MPI_COMM_WORLD);
        
        MPI_Recv(buf, 1, MPI_DOUBLE, (source-1), MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        if(buf[0] < *min_number) *min_number = buf[0];
    }
}
