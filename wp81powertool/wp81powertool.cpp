// wp81powertool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "getopt.h"
#include "bluetooth.h"
#include "vibration.h"
#include "battery.h"
#include "oempanel.h"

#define ACTION_BT_RADIO_ON 1
#define ACTION_BT_RADIO_OFF 2
#define ACTION_QUERY_BT_RADIO 3
#define ACTION_VIBRATE 4
#define ACTION_QUERY_BATTERY 5
#define ACTION_OEM_PANEL 6
#define ACTION_QUERY_OEM_PANEL 7

BOOL verbose;

static void usage(char *programName)
{
	printf("%s - Power tools\n"
		"Usage:\n", programName);
	printf("\t%s [options]\n", programName);
	printf("options:\n"
		"\t-h, --help                Show help options\n"
		"\t-v, --verbose             Increase verbosity\n"
		"\t-b, --btradio {on|off}    Activate/Deactivate Bluetooth radio\n"
		"\t-n, --vibrate {duration}	 Activate vibration during N milliseconds\n"
		"\t-o, --oempanel            Nokia panel\n"	
		"\t-q, --query {help|...}    Display \'query\' option usage\n");
}

static void queryUsage(char *programName)
{
	printf("%s - Power tools\n"
		"Query usage:\n", programName);
	printf("\t%s --query [options]\n", programName);
	printf("options:\n"
		"\thelp     Display this help\n"
		"\tbtradio  Display state of the Bluetooth radio\n"
		"\tbattery  Display state of the battery\n"
		"\toempanel Display Nokia panel information\n");
}

static const struct option main_options[] = {
	{ "help",      no_argument,       NULL, 'h' },
	{ "verbose",   no_argument,       NULL, 'v' },
	{ "btradio",   required_argument, NULL, 'b' },
	{ "vibrate",   required_argument, NULL, 'n' },
	{ "oempanel",  required_argument, NULL, 'o' },
	{ "query",     required_argument, NULL, 'q' },
	{}
};

int main(int argc, char* argv[])
{
	int exit_status = EXIT_SUCCESS;
	DWORD action;
	DWORD vibrationDuration;

	for (;;) {
		int opt;

		opt = getopt_long(argc, argv,
			"hvb:q:n:o",
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
			if (_stricmp("on", optarg) != 0 && _stricmp("off", optarg) != 0)
			{
				printf("Unknown argument [%s]\n", optarg);
				usage(argv[0]);
				return EXIT_FAILURE;
			}
			if (_stricmp("on", optarg) == 0)
			{
				action = ACTION_BT_RADIO_ON;
			}
			else
			{
				action = ACTION_BT_RADIO_OFF;
			}
			break;
		case 'q':
			if (_stricmp("help", optarg) == 0)
			{
				queryUsage(argv[0]);
				return EXIT_SUCCESS;
			}
			else if (_stricmp("btradio", optarg) == 0)
			{
				action = ACTION_QUERY_BT_RADIO;
			}
			else if (_stricmp("battery", optarg) == 0)
			{
				action = ACTION_QUERY_BATTERY;
			}
			else if (_stricmp("oempanel", optarg) == 0)
			{
				action = ACTION_QUERY_OEM_PANEL;
			}
			else
			{
				printf("Unknown argument [%s]\n", optarg);
				queryUsage(argv[0]);
				return EXIT_FAILURE;
			}
			break;
		case 'n':
			action = ACTION_VIBRATE;
			vibrationDuration = atoi(optarg);
			if (vibrationDuration <= 0)
			{
				printf("Vibration duration must be a positive integer.\n", optarg);
				queryUsage(argv[0]);
				return EXIT_FAILURE;
			}
			break;
		case 'o':
			action = ACTION_OEM_PANEL;
			break;
		default:
			usage(argv[0]);
			return EXIT_FAILURE;
		}
	}

	switch (action)
	{
	case ACTION_BT_RADIO_ON:
		return ChangeRadioState(TRUE);
	case ACTION_BT_RADIO_OFF:
		return ChangeRadioState(FALSE);
	case ACTION_QUERY_BT_RADIO:
		return QueryRadioState();
	case ACTION_VIBRATE:
		return vibrate(vibrationDuration);
	case ACTION_QUERY_BATTERY:
		return QueryBattery();
	case ACTION_QUERY_OEM_PANEL:
		return InfoOemPanel();
	}

	return exit_status;
}

