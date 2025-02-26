#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>

#define BLOCK_SIZE 12 

void *base=NULL;  //adress of start of the heap in our virtual memory

typedef struct s_block {
  size_t size; 
  struct s_block *next; // next chunk of memory block (meta-data + requested size)
  int free;
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
    last->next = b;
  b->free = 0;
  return (b);
}

// split the chunk of memory if the size requested is small compared the size of chunk
void split_block(t_block *b, size_t s)
{
  t_block *new;
  new = (t_block*)b->data + s;
  new->size = b->size - s - BLOCK_SIZE;
  new->next = b->next;
  new->free = 1;
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


