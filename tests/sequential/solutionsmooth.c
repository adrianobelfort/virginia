#include "solution.h"

// Insert here all the code your solution has, including all functions and
// definitions

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

/*

 This is your main function.

 The input file name to open is in argv[1]
 The output file to write is in argv[2]

 Its usage would be like
    ./solution input_file output_file

 Change your code accordingly

 DO NOT print anything to the standard output or you will break some scripts
 Instead, use fprintf(stderr, "something");

 */

int solution(int argc, char** argv)
{
    FILE *in, *out;

    int width, height, frames, size;
    long int start, end, area;
    char verbosity = 0;
    char filename[50];
    char *dot;
    int i, line, row, d, dx, dy;
    int *pixels, *filtered;
    double wtime;

    in = openFile(argv[1], "", "rb");
    if (fileOpenError(in)) return 2;

    out = openFile(argv[2], "", "wb");
    if (fileOpenError(out)) return 3;

    // Reading initial file information and copying that to output file

    fread(&width, sizeof(width), 1, in);
    fread(&height, sizeof(height), 1, in);

    area = width * height;

    if (verbosity)
    {
        printf("Frame width: %d\nFrame height: %d\nFrame area: %ld\n", width, height, area);
    }

    fwrite(&width, sizeof(width), 1, out);
    fwrite(&height, sizeof(height), 1, out);

    // Gathering information to compute frame area
    start = ftell(in);
    fseek(in, 0, SEEK_END);
    end = ftell(in);
    fseek(in, start, SEEK_SET);

    frames = (end - start) / (area * sizeof(int));

    if (verbosity)
    {
        printf("Number of frames in the file: %d\n", frames);
    }

    int DY[] = { -1, -1, -1, +0, +0, +0, +1, +1, +1 };
    int DX[] = { -1, +0, +1, -1, +0, +1, -1, +0, +1 };

    pixels = (int*) malloc(area * sizeof(int));
    filtered = (int*) malloc(area * sizeof(int));
    
    // number of neighbors per pixel
    int neighbor_count;

    for (i = 0; i < frames; i++)
    {
        // In the sequential version, this is not necessary
        //fseek(in, i * area * sizeof(int) + start, SEEK_SET);

        if (!feof(in))
        {
            if (fread(pixels, area * sizeof(int), 1, in))
            {
                for (line = 0; line < height; line++)
                {
                    for (row = 0; row < width; row++)
                    {
                        long long int sum = 0;
                        neighbor_count = 0;
                        
                        for (d = 0; d < 9; d++)
                        {
                            dx = row + DX[d];
                            dy = line + DY[d];

                            if (dx >= 0 && dx < width && dy >= 0 && dy < height)
                            {
                                // Wrong in original file
                                sum += pixels[dy * width + dx];
                                neighbor_count++;
                            }
                        }

                        filtered[line * width + row] = sum / neighbor_count;
                    }
                }

                // Writing the ith frame in the output file
                // fseek(out, i * area * sizeof(int) + start, SEEK_SET); // not necessary
                fwrite(filtered, area * sizeof(int), 1, out);
            }
        }
    }

    free(pixels);
    free(filtered);

    fclose(in);
    fclose(out);

    return 0;
}
