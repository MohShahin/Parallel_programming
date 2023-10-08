#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
//:::::::::::::::::::::::::::::::::
#define IMG_WIDTH 800
#define IMG_HEIGHT 800
#define MAX_ITERATIONS 1000
//===========================================================================================
// Calculate the Mandelbrot set for a given point
int mandelbrot(double x, double y) {
    double real = 0.0, imag = 0.0;
    int iterations = 0;
    while (real * real + imag * imag <= 4.0 && iterations < MAX_ITERATIONS) {
        double temp = real * real - imag * imag + x;
        imag = 2.0 * real * imag + y;
        real = temp;
        iterations++;
    }
    return iterations;
}
//--------------------------------------------------------------------------------------------------
int main(int argc, char** argv) {
    int proc_rank, proc_size;
    double start_time, end_time, parallel_execution_time;
    start_time = MPI_Wtime();
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_size);

    // Calculate the portion of the Mandelbrot set that this process is responsible for
    int rows_per_proc = IMG_HEIGHT / proc_size;
    int start_row = proc_rank * rows_per_proc;
    int end_row = (proc_rank + 1) * rows_per_proc;
    if (proc_rank == proc_size - 1) {
        end_row = IMG_HEIGHT;
    }

    int* image = (int*)malloc(IMG_WIDTH * (end_row - start_row) * sizeof(int)); //Buffer
    if (image == NULL) {
        printf("Error: Memory allocation failed.\n");
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    // Start parallel execution time measurement


    for (int j = start_row; j < end_row; j++) {
        for (int i = 0; i < IMG_WIDTH; i++) {
            double x = (i - IMG_WIDTH/2.0) * 4.0 / IMG_WIDTH;
            double y = (j - IMG_HEIGHT/2.0) * 4.0 / IMG_WIDTH;
            image[(j - start_row) * IMG_WIDTH + i] = mandelbrot(x, y);
        }
    }

    end_time = MPI_Wtime();
    parallel_execution_time = end_time - start_time;

    // Print parallel execution time (only once for rank 0)
    if (proc_rank == 0) {
        printf("Execution time: %f seconds\n", parallel_execution_time);
    }

    free(image);
    MPI_Finalize();

    return 0;
}

