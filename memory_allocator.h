#ifndef MEMORY_ALLOCATOR_H
#define MEMORY_ALLOCATOR_H


#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define BLOCK_SIZE sizeof(t_block)


typedef struct s_block {
  size_t size; 
  struct s_block *next; // next chunk of memory block (meta-data + requested size)
  struct s_block *prev;
  int free;
  void *ptr; //poin to the data block , used to check if the addr is allocated memory or not
  char data[1]; //trick to access the first byte of the memory allocated
} t_block;


void *ft_malloc(size_t size);
void *calloc(size_t number, size_t size);
void ft_free(void *p);
void *ft_realloc(void *p, size_t size);
void print_heap();


#endif // !MEMORY_ALLOCATOR_H

