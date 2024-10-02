#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define CNT_MAX_SIZE 2000
#define NUM_THREADS 4

int counter = 0;

int flag = 0;

pthread_mutex_t mutex;

void* fx_increment_counter_mutex(void* rank_thread) {
  #if DEBUG
  const long rank = (long)rank_thread;
  #endif
  for (int i = 0; i < CNT_MAX_SIZE; i++) {
    pthread_mutex_lock(&mutex);
    counter++;
    #if DEBUG
    printf("Thread [%ld] add [ 1 ] to counter => counter = [ %d ]\n", rank, counter);
    #endif
    pthread_mutex_unlock(&mutex);
  }
  return NULL;
}

void* fx_increment_counter_bwt(void *rank_thread) {
  #if DEBUG
  const long rank = (long)rank_thread;
  #endif
  for (int i = 0; i < CNT_MAX_SIZE; i++) {
    while (flag == 1) {}
    flag = 1;
    counter++;
    #if DEBUG
    printf("Thread [%ld] add [ 1 ] to counter => counter = [ %d ]\n", rank, counter);
    #endif
    flag = 0;
  }
  return NULL;
}

void* err_increment_counter(void* rank_thread) {
  #if DEBUG
  const long rank = (long)rank_thread;
  #endif

  /* En algún momento puede que dos hilos o más quieran modificar
   * el valor de counter, pero se produzca incoherencia, ya que todos
   * acceden al mismo tiempo provocando este error. */

  for (int i = 0; i < CNT_MAX_SIZE; i++) {
    counter++;
    #if DEBUG
    printf("Thread [%ld] add [ 1 ] to counter => counter = [ %d ]\n", rank, counter);
    #endif
  }
  return NULL;
}

int main() {
  pthread_t threads[NUM_THREADS];

  for (long i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], ((void *) 0), err_increment_counter, (void*)i);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], ((void *) 0));
  }

  printf("Counter = %d\n", counter);
  counter = 0;

  for (long i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], ((void *) 0), fx_increment_counter_mutex, (void*)i);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], ((void *) 0));
  }

  printf("Counter = %d\n", counter);
  counter = 0;
  pthread_mutex_destroy(&mutex);

  for (long i = 0; i < NUM_THREADS; i++) {
    pthread_create(&threads[i], ((void *) 0), fx_increment_counter_bwt, (void*)i);
  }

  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], ((void *) 0));
  }

  printf("Counter = %d\n", counter);

  return 0;
}
