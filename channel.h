/* CHANNEL.H - Declarations regarding channels. */

/* Copyright (c) 2001 by Radford M. Neal 
 *
 * Permission is granted for anyone to copy, use, modify, or distribute this
 * program and accompanying programs and documents for any purpose, provided 
 * this copyright notice is retained and prominently displayed, along with
 * a note saying that the original programs are available from Radford Neal's
 * web page, and note is made of any changes made to the programs.  The
 * programs and documents are distributed without any warranty, express or
 * implied.  As the programs were written for research purposes only, they have
 * not been tested to the degree that would be advisable in any important
 * application.  All use of these programs is entirely at the user's own risk.
 */


/* TYPES OF CHANNEL, AND CHANNEL PARAMETERS.  The global variables declared
   here are located in channel.c. */

typedef enum { BSC, AWGN, AWLN } channel_type;

extern channel_type channel;	/* Type of channel */

extern double error_prob;	/* Error probability for BSC */
extern double std_dev;		/* Noise standard deviation for AWGN */
extern double lwidth;		/* Width of noise distributoin for AWLN */


/* PROCEDURES TO DO WITH CHANNELS. */

int  channel_parse (char **, int);
void channel_usage (void);
