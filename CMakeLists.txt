cmake_minimum_required(VERSION 3.29)
project(CS3P1_Pthreads C)

set(CMAKE_C_STANDARD 23)

add_compile_options(-g -Wall)

include_directories(introduction/my_rand.h introduction/timer.h)

add_executable(CS3P1_Pthreads
        linked_list/pth_hello_world.c
        linked_list/pth_busy_waiting.c
        linked_list/pth_mutex.c
        linked_list/pth_semaphore.c
        introduction/linked_list.c
        introduction/pth_ll_mult_mut.c
        introduction/pth_ll_one_mut.c
        introduction/pth_ll_rwl.c
        introduction/my_rand.c
        odd_even/odd_even.c
        odd_even/omp_odd_even1.c
        odd_even/omp_odd_even2.c)

target_link_libraries(CS3P1_Pthreads pthread)