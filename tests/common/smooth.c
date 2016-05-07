#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "solution.h"

int main (int argc, char* argv[])
{
    double timeElapsed;
    char *dot, *slash, inputFilename[256], outputFilename[256], **newArgv;
    int i, result;

    if (argc < 3)
    {
        printf("Usage: ./%s input_file output_path\n", argv[0]);
        return 1;
    }

    // Isolate the name of the input file by removing path trail and extension
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

    timeElapsed = omp_get_wtime();

    /****************************** SOLUTION ******************************************/
    result = solution(argc, newArgv);
    /*************************** END OF SOLUTION ***************************/

    timeElapsed = omp_get_wtime() - timeElapsed;

    printf("%s %lf\n", argv[1], timeElapsed);

    for (i = 0; i < 3; i++) free(newArgv[i]);
    free(newArgv);

    return 0;
}
