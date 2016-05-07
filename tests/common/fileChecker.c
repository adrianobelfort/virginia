#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* openFile(char* filename, char* extension, char* mode)
{
    FILE* filePointer;
    char fullname[50];

    strcpy(fullname, filename);
    strcat(fullname, extension);

    filePointer = fopen(fullname, mode);

    return filePointer;
}

int fileOpenError(FILE* filePointer)
{
    if (filePointer == NULL)
    {
        printf("Unable to open file\n");
        return 1;
    }
    else
    {
        return 0;
    }
}

int main (int argc, char* argv[])
{
    FILE *in;

    int width, height, frames, size;
    long int start, end, area;

    if (argc < 2)
    {
        printf("Usage: ./%s filename\n", argv[0]);
        return 1;
    }

    in = openFile(argv[1], "", "rb");
    if (fileOpenError(in)) return 2;

    printf("File: %s\n", argv[1]);

    // Reading initial file information and copying that to output file

    fread(&width, sizeof(width), 1, in);
    fread(&height, sizeof(height), 1, in);

    area = width * height;

    printf("Frame width: %d\nFrame height: %d\nFrame area: %ld\n", width, height, area);

    // Gathering information to compute frame area
    start = ftell(in);
    fseek(in, 0, SEEK_END);
    end = ftell(in);
    fseek(in, start, SEEK_SET);

    frames = (end - start) / (area * sizeof(int));

    printf("Number of frames in the file: %d\n", frames);

    fclose(in);
    return 0;
}
