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
    pivot = x[low];
    swap(x+low,x+high);
    last = low;
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
    #pragma omp parallel
    {
        int part;
        if (firstcall == 1) {
            #pragma omp single nowait
            qs(z,0,zend,0);
        } else {
            if (zstart < zend) {
                part = separate(z,zstart,zend);
                #pragma omp task
                qs(z,zstart,part-1,0);
                #pragma omp task
                qs(z,part+1,zend,0);
            }
        }
    }
}

// test code
int main(int argc, char **argv)
{
    int i, n, *w;
    double start,end;
    if (argc < 2) {
        printf("Usage: omp_quicksort <n>, where n is the number of items to be sorted\n");
	exit(-1);
    }
    n = atoi(argv[1]);
    w = malloc(n*sizeof(int));
    for (i = 0; i < n; i++) w[i] = rand();
    start = omp_get_wtime();
    qs(w,0,n-1,1);
    end = omp_get_wtime();
    if (n < 25)
        for (i = 0; i < n; i++) printf("%d\n",w[i]);
    printf("Elapsed time: %f sec\n", end-start);
}
