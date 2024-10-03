#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "headers/my_rand.h"
#include "headers/timer.h"

/*
 * gcc -g -Wall -o pth_ll_rwl pth_ll_rwl.c my_rand.c -lpthread
 * ./pth_ll_rwl <thread_count>
 */

const int MAX_KEY = 100000000;

struct list_node_s {
   int    data;
   struct list_node_s* next;
};

struct      list_node_s* head = NULL;  
int         thread_count;
int         total_ops;
double      insert_percent;
double      search_percent;
double      delete_percent;
pthread_rwlock_t    rwlock;
pthread_mutex_t     count_mutex;
int         member_count = 0, insert_count = 0, delete_count = 0;

void        usage(char* prog_name);
void        get_input(int* inserts_in_main_p);
void*       thread_work(void* rank);


int         insert(int value);
void        print(void);
int         find(int value);
int         remove(int value);
void        free_list(void);
int         is_empty(void);

int main(int argc, char* argv[]) {
   long i; 
   int key, success, attempts;
   pthread_t* thread_handles;
   int inserts_in_main;
   unsigned seed = 1;
   double start, finish;

   if (argc != 2) usage(argv[0]);
   thread_count = strtol(argv[1],NULL,10);
   get_input(&inserts_in_main);

   i = attempts = 0;
   while ( i < inserts_in_main && attempts < 2*inserts_in_main ) {
      key = my_rand(&seed) % MAX_KEY;
      success = insert(key);
      attempts++;
      if (success) i++;
   }
   printf("Inserted %ld keys in empty list\n", i);

   thread_handles = malloc(thread_count*sizeof(pthread_t));
   pthread_mutex_init(&count_mutex, NULL);
   pthread_rwlock_init(&rwlock, NULL);

   GET_TIME(start);
   for (i = 0; i < thread_count; i++)
      pthread_create(&thread_handles[i], NULL, thread_work, (void*) i);

   for (i = 0; i < thread_count; i++)
      pthread_join(thread_handles[i], NULL);
   GET_TIME(finish);
   printf("Elapsed time = %e seconds\n", finish - start);
   printf("Total ops = %d\n", total_ops);
   printf("member ops = %d\n", member_count);
   printf("insert ops = %d\n", insert_count);
   printf("delete ops = %d\n", delete_count);

   free_list();
   pthread_rwlock_destroy(&rwlock);
   pthread_mutex_destroy(&count_mutex);
   free(thread_handles);

   return 0;
}

void usage(char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count>\n", prog_name);
   exit(0);
}

void get_input(int* inserts_in_main_p) {
   printf("How many keys should be inserted in the main thread?\n");
   scanf("%d", inserts_in_main_p);
   printf("How many ops total should be executed?\n");
   scanf("%d", &total_ops);
   printf("Percent of ops that should be searches? (between 0 and 1)\n");
   scanf("%lf", &search_percent);
   printf("Percent of ops that should be inserts? (between 0 and 1)\n");
   scanf("%lf", &insert_percent);
   delete_percent = 1.0 - (search_percent + insert_percent);
}

int insert(int value) {
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   struct list_node_s* temp;
   int rv = 1;
   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }
   if (curr == NULL || curr->data > value) {
      temp = malloc(sizeof(struct list_node_s));
      temp->data = value;
      temp->next = curr;
      if (pred == NULL)
         head = temp;
      else
         pred->next = temp;
   } else {
      rv = 0;
   }
   return rv;
}

void print(void) {
   struct list_node_s* temp;
   printf("list = ");
   temp = head;
   while (temp != (struct list_node_s*) NULL) {
      printf("%d ", temp->data);
      temp = temp->next;
   }
   printf("\n");
}

int  find(int value) {
   struct list_node_s* curr_p;
   for(curr_p = head; curr_p != NULL && curr_p->data < value; curr_p = curr_p->next) {}
   if (curr_p == NULL || curr_p->data > value) {
      return 0;
   }
   return 1;
}

int remove(int value) {
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   int rv = 1;

   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }
   
   if (curr != NULL && curr->data == value) {
      if (pred == NULL) {
         head = curr->next;
         free(curr);
      } else { 
         pred->next = curr->next;
         free(curr);
      }
   } else {
      rv = 0;
   }

   return rv;
}

void free_list(void) {
   struct list_node_s* current;
   struct list_node_s* following;
   if (is_empty()) return;
   current = head; 
   following = current->next;
   while (following != NULL) {
      free(current);
      current = following;
      following = current->next;
   }
   free(current);
}
int is_empty(void) {
   return head == NULL ? 1 : 0;
}

void* thread_work(void* rank) {
   long my_rank = (long) rank;
   int i, val;
   double which_op;
   unsigned seed = my_rank + 1;
   int my_member_count = 0, my_insert_count=0, my_delete_count=0;
   int ops_per_thread = total_ops/thread_count;

   for (i = 0; i < ops_per_thread; i++) {
      which_op = my_drand(&seed);
      val = my_rand(&seed) % MAX_KEY;
      if (which_op < search_percent) {
         pthread_rwlock_rdlock(&rwlock);
         find(val);
         pthread_rwlock_unlock(&rwlock);
         my_member_count++;
      } else if (which_op < search_percent + insert_percent) {
         pthread_rwlock_wrlock(&rwlock);
         insert(val);
         pthread_rwlock_unlock(&rwlock);
         my_insert_count++;
      } else {
         pthread_rwlock_wrlock(&rwlock);
         remove(val);
         pthread_rwlock_unlock(&rwlock);
         my_delete_count++;
      }
   }
   pthread_mutex_lock(&count_mutex);
   member_count += my_member_count;
   insert_count += my_insert_count;
   delete_count += my_delete_count;
   pthread_mutex_unlock(&count_mutex);
   return NULL;
}
