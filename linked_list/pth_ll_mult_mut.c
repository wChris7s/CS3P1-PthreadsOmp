#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "headers/my_rand.h"
#include "headers/timer.h"

/*
 * gcc -g -Wall -I. -o pth_ll_mult_mut pth_ll_mult_mut.c my_rand.c -lpthread
 * ./pth_ll_mult_mut <thread_count>
 */

const int MAX_KEY = 100000000;

const int IN_LIST = 1;
const int EMPTY_LIST = -1;
const int END_OF_LIST = 0;

struct list_node_s {
   int    data;
   pthread_mutex_t mutex;
   struct list_node_s* next;
};

struct list_node_s* head = NULL;  
pthread_mutex_t head_mutex;
int         thread_count;
int         total_ops;
double      insert_percent;
double      search_percent;
double      delete_percent;
pthread_mutex_t count_mutex;
int         member_total=0, insert_total=0, delete_total=0;

void        usage(char* prog_name);
void        get_input(int* inserts_in_main_p);
void*       thread_work(void* rank);

void        Init_ptrs(struct list_node_s** curr_pp,    struct list_node_s** pred_pp);
int         Advance_ptrs(struct list_node_s** curr_pp, struct list_node_s** pred_pp);

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
   thread_count = strtol(argv[1], NULL, 10);
   get_input(&inserts_in_main);

   i = attempts = 0;
   pthread_mutex_init(&head_mutex, NULL);
   while ( i < inserts_in_main && attempts < 2*inserts_in_main ) {
      key = my_rand(&seed) % MAX_KEY;
      success = insert(key);
      attempts++;
      if (success) i++;
   }
   printf("Inserted %ld keys in empty list\n", i);

   thread_handles = malloc(thread_count*sizeof(pthread_t));
   pthread_mutex_init(&count_mutex, NULL);

   GET_TIME(start);
   for (i = 0; i < thread_count; i++)
      pthread_create(&thread_handles[i], NULL, thread_work, (void*) i);

   for (i = 0; i < thread_count; i++)
      pthread_join(thread_handles[i], NULL);
   GET_TIME(finish);
   printf("Elapsed time = %e seconds\n", finish - start);
   printf("Total ops = %d\n", total_ops);
   printf("member ops = %d\n", member_total);
   printf("insert ops = %d\n", insert_total);
   printf("delete ops = %d\n", delete_total);

   free_list();
   pthread_mutex_destroy(&head_mutex);
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
   printf("How many total ops should the threads execute?\n");
   scanf("%d", &total_ops);
   printf("Percent of ops that should be searches? (between 0 and 1)\n");
   scanf("%lf", &search_percent);
   printf("Percent of ops that should be inserts? (between 0 and 1)\n");
   scanf("%lf", &insert_percent);
   delete_percent = 1.0 - (search_percent + insert_percent);
}

void Init_ptrs(struct list_node_s** curr_pp, struct list_node_s** pred_pp) {
   *pred_pp = NULL;
   pthread_mutex_lock(&head_mutex);
   *curr_pp = head;
   if (*curr_pp != NULL)
      pthread_mutex_lock(&((*curr_pp)->mutex));
}

int Advance_ptrs(struct list_node_s** curr_pp, struct list_node_s** pred_pp) {
   int rv = IN_LIST;
   struct list_node_s* curr_p = *curr_pp;
   struct list_node_s* pred_p = *pred_pp;
   if (curr_p == NULL) {
      if (pred_p == NULL) {
         pthread_mutex_unlock(&head_mutex);
         return EMPTY_LIST;
       }
      return END_OF_LIST;
   }
   if (curr_p->next != NULL) {
      pthread_mutex_lock(&(curr_p->next->mutex));
   } else {
      rv = END_OF_LIST;
   }
   if (pred_p != NULL) {
      pthread_mutex_unlock(&(pred_p->mutex));
   }
   else {
      pthread_mutex_unlock(&head_mutex);
   }
   *pred_pp = curr_p;
   *curr_pp = curr_p->next;
   return rv;
}

int insert(int value) {
   struct list_node_s* curr;
   struct list_node_s* pred;
   struct list_node_s* temp;
   int rv = 1;
   Init_ptrs(&curr, &pred);
   while (curr != NULL && curr->data < value) {
      Advance_ptrs(&curr, &pred);
   }
   if (curr == NULL || curr->data > value) {
      temp = malloc(sizeof(struct list_node_s));
      pthread_mutex_init(&(temp->mutex), NULL);
      temp->data = value;
      temp->next = curr;
      if (curr != NULL) 
         pthread_mutex_unlock(&(curr->mutex));
      if (pred == NULL) {
         head = temp;
         pthread_mutex_unlock(&head_mutex);
      } else {
         pred->next = temp;
         pthread_mutex_unlock(&(pred->mutex));
      }
   } else {
      if (curr != NULL) 
         pthread_mutex_unlock(&(curr->mutex));
      if (pred != NULL)
         pthread_mutex_unlock(&(pred->mutex));
      else
         pthread_mutex_unlock(&head_mutex);
      rv = 0;
   }
   return rv;
}

void print(void) {
   printf("list = ");
   struct list_node_s* temp = head;
   while (temp != (struct list_node_s*) NULL) {
      printf("%d ", temp->data);
      temp = temp->next;
   }
   printf("\n");
}

int find(int value) {
   struct list_node_s *temp, *old_temp;
   pthread_mutex_lock(&head_mutex);
   temp = head;
   if (temp != NULL) pthread_mutex_lock(&(temp->mutex));
   pthread_mutex_unlock(&head_mutex);
   while (temp != NULL && temp->data < value) {
      if (temp->next != NULL) 
         pthread_mutex_lock(&(temp->next->mutex));
      old_temp = temp;
      temp = temp->next;
      pthread_mutex_unlock(&(old_temp->mutex));
   }
   if (temp == NULL || temp->data > value) {
      if (temp != NULL) {
         pthread_mutex_unlock(&(temp->mutex));
      }
      return 0;
   }
   pthread_mutex_unlock(&(temp->mutex));
   return 1;
}

int remove(int value) {
   struct list_node_s* curr;
   struct list_node_s* pred;
   int rv = 1;
   Init_ptrs(&curr, &pred);
   while (curr != NULL && curr->data < value) {
      Advance_ptrs(&curr, &pred);
   }
   if (curr != NULL && curr->data == value) {
      if (pred == NULL) {
         head = curr->next;
         pthread_mutex_unlock(&head_mutex);
         pthread_mutex_unlock(&(curr->mutex));
         pthread_mutex_destroy(&(curr->mutex));
         free(curr);
      } else {
         pred->next = curr->next;
         pthread_mutex_unlock(&(pred->mutex));
         pthread_mutex_unlock(&(curr->mutex));
         pthread_mutex_destroy(&(curr->mutex));
         free(curr);
      }
   } else {
      if (pred != NULL)
         pthread_mutex_unlock(&(pred->mutex));
      if (curr != NULL)
         pthread_mutex_unlock(&(curr->mutex));
      if (curr == head)
         pthread_mutex_unlock(&head_mutex);
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

int  is_empty(void) {
   return head == NULL ? 1 : 0;
}

void* thread_work(void* rank) {
   long my_rank = (long) rank;
   int i, val;
   double which_op;
   unsigned seed = my_rank + 1;
   int my_member=0, my_insert=0, my_delete=0;
   int ops_per_thread = total_ops/thread_count;
   for (i = 0; i < ops_per_thread; i++) {
      which_op = my_drand(&seed);
      val = my_rand(&seed) % MAX_KEY;
      if (which_op < search_percent) {
         find(val);
         my_member++;
      } else if (which_op < search_percent + insert_percent) {
         insert(val);
         my_insert++;
      } else {
         remove(val);
         my_delete++;
      }
   }
   pthread_mutex_lock(&count_mutex);
   member_total += my_member;
   insert_total += my_insert;
   delete_total += my_delete;
   pthread_mutex_unlock(&count_mutex);
   return NULL;
}
