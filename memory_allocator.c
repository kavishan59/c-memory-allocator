#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

void *malloc_custom(size_t size)
{
  void *p;
  p = sbrk(0);
  if (sbrk(size) == (void *)-1)
    return NULL;
  return p;
}

int main()
{
  int *arr= malloc_custom(sizeof(int) * 4);
  arr[0] = 0;
  arr[1] = 1;
  arr[2] = 2;
  arr[3] = 3;
 
  for(int i = 0; i < 4 ; i++)
  {
    printf("%d ",arr[i]);
  }
  return 0; 
}
