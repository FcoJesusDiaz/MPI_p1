#define FILENAME "numbers.dat"
#define MAX_ARRAY 255


void distribute_numbers(int rank, int size, int *n_numbers, int *num_available, double buf[]);
void send_numbers(double *num_array, int n_numbers);
void print_message(char *string, int rank);