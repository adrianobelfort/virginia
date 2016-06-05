#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <argp.h>

void getTime(struct timespec* timestamp)
{
	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, timestamp);
	clock_gettime(CLOCK_REALTIME, timestamp);
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

FILE* openFileWithExtension(char* filename, char* extension, char* mode)
{
    FILE* filePointer;
    char fullname[50];

    strcpy(fullname, filename);
    strcat(fullname, extension);

    filePointer = fopen(fullname, mode);

    return filePointer;
}

FILE* openFile(char* filename, char* mode)
{
	return openFileWithExtension(filename, "", mode);
}

void closeFile(FILE* fileDescriptor)
{
	fclose(fileDescriptor);
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

/***************************** ARGP HANDLING *******************************/

const char* argp_program_version = "virginia 1.0";
const char* argp_program_bug_address = "<adriano.belfort@outlook.com>";

/* Program documentation */

static char documentation[] = "virginia - a simple, configurable test framework\
\vvirginia was created by Adriano Belfort de Sousa in 2016\n";

/* A description of the arguments we accept */
static char args_doc[] = "programs-and-arguments";

/* The options we understand */
static struct argp_option options[] = {
/* long name | character | value | option flag | description */

    {"track-time", 'r', 0, 0, "Keeps record of the execution time"},
    {"test", 't', "TEST-CASE", 0, "Run the program with input coming from a test case file"},
	{"verbose", 'v', 0, 0, "Print detailed information for debugging purposes"},

    {0}
};

/* Struct used to exchange information between parse_opt and main */
struct arguments
{
    char *testCaseName,         // --test -t
         **userProgram,         // programs-and-arguments
         trackTime,             // --track-time -r
		 verbose;
};

/* Parse a single option */
static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
    /*  Get the input argument from argp_parse, which we
        know is a pointer to our arguments structure */
    struct arguments *arguments = state->input;

    /*  We set the right flags and variables according to
        what we get as key  */
    switch (key)
    {
        case 'r':
            arguments->trackTime = 1;
            break;
        case 't':
            arguments->testCaseName = arg;
            break;
		case 'v':
			arguments->verbose = 1;
			break;

		case ARGP_KEY_NO_ARGS:
			argp_usage(state);

		case ARGP_KEY_ARG:
		/* Here we know that state->arg_num == 0, since we
		   force argument parsing to end before any more arguments can
		   get here.

		   Now we consume all the rest of the arguments.
		   state->next is the index in state->argv of the
		   next argument to be parsed, which is the second string
		   we’re interested in, so we can just use
		   &state->argv[state->next - 1] as the value for
		   arguments->strings to get strings from the first.

		   In addition, by setting state->next to the end
		   of the arguments, we can force argp to stop parsing here and
		   return. */
		   arguments->userProgram = &state->argv[state->next - 1];
		   state->next = state->argc;

		   break;

		default:
			return ARGP_ERR_UNKNOWN;
    }
	return 0;
}

void initializeArguments(struct arguments* arguments)
{
	/* Default values for the arguments */
	arguments->trackTime = 0;
	arguments->testCaseName = NULL;
	arguments->verbose = 0;
}

/* Definition of our argp parser */
static struct argp argp = {options, parse_opt, args_doc, documentation};

/*************************** END OF ARGP HANDLING **************************/

void reduceStrings(char* string, char** strings)
{
	int i;

	strcpy(string, strings[0]);

	for (i = 1; strings[i] != NULL; i++)
	{
		strcat(string, " ");
		strcat(string, strings[i]);
	}
}

int main (int argc, char* argv[])
{
	int i;
	struct arguments arguments;

	unsigned long long int timeElapsed;
	struct timespec start, end;

	char command[512];

	initializeArguments(&arguments);

	/* 	Parse our arguments; every option seen by parse_opt
		will be reflected in arguments	*/
	argp_parse(&argp, argc, argv, ARGP_IN_ORDER, 0, &arguments);

	if (arguments.verbose)
	{
		printf("To run: ");
		for (i = 0; arguments.userProgram[i] != NULL; i++)
		{
			printf("%s ", arguments.userProgram[i]);
		}
		printf("\n");
		printf("Track time? %s\n", arguments.trackTime ? "yes" : "no");
		printf("Verbose? %s\n", arguments.verbose ? "yes" : "no");
		printf("With test case: %s\n", arguments.testCaseName);
	}

	reduceStrings(command, arguments.userProgram);

	if (arguments.verbose) printf("Complete input string: %s\n", command);

	getTime(&start);

    /****************************** SOLUTION ******************************************/
    system(command);
    /*************************** END OF SOLUTION ***************************/

	getTime(&end);

    timeElapsed = timeDifference(end, start);

	if (arguments.trackTime)
	{
    	printf("%s %llu\n", argv[1], timeElapsed);
	}

    return 0;
}
