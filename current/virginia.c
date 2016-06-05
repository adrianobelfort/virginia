#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <argp.h>

void getTime(struct timespec* timestamp)
{
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
    char fullname[256];

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
        return 1;
    }
    else
    {
        return 0;
    }
}

/***************************** ARGP HANDLING *******************************/

const char* argp_program_version = "virginia 1.1";
const char* argp_program_bug_address = "<adriano.belfort@outlook.com>";

/* Program documentation */

static char documentation[] = "virginia - a simple, configurable test framework\
\vvirginia was created by Adriano Belfort de Sousa in 2016\n";

/* A description of the arguments we accept */
static char args_doc[] = "programs-and-arguments";

/* The options we understand */
static struct argp_option options[] = {
/* long name | character | value | option flag | description */

    {"log-time", 'l', "LOG-FILE", 0, "Log the execution time in nanoseconds to the specified log file"},
	{"test-case", 'c', "TEST-FILE", 0, "Run the target program with the specified file as argument"},
    {"input", 'i', "INPUT-FILE", 0, "Run the target program with input coming from a test case file"},
	{"tag", 't', "TAG", 0, "Adds a tag to the execution"},
	{"output", 'o', "OUTPUT-FILE", 0, "Redirect the output to the specified file"},
	{"verbose", 'v', 0, 0, "Print detailed information for debugging purposes"},
	{"laconic", 'e', 0, 0, "Print only the most important details"},

    {0}
};

/* Struct used to exchange information between parse_opt and main */
struct arguments
{
    char *testCaseName,         // --testcase -c
         **userProgram,         // programs-and-arguments
         trackTime,             // --log-time -l
		 *timeLogFilename,		// log-time argument
		 *executionTag,			// --tag -t
		 redirectInput,			// --input -i
		 *inputFilename,		// --input -i
		 redirectOutput, 		// --output -o
		 *outputFilename,		// --output -o
		 laconic,
		 verbose;				// --verbose -v
		 // add case o
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
        case 'l':
            arguments->trackTime = 1;
			arguments->timeLogFilename = arg;
            break;
        case 'c':
            arguments->testCaseName = arg;
            break;
		case 'i':
			arguments->redirectInput = 1;
			arguments->inputFilename = arg;
			break;
		case 'v':
			arguments->verbose = 1;
			break;
		case 't':
			arguments->executionTag = arg;
			break;
		case 'e':
			arguments->laconic = 1;
			break;
		case 'o':
			arguments->redirectOutput = 1;
			arguments->outputFilename = arg;
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
	arguments->timeLogFilename = NULL;
	arguments->executionTag = NULL;
	arguments->redirectInput = 0;
	arguments->inputFilename = NULL;
	arguments->redirectOutput = 0;
	arguments->outputFilename = NULL;
	arguments->verbose = 0;
	arguments->laconic = 0;
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

void composeOutputRedirectString(char* string, char* filename)
{
	strcat(string, " ");
	strcat(string, ">> ");
	strcat(string, filename);
}

void composeInputRedirectString(char* string, char* filename)
{
	strcat(string, " ");
	strcat(string, "< ");
	strcat(string, filename);
}

int main (int argc, char* argv[])
{
	int i;
	struct arguments arguments;

	unsigned long long int timeElapsed;
	struct timespec start, end;

	FILE* timeLogFile;

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
		if (arguments.trackTime)
		{
			printf("Write to file %s\n", arguments.timeLogFilename);
		}
		printf("Verbose? %s\n", arguments.verbose ? "yes" : "no");
		printf("Has test case? %s\n", arguments.testCaseName != NULL ? "yes" : "no");
		if (arguments.testCaseName)
		{
			printf("Test case name: %s\n", arguments.testCaseName);
		}
		if (arguments.executionTag)
		{
			printf("Execution tag: %s\n", arguments.executionTag);
		}
		if (arguments.redirectInput)
		{
			printf("Redirect input from %s\n", arguments.inputFilename);
		}
		if (arguments.redirectOutput)
		{
			printf("Redirect output to %s\n", arguments.outputFilename);
		}
	}

	reduceStrings(command, arguments.userProgram);

/************************** PROGRAM START *******************************/

	if (!arguments.laconic)
	{
		printf("\n");
		printf("@ Virginia, a simple, configurable test framework\n");
		printf("@ 2016-2016 by Adriano Belfort\n");
		printf("@\n");
		if (arguments.executionTag)
		{
			printf("@ Execution tag: %s\n", arguments.executionTag);
		}
		printf("@ Command: %s\n", command);
	}
	else
	{
		printf("[virginia] Running ");
		if (arguments.executionTag)
		{
			printf("%s: ", arguments.executionTag);
		}
		printf("%s\n", command);
	}

	if (arguments.redirectInput)
	{
		if (!arguments.laconic) printf("@ Redirect input from %s\n", arguments.inputFilename);
		composeInputRedirectString(command, arguments.inputFilename);
	}

	if (arguments.redirectOutput)
	{
		if (!arguments.laconic) printf("@ Redirect output to %s\n", arguments.outputFilename);
		composeOutputRedirectString(command, arguments.outputFilename);
	}

	if (arguments.trackTime)
	{
		if (!arguments.laconic) printf("@ Save execution time to %s\n", arguments.timeLogFilename);
	}

	if (arguments.verbose) printf("Complete input string: %s\n", command);


	printf("\n");

	getTime(&start);
    /****************************** SOLUTION *******************************/
	system(command);
    /*************************** END OF SOLUTION ***************************/
	getTime(&end);

	printf("\n");
	if (!arguments.laconic)
	{
		printf("@ Finished program execution!");
	}
	else
	{
		printf("[virginia] Finished program execution\n");
	}

	if (arguments.trackTime)
	{
		timeElapsed = timeDifference(end, start);

		timeLogFile = openFile(arguments.timeLogFilename, "a+");

		if (!fileOpenError(timeLogFile))
		{
			if (arguments.executionTag)
			{
				fprintf(timeLogFile, "%s: ", arguments.executionTag);
			}
			fprintf(timeLogFile, "%llu\n", timeElapsed);

			closeFile(timeLogFile);
		}
		else
		{
			fprintf(stderr, "Error: could not open log file %s\n", arguments.timeLogFilename);
		}
	}


    return 0;
}
