/*
<MAIN> ::= "{" <INSTRCTLST>
<INSTRCTLST> ::= <INSTRUCTION><INSTRCTLST> | "}"
<INSTRUCTION> ::= <FD> | <LT> | <RT> | <DO> | <SET>
<FD> ::= "FD" <VARNUM>
<LT> ::= "LT" <VARNUM>
<RT> ::= "RT" <VARNUM>
<DO> ::= "DO" <VAR> "FROM" <VARNUM> "TO" <VARNUM> "{" <INSTRCTLST>
<VAR> ::= [A−Z]
<VARNUM> ::= number | <VAR>
<SET> ::= "SET" <VAR> ":=" <POLISH>
<POLISH> ::= <OP> <POLISH> | <VARNUM> <POLISH> | ";"
<OP> ::= "+" | "−" | "*" | "/"
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#include "turtleI.h"

#define ERROR(STR) fprintf(stderr, STR); exit(EXIT_FAILURE)
#define MILLISECONDDELAY 20


int main(int argc, char** argv)
{
	FILE *fp;
	prog *start, *current;
	char instrc[50];
	SDL_Simplewin sw;
	location loc;
	vector vec;
	double var[26];
	int i;

	loc.x = WWIDTH/2;
	loc.y = WHEIGHT/2;
	vec.x = 1;
	vec.y = 0;

	for(i = 0; i < 26; i++) {
		var[i] = 0.0;
	}

	if(argc != 2) {
		ERROR("Usage : make <file to run>\n");
	}

	if(!(fp = fopen(argv[1], "r"))) {
		ERROR("Cannot Open the File!\n");
	}

	if(fscanf(fp, "%s", instrc)) {
		current = prog_insert(instrc, &loc, &vec, var, &sw);
		start = current;
	}

	while(fscanf(fp, "%s", instrc) != EOF) {
		current->next = prog_insert(instrc, &loc, &vec, var, &sw);
		current = current->next;
	}

	fclose(fp);

	Neill_SDL_Init(&sw);

	Neill_SDL_SetDrawColour(&sw, 255, 255, 255);

	check_main(start);

	do {
		Neill_SDL_UpdateScreen(&sw);

		/* Has anyone pressed ESC or killed the SDL window ?
		 Must be called frequently - it's the only way of escaping  */
		Neill_SDL_Events(&sw);
	} while(!sw.finished);

	/* Clear up graphics subsystems */
	SDL_Quit();
	atexit(SDL_Quit);

	prog_free(&start);

	return 0;
}
