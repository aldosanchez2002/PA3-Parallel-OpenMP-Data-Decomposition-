// OpenMP quicksort; not necessarily efficient
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void swap(int *yi, int *yj)
{
    int tmp = *yi;
    *yi = *yj;
    *yj = tmp;
}

int separate(int *x, int low, int high)
{
    int i, pivot, last;
    // pivot = x[low];
    pivot = x[(low+high)/2];
    swap(x+low,x+high);
    last = low;
    #pragma omp parallel for
    for (i = low; i < high; i++) {
        if (x[i] <= pivot) {
            swap(x+last,x+i);
            last += 1;
        }
    }
    swap(x+last,x+high);
    return last;
}

// quicksort of the array z, elements zstart through zend; set the
// latter to 0 and m-1 in first call, where m is the length of z;
// firstcall is 1 or 0, according to whether this is the first of the
// recursive calls
void qs(int *z, int zstart, int zend, int firstcall)
{
        int part;
        if (firstcall == 1) {
                #pragma omp single nowait
                qs(z,0,zend,0);
        } else {
            if (zstart < zend) {
                part = separate(z,zstart,zend);
                #pragma parallel
                    #pragma omp task firstprivate(z,zstart,part)
                    {
                    qs(z,zstart,part-1,0);
                    }
                    #pragma omp task firstprivate(z,zend,part)
                    {
                    qs(z,part+1,zend,0);
                    }
        }
    }
}

int compare(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

// test code
int main(int argc, char **argv)
{
    int i, n, *w, *w_copy;
    double start_qs, end_qs, start_qsort, end_qsort;

    if (argc < 2)
    {
        printf("Usage: omp_quicksort <n>, where n is the number of items to be sorted\n");
        exit(-1);
    }

    n = atoi(argv[1]);
    w = malloc(n * sizeof(int));
    w_copy = malloc(n * sizeof(int));

    // Initialize arrays
    for (i = 0; i < n; i++)
    {
        w[i] = rand();
        w_copy[i] = w[i];
    }

    // Measure OpenMP quicksort time
    
    start_qs = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single nowait
        qs(w, 0, n-1, 1);
    }
    end_qs = omp_get_wtime();

    // Measure qsort time
    start_qsort = omp_get_wtime();
    qsort(w_copy, n, sizeof(int), compare);
    end_qsort = omp_get_wtime();

    // Print sorted arrays if n < 25
    if (n < 25)
    {
        printf("OpenMP Quicksort Result:\n");
        for (i = 0; i < n; i++)
            printf("%d\n", w[i]);

        printf("\nqsort Result:\n");
        for (i = 0; i < n; i++)
            printf("%d\n", w_copy[i]);
    }

    // Print elapsed time for each algorithm
    printf("\nElapsed time for OpenMP Quicksort: %f sec\n", end_qs - start_qs);
    printf("Elapsed time for qsort: %f sec\n", end_qsort - start_qsort);

    // Free allocated memory
    free(w);
    free(w_copy);

    return 0;
}
/// ./omp_quicksort 1000000
