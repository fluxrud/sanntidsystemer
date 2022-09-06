#include <stdlib.h>
#include <stdio.h>
#include "sort.h"


int main(int argc, char *argv[])
{
	int i = argc - 1;

	printf("\nArguments before sort: \n");

	while(i--)
		printf ("%s\n", argv[argc - i - 1]);
	
	sort(argc, argv);

	i = argc - 1;
	
	printf("\nArguments after  sort: \n");
	while(i--)
		printf ("%s\n", argv[argc - i - 1]);


	return 0;
}
