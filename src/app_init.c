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

void distribute_numbers(int rank, int size, int *n_numbers, int *num_available, double buf[]){
    double *num_array;
    MPI_Status status;
    if(rank == 0){
        FILE *fp = NULL;
        num_array = malloc(MAX_ARRAY * sizeof(int));
        
        if((fp = fopen(FILENAME, "r")) == NULL){
            fprintf(stderr, "Error opening file: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        for(*n_numbers = 0; fscanf(fp, "%lf,", &num_array[*n_numbers]) == 1; (*n_numbers)++){};
        fclose(fp);
        
        if(size < *n_numbers){
            fprintf(stderr, "Not enough processes launched\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            exit(EXIT_FAILURE);
        }
        buf[0] = num_array[0];
        *num_available = 1;
    }

    MPI_Bcast(n_numbers, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0){
        send_numbers(num_array, *n_numbers);
        free(num_array);
    }
    else if (rank < *n_numbers){
        MPI_Recv(buf, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        *num_available = 1;
    }

    if(*num_available) printf("I'm process %d out of %d. My number is: %.2f\n", (rank+1), size, buf[0]);
    MPI_Barrier(MPI_COMM_WORLD);
}

void send_numbers(double *num_array, int n_numbers){
    for(int i = 1; i < n_numbers; i++){
        MPI_Send((num_array + i), 1, MPI_DOUBLE, i, i, MPI_COMM_WORLD);
    }
}

void print_message(char *string, int rank){
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 0) printf("\n\n%s\n", string);
    MPI_Barrier(MPI_COMM_WORLD);
}