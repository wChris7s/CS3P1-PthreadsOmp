cmake_minimum_required(VERSION 3.29)
project(CS3P1_Pthreads C)

set(CMAKE_C_STANDARD 23)

add_compile_options(-g -Wall)

include_directories(linked_list/headers/my_rand.h linked_list/headers/timer.h)

add_executable(CS3P1_Pthreads
        linked_list/linked_list.c
        linked_list/pth_ll_mult_mut.c
        linked_list/pth_ll_one_mut.c
        linked_list/pth_ll_rwl.c
        linked_list/headers/my_rand.c
        odd_even/odd_even.c
        odd_even/omp_odd_even1.c
        odd_even/omp_odd_even2.c)

target_link_libraries(CS3P1_Pthreads pthread)