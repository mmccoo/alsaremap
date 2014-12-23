#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <ctype.h>
#include <sys/poll.h>
#include <alsa/asoundlib.h>
#include <getopt.h>
#include "alsaprint.h"

/* This value matches anything. */

#define DONTCARE (-1)

/* Used for values the current event doesn't support. */

#define INAPPROPRIATE (-2)

/* These are the various special matches available. */

#define SPECIAL_NONE 0
#define SPECIAL_DISCARD 1
#define SPECIAL_TRACE 2

/* These are the magic controllers. */

#define CONTROLLER_PITCHBEND 256

/* The match table is a linked list of Match structures. */

typedef struct Match_s {
	int special;
	int stop;
	int inputchannel;
	int inputinstrument;
	int inputnote;
	int inputcontroller;
	int outputchannel;
	int outputinstrument;
	int outputcontroller;
	int outputnote;
	struct Match_s* next;
} Match;

Match* matchtable = NULL;

/* Keeps track of which instrument is on each of the output channels. */

int outputinstrument[16]; // Zero-initialised (0 == piano)

/* Define the input and output ports. */

int inport = 0;
int outport = 0;

/* Are we doing pseudocontroller mapping? */

int pseudo_pitchbend = 0;
int pseudo_aftertouch = 0;

/* The handle to the ALSA sequencer. */

snd_seq_t* sequencer = NULL;

/* ======================================================================= */
/*                                UTILITIES                                */
/* ======================================================================= */

void error(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	fprintf(stderr, "Error: ");
	vfprintf(stderr, format, ap);
	fprintf(stderr, "\n");
	va_end(ap);

	exit(1);
}

void printconditional(const char* prefix, int value)
{
	if (value >= 0)
		printf("%s%d ", prefix, value);
}

/* ======================================================================= */
/*                           CONFIGURATION FILE                            */
/* ======================================================================= */

char* readline(FILE* file)
{
	static char buffer[256];
	int i = 0;
	char c;
	
	for (;;)
	{
		c = fgetc(file);

		switch (c)
		{
			case EOF:
				if (i == 0)
					return NULL;
				goto newline;

			case ' ':
			case '\t':
			case '\r':
				continue;

			case ';':
				do {
					c = fgetc(file);
				} while ((c != EOF) && (c != '\n'));
				if (c == EOF)
					c = '\n';
				goto newline;

			case '\n':
			newline:
				if (i == 0)
					continue;
				buffer[i++] = '\0';
				return buffer;

			default:
				if (i == 255)
					error("Input line too long");
				buffer[i++] = c;
				break;
		}
	}
}

void add_match(Match* match)
{
	static Match* lastmatch = NULL;

	if (!lastmatch)
		matchtable = match;
	else
		lastmatch->next = match;

	lastmatch = match;
}

char* read_spec(char* buffer, int* channel, int* instrument, int* note,
		int* controller, int* special)
{
	char c;

	*channel = DONTCARE;
	*instrument = DONTCARE;
	*note = DONTCARE;
	*controller = DONTCARE;

	for (;;)
	{
		c = *buffer++;
		switch (c)
		{
			case '=':
			case '>':
			case '\0':
				/* Terminator */
				buffer--;
				goto exit;

			case ' ':
			case '\t':
			case '\n':
			case '\r':
				/* Ignored whitespace */
				break;

			case '0':	case '1':	case '2':
			case '3':	case '4':	case '5':
			case '6':	case '7':	case '8':
			case '9':
			/* Channel */
				buffer--;
				*channel = strtol(buffer, &buffer, 10);
				break;

			case '#':
				/* Instrument */
				*instrument = strtol(buffer, &buffer, 10);
				break;

			case '&':
				/* Note */
				*note = strtol(buffer, &buffer, 10);
				break;

			case '*':
				/* Controller */
				*controller = strtol(buffer, &buffer, 10);
				switch (*controller)
				{
					case CONTROLLER_PITCHBEND:
						pseudo_pitchbend = 1;
						break;
				}
				break;

			case 'X':
				/* Discard */
				*special = SPECIAL_DISCARD;
				goto exit;

			case 'T':
				/* Trace */
				*special = SPECIAL_TRACE;
				goto exit;

			default:
				error("Invalid character '%c' in input file", c);
		}
	}

exit:
	return buffer;
}

void add_rule(char* s)
{
	int i = SPECIAL_NONE;
	Match* match = (Match*) calloc(1, sizeof(Match));

	match->special = SPECIAL_NONE;
	s = read_spec(s, &match->inputchannel,
			&match->inputinstrument, &match->inputnote,
			&match->inputcontroller, &i);
	
	switch (*s++)
	{
		case '>': /* Immediate stop */
			match->stop = 1;
			break;

		case '=': /* Continue matching */
			match->stop = 0;
			break;

		default:
			error("Expected '=' or '>' as a terminator in input file");
	}

	s = read_spec(s, &match->outputchannel, &match->outputinstrument,
			&match->outputnote, &match->outputcontroller, &match->special);
	if (*s != '\0')
		error("Expected end of line in input file, got '%c'", *s);

	add_match(match);
}

void read_map_file(char* filename)
{
	FILE* file = fopen(filename, "r");
	if (!file)
		error("Couldn't open map file");

	for (;;)
	{
		char* s = readline(file);
		if (!s)
			return;
		add_rule(s);
	}

	fclose(file);
}


/* ======================================================================= */
/*                            MATCHING ENGINE                              */
/* ======================================================================= */

int match_event(int* channel, int* instrument, int* note, int* controller)
{
	Match* m = matchtable;

	while (m)
	{
		if (((m->inputchannel == DONTCARE) ||
		     (m->inputchannel == *channel)) &&
		    ((m->inputinstrument == DONTCARE) ||
		     (m->inputinstrument == *instrument)) &&
		    ((m->inputnote == DONTCARE) ||
		     (m->inputnote == *note)) &&
		    ((m->inputcontroller == DONTCARE) ||
		     (m->inputcontroller == *controller)))
		{
			/* A match! */

			switch (m->special)
			{
				case SPECIAL_DISCARD:
					return 0;

				case SPECIAL_TRACE:
					printconditional("", *channel);
					printconditional("#", *instrument);
					printconditional("&", *note);
					printconditional("*", *controller);
					putchar('\n');
					break;
			}

			if ((m->outputchannel != DONTCARE) &&
			    (*channel != INAPPROPRIATE))
				*channel = m->outputchannel;

			if ((m->outputinstrument != DONTCARE) &&
			    (*instrument != INAPPROPRIATE))
				*instrument = m->outputinstrument;

			if ((m->outputnote != DONTCARE) &&
			    (*note != INAPPROPRIATE))
				*note = m->outputnote;

			if ((m->outputcontroller != DONTCARE) &&
			    (*controller != INAPPROPRIATE))
				*controller = m->outputcontroller;

			if (m->stop)
				return 1;
		}

		m = m->next;
	}

	return 1;
}
		   
/* ======================================================================= */
/*                                 ALSA                                    */
/* ======================================================================= */

void open_alsa_connection(void)
{
	int client;
	
	if (snd_seq_open(&sequencer, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0)
		error("Couldn't open ALSA sequencer");
		
	snd_seq_set_client_name(sequencer, "amidimap");
	client = snd_seq_client_id(sequencer);

	inport = snd_seq_create_simple_port(sequencer, "amidimap IN",
		SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
		SND_SEQ_PORT_TYPE_APPLICATION);
	if (inport < 0)
		error("Couldn't create input port");

	outport = snd_seq_create_simple_port(sequencer, "amidimap OUT",
		SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
		SND_SEQ_PORT_TYPE_APPLICATION);
	if (outport < 0)
		error("Couldn't create output port");

	printf("IN = %d:%d, OUT = %d:%d\n",
		client, inport,
		client, outport);
}

void change_instrument(int channel, int instrument)
{
	snd_seq_event_t event;
	
	snd_seq_ev_clear(&event);
	snd_seq_ev_set_pgmchange(&event, channel, instrument);
	snd_seq_ev_set_subs(&event);
	snd_seq_ev_set_direct(&event);
	snd_seq_ev_set_source(&event, outport);
	snd_seq_event_output_direct(sequencer, &event);

	outputinstrument[channel] = instrument;
}

void process_events(void)
{
	int numpolldesc = snd_seq_poll_descriptors_count(sequencer, POLLIN);
	struct pollfd* pfd;
	snd_seq_event_t* event;
	int channel;
	int instrument;
	int note;
	int controller;
	int i;

	pfd = (struct pollfd*) malloc(sizeof(struct pollfd) * numpolldesc);
	if (!pfd)
		error("Memory allocation failed");
	
	snd_seq_poll_descriptors(sequencer, pfd, numpolldesc, POLLIN);
	
	for (;;)
	{
		if (snd_seq_event_input(sequencer, &event) < 0)
		{
			poll(pfd, 1, 100000);
			continue;
		}

		snd_seq_ev_set_subs(event);  
		snd_seq_ev_set_direct(event);

		std::cout << *event << std::endl;
		switch (event->type)
		{
			/* snd_seq_ev_note */

			case SND_SEQ_EVENT_NOTE:
			case SND_SEQ_EVENT_NOTEON:
			case SND_SEQ_EVENT_NOTEOFF:
			case SND_SEQ_EVENT_KEYPRESS:
				channel = event->data.note.channel;
				instrument = DONTCARE;
				note = event->data.note.note;
				controller = INAPPROPRIATE;
				break;

			/* snd_seq_ev_ctrl */

			case SND_SEQ_EVENT_PGMCHANGE:
				channel = event->data.control.channel;
				instrument = event->data.control.value;
				note = INAPPROPRIATE;
				controller = INAPPROPRIATE;
				break;

			case SND_SEQ_EVENT_CONTROLLER:
			case SND_SEQ_EVENT_CHANPRESS:
			case SND_SEQ_EVENT_CONTROL14:
			case SND_SEQ_EVENT_NONREGPARAM:
			case SND_SEQ_EVENT_REGPARAM:
				channel = event->data.control.channel;
				instrument = INAPPROPRIATE;
				note = INAPPROPRIATE;
				controller = event->data.control.param;
				break;

			case SND_SEQ_EVENT_PITCHBEND:
				channel = event->data.control.channel;
				instrument = INAPPROPRIATE;
				note = INAPPROPRIATE;
				controller = CONTROLLER_PITCHBEND;
				if (pseudo_pitchbend)
				{
					event->type = SND_SEQ_EVENT_CONTROLLER;
					/* Scale -8192..8191 to 0..127. */
					event->data.control.value /= 128;
					event->data.control.value += 64;
				}
				break;
				
			/* everything else */

			default:
				channel = INAPPROPRIATE;
				instrument = INAPPROPRIATE;
				note = INAPPROPRIATE;
				controller = INAPPROPRIATE;
				break;
		}

		if (match_event(&channel, &instrument, &note, &controller))
		{
			switch (event->type)
			{
				/* snd_seq_ev_note */

				case SND_SEQ_EVENT_NOTE:
				case SND_SEQ_EVENT_NOTEON:
				case SND_SEQ_EVENT_NOTEOFF:
				case SND_SEQ_EVENT_KEYPRESS:
					if (outputinstrument[channel] != instrument)
						change_instrument(channel, instrument);

					event->data.note.channel = channel;
					event->data.note.note = note;
					break;

				/* snd_seq_ev_ctrl */

				case SND_SEQ_EVENT_PGMCHANGE:
					event->data.control.channel = channel;
					if (event->data.control.param == 0)
					{
						outputinstrument[channel] = instrument;
						event->data.control.value = instrument;
					}
					break;

				case SND_SEQ_EVENT_CONTROLLER:
				case SND_SEQ_EVENT_CHANPRESS:
				case SND_SEQ_EVENT_CONTROL14:
				case SND_SEQ_EVENT_NONREGPARAM:
				case SND_SEQ_EVENT_REGPARAM:
				case SND_SEQ_EVENT_PITCHBEND:
					event->data.control.channel = channel;
					switch (controller)
					{
						case CONTROLLER_PITCHBEND:
							event->type = SND_SEQ_EVENT_PITCHBEND;
							event->data.control.value -= 64;
							event->data.control.value *= 128;
							break;
							
						default:
							event->data.control.param = controller;
					}
					break;

				/* everything else */

				default:
					break;
			}

			snd_seq_ev_set_source(event, outport);
			i = snd_seq_event_output_direct(sequencer, event);
			if (i < 0)
				error("snd_seq_event_output_direct() failed: %s", snd_strerror(i));
		}

		snd_seq_free_event(event);
	}
}

/* ======================================================================= */
/*                              MAIN PROGRAM                               */
/* ======================================================================= */

void help(void)
{
	printf(
		"amidimap v0.1.1 (C) 2002-2005 David Given\n"
		"Usage: amidimap <options>\n"
		"where <options> is zero or more of:\n"
		"  -h, --help\n"
		"    Displays this message\n"
		"  -r <rule>, --rule <rule>\n"
		"    Adds a single rule\n"
		"  -f <file>, --file <file>\n"
		"    Adds a file full of rules\n"
		"Options are processed left to right; the order is important. If you don't\n"
		"specify any rules at all, that is a perfectly legal map that throws all\n"
		"events away. The minimum useful map is the single rule '>', which directs\n"
		"all input events to the output.\n");
	exit(0);
}

int main(int argc, char* argv[])
{
	for (;;)
	{
		int index = 0;
		static struct option long_options[] = {
			{"help", 0, NULL, 'h'},
			{"rule", 1, NULL, 'r'},
			{"file", 1, NULL, 'f'},
			{NULL, 0, NULL, 0}};

		int c = getopt_long(argc, argv, "hr:f:", long_options, &index);
		if (c == -1)
			break;

		switch (c)
		{
			case 'h': /* Help */
				help();

			case 'r': /* Add single rule */
				if (!optarg)
					error("you must specify a rule to add");
				add_rule(optarg);
				break;

			case 'f': /* Add a map file */
				if (!optarg)
					error("you must specify a map file to read");
				read_map_file(optarg);
				break;

			case '?': /* Unrecognised parameter */
				error("Unrecognised parameter");
		}
	}
	
	if (optind < argc)
		error("too many parameters");

	open_alsa_connection();
	process_events();
	
	return 0;
}

