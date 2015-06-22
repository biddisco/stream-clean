#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <omp.h>

double min(double x, double y) {
    return x<y ? x : y;
}

double max(double x, double y) {
    return x>y ? x : y;
}

typedef double stream_t;

static double avgtime[4] = {0},
              maxtime[4] = {0},
              mintime[4] = {100000.,100000.,100000.,100000.};

static char *label[4] =
{
    "Copy:      ",
    "Scale:     ",
    "Add:       ",
    "Triad:     "
};

double mysecond() {
    return omp_get_wtime();
}

int main(int argc, char** argv) {
    int         BytesPerWord;
    int         k;
    ssize_t     j;
    stream_t    scalar;
    const size_t ntimes = 10;
    size_t array_size;
    double      times[4][ntimes];

    int pow = 8;
    if(argc>1) {
        sscanf(argv[1], "%d", &pow);
        if(pow<4 || pow>15) {
            printf("error: pass an integer in the range 4:15 as argument\n");
            printf("       you passed %d\n", pow);
            exit(1);
        }
    }
    // use prime factors so that work will divide nicely
    // for any number of threads in the range [1,16]
    const size_t n = 3*3*5*7*11*13*(1<<pow);

    stream_t *a, *b, *c;
    array_size = sizeof(stream_t)*n;
    posix_memalign((void**)&a, 64, array_size);
    posix_memalign((void**)&b, 64, array_size);
    posix_memalign((void**)&c, 64, array_size);

    double bytes[4];
    bytes[0] = 2 * array_size;
    bytes[1] = 2 * array_size;
    bytes[2] = 3 * array_size;
    bytes[3] = 3 * array_size;

    printf("------------------------------------\n");
    BytesPerWord = sizeof(stream_t);
    printf("Array size = %llu (elements).\n" , (unsigned long long) n);
    printf("Memory per array = %.1f MiB (= %.1f GiB).\n", 
    BytesPerWord * ( (double) n / 1024.0/1024.0),
    BytesPerWord * ( (double) n / 1024.0/1024.0/1024.0));
    printf("Each kernel will be executed %d times.\n", ntimes);
    printf ("Number of Threads requested = %i\n", omp_get_max_threads());

    /* Get initial value for system clock. */
    #pragma omp parallel for
    for (j=0; j<n; j++) {
        a[j] = 1.0;
        b[j] = 2.0;
        c[j] = 0.0;
    }

    printf("------------------------------------\n");

    #pragma omp parallel for
    for (j = 0; j < n; j++)
        a[j] = 2.0E0 * a[j];

    scalar = 3.0;
    for (k=0; k<ntimes; k++)
    {
        times[0][k] = mysecond();
            #pragma omp parallel for
        #ifdef USE_NONTEMPORAL
            #pragma vector nontemporal
        #endif
            for (j=0; j<n; j++)
                c[j] = a[j];
        times[0][k] = mysecond() - times[0][k];

        times[1][k] = mysecond();
            #pragma omp parallel for
        #ifdef USE_NONTEMPORAL
            #pragma vector nontemporal
        #endif
            for (j=0; j<n; j++)
                b[j] = scalar*c[j];
        times[1][k] = mysecond() - times[1][k];

        times[2][k] = mysecond();
            #pragma omp parallel for
        #ifdef USE_NONTEMPORAL
            #pragma vector nontemporal
        #endif
            for (j=0; j<n; j++)
                c[j] = a[j]+b[j];
        times[2][k] = mysecond() - times[2][k];

        times[3][k] = mysecond();
            #pragma omp parallel for
        #ifdef USE_NONTEMPORAL
            #pragma vector nontemporal
        #endif
            for (j=0; j<n; j++)
                a[j] = b[j]+scalar*c[j];
        times[3][k] = mysecond() - times[3][k];
    }

    /*  --- SUMMARY --- */

    for (k=1; k<ntimes; k++) /* note -- skip first iteration */
    {
        for (j=0; j<4; j++)
        {
            avgtime[j] = avgtime[j] + times[j][k];
            mintime[j] = min(mintime[j], times[j][k]);
            maxtime[j] = max(maxtime[j], times[j][k]);
        }
    }

    printf("Function    Best Rate MB/s  Avg time     Min time     Max time\n");
    for (j=0; j<4; j++) {
        avgtime[j] = avgtime[j]/(double)(ntimes-1);

        printf("%s%12.1f  %11.6f  %11.6f  %11.6f\n", label[j],
           1.0E-06 * bytes[j]/mintime[j],
           avgtime[j],
           mintime[j],
           maxtime[j]);
    }
    printf("------------------------------------\n");

    return 0;
}

