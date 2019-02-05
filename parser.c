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

#include "turtleP.h"

#define ERROR(STR) fprintf(stderr, STR); exit(EXIT_FAILURE)

void testing(void);

int main(int argc, char** argv)
{
	FILE *fp;
	prog *start, *current;
	char instrc[50];

	testing();

	if(argc != 2) {
		ERROR("Usage : make <file to run>\n");
	}

	if(!(fp = fopen(argv[1], "r"))) {
		ERROR("Cannot Open the File!\n");
	}

	if(fscanf(fp, "%s", instrc)) {
		start = current = prog_insert(instrc);
	}

	while(fscanf(fp, "%s", instrc) != EOF) {
		current->next = prog_insert(instrc);
		current = current->next;
	}

	fclose(fp);

	if(check_main(start)) {
		print_list(start);
		printf("Parsered Success!\n");
	} else {
		prog_free(&start);
		ERROR("Parsered Failed!\n");
	}


	prog_free(&start);

	return 0;
}
