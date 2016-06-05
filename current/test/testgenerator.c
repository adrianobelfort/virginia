#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define image(x,y) pixels[x*width+y]

int main(int argc, char* argv[])
{
	srand(time(NULL));

	int width, heigth;
	int x, y, i, max;
	char filename[50], fullname[50];

	FILE *out;

	if (argc < 5)
    {
        printf("Usage: ./%s FILENAME {without extension} WIDTH HEIGHT FRAMES\n", argv[0]);
        return 1;
    }

	strcpy(filename, argv[1]);
	strcpy(fullname, argv[1]);
	strcat(fullname, ".in");

	out = fopen(fullname, "wb");
	if (out == NULL)
	{
		strcpy(fullname, argv[1]);
		strcat(fullname, ".out");
		perror(fullname);
		exit(EXIT_FAILURE);
	}

	width = atoi(argv[2]);
	heigth = atoi(argv[3]);
	//width = rand() % 512 + 1536;
	//heigth = rand() % 384 + 1152;

	fwrite(&width, sizeof(width), 1, out);
	fwrite(&heigth, sizeof(heigth), 1, out);
	fflush(out);

	int *pixels = (int*) malloc(heigth * width * sizeof(int));

	max = atoi(argv[4]);
	for (i = 0; i < max; i++) {
		for (y = 0; y < heigth; y++) {
			for (x = 0; x < width; x++) {
				image(y,x) = rand();
			}
		}
		fwrite(pixels, heigth * width * sizeof(int), 1, out);
	}

	free(pixels);
	fflush(out);
	fclose(out);

	return EXIT_SUCCESS;
}
