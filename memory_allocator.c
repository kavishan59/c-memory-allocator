#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define BLOCK_SIZE sizeof(t_block) 

void *base=NULL;  //adress of start of the heap in our virtual memory

typedef struct s_block {
  size_t size; 
  struct s_block *next; // next chunk of memory block (meta-data + requested size)
  struct s_block *prev;
  int free;
  void *ptr; //poin to the data block , used to check if the addr is allocated memory or not
  char data[1]; //trick to access the first byte of the memory allocated
} t_block;


//find the next available chunk of memory
t_block *find_block(t_block **last, size_t size)
{
  t_block *b = base;
  while(b && !(b->free && b->size >= size))
  {
    *last = b;
    b = b->next;
  }
  return (b);
}

// create (if not found fitting chunk of memory) a chunk of memory
t_block *extend_heap(t_block *last, size_t s)
{
  t_block *b;

  b = sbrk(0);
  if (sbrk(BLOCK_SIZE + s) == (void*)-1)
    return (NULL);
  b->size = s;
  b->next = NULL;
  if (last)
  {
    last->next = b;
    b->prev = last;
  }
  else
    b->prev = NULL; //first block in the list
  b->free = 0;
  b->ptr = b->data;
  return (b);
}

// split the chunk of memory if the size requested is small compared the size of chunk
void split_block(t_block *b, size_t s)
{
  t_block *new;
  new = (t_block*)((char *)b->data + s);
  new->size = b->size - s - BLOCK_SIZE;
  new->next = b->next;
  new->free = 1;
  new->ptr = new->data;
  new->prev = b;
  if (new->next)
    new->next->prev = new; //update the next block's prev pointer if it exists

  b->size = s;
  b->next = new;
}


void *ft_malloc(size_t size)
{
  t_block *b,*last;
  size_t s;

  s= (size + 3) & ~3; //align the size requested to be a multiple of 4(sizeof(int))
  if (base)
  {
    //first find a block
    last = base;
    b = find_block(&last, s);
    if (b)
    {
      // can we split ?
      if ((b->size - s) >= (BLOCK_SIZE + 4)) ///can we get our requested size + chunk of minimum size
        split_block(b, s);
      b->free = 0;
    }
    else 
    {
      //no fitting block, extend the heap
      b = extend_heap(last, s);
      if(!b)
        return(NULL);
    }
  }
  else 
  {
    //first time 
    b = extend_heap(NULL, s);
    if(!b)
      return(NULL);
    base = b; //start of the heap
  }
  b->ptr = b->data;
  return (b->data);
}

void *calloc(size_t number, size_t size)
{
  size_t *new;
  size_t s4,i;
  
  new = ft_malloc(number * size);
  if (new)
  {
    s4 = ((number * size) + 3) & ~3;   //align data to be multiple of 4
    for (i = 0; i < s4 ;i++)
      new[i] = 0;
  }
  return (new);
}


//fusion free right and left to avoid fragmentation (coalescing)
t_block *fusion(t_block *b)
{
  //merge with next free block
  while (b->next && b->next->free) //keep emrging until no free right blovk
  {
    b->size += BLOCK_SIZE + b->next->size;
    b->next = b->next->next;
    if (b->next)
      b->next->prev = b;
  }
  
  //merge with previous free block
  while (b->prev && b->prev->free)   //keep merging until no free left block
  {
    b->prev->size += BLOCK_SIZE + b->size;
    b->prev->next = b->next;
    if (b->next)
      b->next->prev = b->prev;
    b = b->prev;
  }
  return (b);
}

//get the block from addr
t_block *get_block(void *p)
{
  return (t_block *)((char *)p - offsetof(t_block, data));
}

//check if the addr inside the heap , and that the is it a block
int check_valid_addr(void *p)
{
  if(!base)
    return 0;

  void *heap_end = sbrk(0);
  t_block *b = get_block(p);

  if(p > base && p < heap_end && b->ptr == b->data)
    return 1;
  
  return 0;
}

void ft_free(void *p)
{
  if(!p || !check_valid_addr(p))
    return;
  
  t_block *b =get_block(p);
  b->free = 1;
  b = fusion(b);
  
  //if we are the last block , we release the memory
  if(!b->next)
  {
    if (b->prev)
      b->prev->next = NULL;
    else  //no more block
      base = NULL;

    brk(b);
  }
}

//debug function
void print_heap()
{
    t_block *b = base;
    printf("\n--- HEAP STATUS ---\n");
    while (b)
    {
        printf("[ %s | %zu bytes ] -> ", b->free ? "Free" : "Used", b->size);
        b = b->next;
    }
    printf("NULL\n");
}


