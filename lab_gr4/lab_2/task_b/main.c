#include <stdio.h>
#include <stdlib.h>
#include "array.h"

int main ()
{
	Array tmp;
	Array a = array_new(10);
	for(int i = 0; i < 100; i++)
		array_insertBack(&a, 10);
	array_destroy(a);
	return 0;
} 
