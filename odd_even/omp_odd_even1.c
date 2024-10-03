#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

/*
 * gcc -g -Wall -fopenmp -o omp_odd_even1 omp_odd_even1.c
 * ./omp_odd_even1 <thread count> <n> <g|i>
 * n: number of elements in list
 * g: generate list using a random number generator
 * i: user input list
 */

#ifdef DEBUG
const int RMAX = 100;
#else
const int RMAX = 10000000;
#endif

int thread_count;

void usage(char* prog_name);
void get_args(int argc, char* argv[], int* n_p, char* g_i_p);
void generate_list(int a[], int n);
void print_list(int a[], int n, char* title);
void read_list(int a[], int n);
void odd_even_sort(int a[], int n);

int main(int argc, char* argv[]) {
   int  n;
   char g_i;
   int* a;
   double start, finish;

   get_args(argc, argv, &n, &g_i);
   a = malloc(n*sizeof(int));
   if (g_i == 'g') {
      generate_list(a, n);
   } else {
      read_list(a, n);
   }

   start = omp_get_wtime();
   odd_even_sort(a, n);
   finish = omp_get_wtime();
   printf("Elapsed time = %e seconds\n", finish - start);
   free(a);
   return 0;
}

void usage(char* prog_name) {
   fprintf(stderr, "usage:   %s <thread count> <n> <g|i>\n", prog_name);
   fprintf(stderr, "   n:   number of elements in list\n");
   fprintf(stderr, "  'g':  generate list using a random number generator\n");
   fprintf(stderr, "  'i':  user input list\n");
}

void get_args(int argc, char* argv[], int* n_p, char* g_i_p) {
   if (argc != 4 ) {
      usage(argv[0]);
      exit(0);
   }
   thread_count = strtol(argv[1], NULL, 10);
   *n_p = strtol(argv[2], NULL, 10);
   *g_i_p = argv[3][0];
   if (*n_p <= 0 || (*g_i_p != 'g' && *g_i_p != 'i') ) {
      usage(argv[0]);
      exit(0);
   }
}

void generate_list(int a[], int n) {
   int i;
   srandom(1);
   for (i = 0; i < n; i++)
      a[i] = random() % RMAX;
}

void print_list(int a[], int n, char* title) {
   int i;
   printf("%s:\n", title);
   for (i = 0; i < n; i++)
      printf("%d ", a[i]);
   printf("\n\n");
}

void read_list(int a[], int n) {
   int i;
   printf("Please enter the elements of the list\n");
   for (i = 0; i < n; i++)
      scanf("%d", &a[i]);
}

void odd_even_sort(int a[], int n) {
   int phase, i, tmp;
   for (phase = 0; phase < n; phase++) {
      if (phase % 2 == 0)
         # pragma omp parallel for num_threads(thread_count) default(none) shared(a, n) private(i, tmp)
         for (i = 1; i < n; i += 2) {
            if (a[i-1] > a[i]) {
               tmp = a[i-1];
               a[i-1] = a[i];
               a[i] = tmp;
            }
         }
      else
         # pragma omp parallel for num_threads(thread_count) default(none) shared(a, n) private(i, tmp)
         for (i = 1; i < n-1; i += 2) {
            if (a[i] > a[i+1]) {
               tmp = a[i+1];
               a[i+1] = a[i];
               a[i] = tmp;
            }
         }
   }
}


