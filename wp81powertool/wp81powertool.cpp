// wp81powertool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "getopt.h"

static void usage(char *programName)
{
	printf("%s - Power tools\n"
		"Usage:\n", programName);
	printf("\t%s [options]\n", programName);
	printf("options:\n"
		"\t-h, --help               Show help options\n"
		"\t-v, --verbose            Increase verbosity\n"
		"\t-b, --bluetooth {on|off} Activate/Deactivate bluetooth\n");
}

static const struct option main_options[] = {
	{ "help",      no_argument,       NULL, 'h' },
	{ "verbose",   no_argument,       NULL, 'v' },
	{ "bluetooth", required_argument, NULL, 'b' },
	{}
};

int main(int argc, char* argv[])
{
	int exit_status = EXIT_SUCCESS;
	BOOL verbose = FALSE;

	for (;;) {
		int opt;

		opt = getopt_long(argc, argv,
			"hvb:",
			main_options, NULL);

		if (opt < 0) {
			// no more option to parse
			break;
		}

		switch (opt) {
		case 'h':
			usage(argv[0]);
			return EXIT_SUCCESS;
		case 'v':
			printf("Verbose mode\n");
			verbose = TRUE;
			break;
		case 'b':
			if (strcmp("on", optarg) != 0 && strcmp("off", optarg) != 0)
			{
				printf("Unknown argument [%s]\n", optarg);
				usage(argv[0]);
				return EXIT_FAILURE;
			}
			break;
		default:
			usage(argv[0]);
			return EXIT_FAILURE;
		}
	}

	//exit_status = mainLoop_run(verbose);

	return exit_status;
}

