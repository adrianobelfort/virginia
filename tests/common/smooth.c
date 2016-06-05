#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "solution.h"

void getTime(struct timespec* timestamp)
{
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, timestamp);
}

unsigned long long int timeDifference(struct timespec end, struct timespec start)
{
	long int secondsTime, nanoTime;
	long int BILLION = 1000000000;
	unsigned long long int wideTime;

	secondsTime = end.tv_sec - start.tv_sec;
	nanoTime = end.tv_nsec - start.tv_nsec;

	if (nanoTime < 0)
	{
		nanoTime = BILLION + nanoTime;
		secondsTime = secondsTime - 1;
	}

	wideTime = secondsTime * BILLION + nanoTime;

	return wideTime;
}

int main (int argc, char* argv[])
{
    unsigned long long int timeElapsed;
    char *dot, *slash, inputFilename[256], outputFilename[256], **newArgv;
    int i, result;
	struct timespec start, end;

    if (argc < 3)
    {
        printf("Usage: ./%s input_file output_path\n", argv[0]);
        return 1;
    }

    // Isolate the name of the input file by removing path tail and extension
    slash = strrchr(argv[1], '/');
    if (slash != NULL)
    {
        strcpy(inputFilename, slash + 1);
    }
    else
    {
        strcpy(inputFilename, argv[1]);
    }

    dot = strrchr(inputFilename, '.');
    if (dot != NULL)
    {
        dot[0] = '\0';
    }

    strcpy(outputFilename, argv[2]);
    strcat(outputFilename, "/");
    strcat(outputFilename, inputFilename);
    strcat(outputFilename, ".out");

    // Create a new argv array
    newArgv = (char**) malloc(argc * sizeof(char*));

    newArgv[0] = (char*) malloc((strlen(argv[0]) + 1) * sizeof(char));
    newArgv[1] = (char*) malloc((strlen(argv[1]) + 1) * sizeof(char));
    newArgv[2] = (char*) malloc((strlen(outputFilename) + 1) * sizeof(char));

    strcpy(newArgv[0], argv[0]);
    strcpy(newArgv[1], argv[1]);
    strcpy(newArgv[2], outputFilename);

    // Solution usage: ./solution input_file output_file

	getTime(&start);

    /****************************** SOLUTION ******************************************/
    result = solution(argc, newArgv);
    /*************************** END OF SOLUTION ***************************/

	getTime(&end);

    timeElapsed = timeDifference(end, start);

    printf("%s %llu\n", argv[1], timeElapsed);

    for (i = 0; i < 3; i++) free(newArgv[i]);
    free(newArgv);

    return 0;
}
