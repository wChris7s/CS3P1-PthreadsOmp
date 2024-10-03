#include <stdio.h>
#include <stdlib.h>

/*
 * gcc -g -Wall -o odd_even odd_even.c
 * odd_even <n> <g|i>
 * n: number of elements in list
 * g: generate list using a random number generator
 * i: user input list
 */

const int RMAX = 100;

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

   get_args(argc, argv, &n, &g_i);
   a = (int*) malloc(n*sizeof(int));
   if (g_i == 'g') {
      generate_list(a, n);
      print_list(a, n, "Before sort");
   } else {
      read_list(a, n);
   }

   odd_even_sort(a, n);
   print_list(a, n, "After sort");
   free(a);
   return 0;
}

void usage(char* prog_name) {
   fprintf(stderr, "usage:   %s <n> <g|i>\n", prog_name);
   fprintf(stderr, "   n:   number of elements in list\n");
   fprintf(stderr, "  'g':  generate list using a random number generator\n");
   fprintf(stderr, "  'i':  user input list\n");
}

void get_args(int argc, char* argv[], int* n_p, char* g_i_p) {
   if (argc != 3 ) {
      usage(argv[0]);
      exit(0);
   }
   *n_p = atoi(argv[1]);
   *g_i_p = argv[2][0];
   if (*n_p <= 0 || (*g_i_p != 'g' && *g_i_p != 'i') ) {
      usage(argv[0]);
      exit(0);
   }
}

void generate_list(int a[], int n) {
   int i;
   srandom(0);
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

void odd_even_sort(int  a[], int n) {
   int phase, i, temp;
   printf("n: %d\n", n);
   for (phase = 0; phase < n; phase++) 
      if (phase % 2 == 0) {
         for (i = 1; i < n; i += 2) {
            printf("even (i): %d\n", i);
            if (a[i-1] > a[i]) {
               temp = a[i];
               a[i] = a[i-1];
               a[i-1] = temp;
            }
         }
      } else {
         for (i = 1; i < n-1; i += 2) {
            printf("odd (i): %d\n", i);
            if (a[i] > a[i+1]) {
               temp = a[i];
               a[i] = a[i+1];
               a[i+1] = temp;
            }
         }
      }
}
