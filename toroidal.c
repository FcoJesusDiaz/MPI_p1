#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define FILENAME "numbers.dat"
#define MAXLINE 255
#define MAX_ARRAY 255

void send_numbers(double *num_array, int n_numbers);
void send_by_rows(int rank, int side_length, double *min_number);
void send_by_columns(int rank, int side_length, double *min_number);
void print_message(char *string, int rank);


int main(int argc, char **argv){

    int rank, size, n_numbers;
    double buf[1];
    MPI_Status status;


    /*Start MPI Application*/
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0){
        char *line;
        FILE *fp = NULL;
        double num_array[MAX_ARRAY];
        line = malloc(MAXLINE * sizeof(double));
        
        if((fp = fopen(FILENAME, "r")) == NULL){
            fprintf(stderr, "Error opening file: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        for(n_numbers = 0; fscanf(fp, "%lf,", &num_array[n_numbers]) == 1; n_numbers++){};

        if(size < n_numbers){
            fprintf(stderr, "Not enough processes launched\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return EXIT_FAILURE;
        }
        
        buf[0] = num_array[0];
        send_numbers(num_array, n_numbers);
        MPI_Bcast(&n_numbers, 1, MPI_INT, rank, MPI_COMM_WORLD);

        free(line);
        fclose(fp);
    }
    else{
        MPI_Recv(buf, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }

    MPI_Bcast(&n_numbers, 1, MPI_INT, 0, MPI_COMM_WORLD);
    rank += 1;
    printf("I'm process %d out of %d. My number is: %.2f\n", rank, size, buf[0]);

    print_message("*************PROPAGATION BETWEEN ROWS****************", rank);
    send_by_rows(rank, sqrt(n_numbers), &buf[0]);
    print_message("*************PROPAGATION BETWEEN COLUMNS****************", rank);
    send_by_columns(rank, sqrt(n_numbers), &buf[0]);

    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 1) printf("\n\n*************************AFTER COMPUTING SMALLER NUMBER********************\nThe smallest number is %.2f\n", buf[0]);
    MPI_Finalize();
    return EXIT_SUCCESS;
}

void send_numbers(double *num_array, int n_numbers){
    for(int i = 1; i < n_numbers; i++){
        MPI_Send((num_array + i), 1, MPI_DOUBLE, i, i, MPI_COMM_WORLD);
    }
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

void print_message(char *string, int rank){
    MPI_Barrier(MPI_COMM_WORLD);
    if(rank == 1) printf("\n\n%s\n", string);
    MPI_Barrier(MPI_COMM_WORLD);
}
