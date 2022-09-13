#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	long xy_size = 1000*1000*500; // 4 GB (sizeof(long) = 8 bytes)
	long x_dim = 10000;
	long y_dim = xy_size/x_dim;
	long** matrix = malloc(y_dim*sizeof(long*));
	for(long y = 0; y < y_dim; y++){
		matrix[y] = malloc(x_dim*sizeof(long));
		memset(matrix[y], 0, x_dim*sizeof(long));
	}
	printf("Allocation complete (press any key to continue...)\n");
	getchar();
	return 0;
}


/*

[
[1, 2, 3, 4],
[5, 6, 7, 8]
]

[
[1, 2],
[3, 4],
[5, 6],
[7, 8]
]

*/
