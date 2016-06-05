#include <stdio.h>

int main (int argc, char* argv[])
{
	int i, j, k = 0, m = 0;

	for (i = 0; i < 100000000; i++)
	{
		for (j = 0; j < 10; j++) k++;

		if ((m+1) % 2 > m) m++;
		else m--;
	}

	return 0;
}
