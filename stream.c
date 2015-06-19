# include <stdio.h>
# include <unistd.h>
# include <math.h>
# include <float.h>
# include <limits.h>
# include <sys/time.h>

#include <omp.h>

//static const size_t n = 10000000;
#define n (1<<23)

double min(double x, double y) {
    return x<y ? x : y;
}

double max(double x, double y) {
    return x>y ? x : y;
}


#define STREAM_TYPE double

static double avgtime[4] = {0},
              maxtime[4] = {0},
              mintime[4] = {FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX};

static char *label[4] = {
    "Copy:      ",
    "Scale:     ",
    "Add:       ",
    "Triad:     "};

double mysecond() {
    return omp_get_wtime();
}

int main() {
    int         BytesPerWord;
    int         k;
    ssize_t     j;
    STREAM_TYPE     scalar;
    const size_t ntimes = 10;
    double      times[4][ntimes];

    // the arrays have to become local variables if we are going to 
    STREAM_TYPE *restrict a;
    STREAM_TYPE *restrict b;
    STREAM_TYPE *restrict c;

    a = (STREAM_TYPE*)malloc(sizeof(STREAM_TYPE)*n);
    b = (STREAM_TYPE*)malloc(sizeof(STREAM_TYPE)*n);
    c = (STREAM_TYPE*)malloc(sizeof(STREAM_TYPE)*n);

    double bytes[4];
    bytes[0] = 2 * sizeof(STREAM_TYPE) * n;
    bytes[1] = 2 * sizeof(STREAM_TYPE) * n;
    bytes[2] = 3 * sizeof(STREAM_TYPE) * n;
    bytes[3] = 3 * sizeof(STREAM_TYPE) * n;

    /* --- SETUP --- determine precision and check timing --- */

    printf("------------------------------------\n");
    printf("STREAM version $Revision: 5.10 $\n");
    printf("------------------------------------\n");
    BytesPerWord = sizeof(STREAM_TYPE);
    printf("This system uses %d bytes per array element.\n",
    BytesPerWord);

    printf("------------------------------------\n");

    printf("Array size = %llu (elements), Offset = (elements)\n" , (unsigned long long) n);
    printf("Memory per array = %.1f MiB (= %.1f GiB).\n", 
    BytesPerWord * ( (double) n / 1024.0/1024.0),
    BytesPerWord * ( (double) n / 1024.0/1024.0/1024.0));
    printf("Total memory required = %.1f MiB (= %.1f GiB).\n",
    (3.0 * BytesPerWord) * ( (double) n / 1024.0/1024.),
    (3.0 * BytesPerWord) * ( (double) n / 1024.0/1024./1024.));
    printf("Each kernel will be executed %d times.\n", ntimes);
    printf(" The *best* time for each kernel (excluding the first iteration)\n"); 
    printf(" will be used to compute the reported bandwidth.\n");

    printf("------------------------------------\n");
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
            for (j=0; j<n; j++)
                c[j] = a[j];
        times[0][k] = mysecond() - times[0][k];

        times[1][k] = mysecond();
            #pragma omp parallel for
            for (j=0; j<n; j++)
                b[j] = scalar*c[j];
        times[1][k] = mysecond() - times[1][k];

        times[2][k] = mysecond();
            #pragma omp parallel for
            for (j=0; j<n; j++)
                c[j] = a[j]+b[j];
        times[2][k] = mysecond() - times[2][k];

        times[3][k] = mysecond();
            #pragma omp parallel for
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

