#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define FILENAME "numbers.dat"
#define MAXLINE 255

void send_numbers();

int main(int argc, char **argv){

    int rank, size, i, side;
    int buf[1];
    char *numbers;
    FILE *fp = NULL;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0){
        numbers = malloc(MAXLINE * sizeof(char));

        if((fp = fopen(FILENAME, "r")) == NULL){
            fprintf(stderr, "Error opening file: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        if(fgets(numbers, MAXLINE, fp) == NULL){
            fprintf(stderr, "Error reading numbers from file: %s\n", strerror(errno));
            return EXIT_FAILURE;
        }

        fclose(fp);
        
        printf("Numbers: %s\n", numbers);
        buf[0] = atoi(strtok(numbers, ","));
        send_numbers();
        free(numbers);
        
    }
    else{
        MPI_Recv(buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    }

    printf("I'm process %d out of %d. My number is: %d\n", (rank + 1), size, buf[0]);

    MPI_Finalize();
    return EXIT_SUCCESS;
}

void send_numbers(){
    int number;
    char *token = malloc(MAXLINE * sizeof(char));
    for(int i = 1; (token = strtok(NULL, ",")); i++){
        number = atoi(token);
        MPI_Send(&number, 1, MPI_INT, i, i, MPI_COMM_WORLD);
    }
    free(token);
}
