#include "turtleP.h"

#include <assert.h>

#define WORDLENGTH 50
#define ERROR(STR) fprintf(stderr, STR); exit(EXIT_FAILURE)

/* for <MAIN> ::= "{" <INSTRCTLST> */
bool check_main(prog *p)
{
	if(strcmp(p->str, "{") != 0) {
		fprintf(stderr, "!!ERROR!! Expected start with '{'\n");

		return false;
	}

	/* check DO and program end with '}' */
	if(!check_curlybracket(p)) {
		fprintf(stderr, "!!ERROR!! Expected DO and program end with '}'\n");
		return false;
	}


	if(!check_INSTRCTLST(p->next)) {
		return false;
	}

	return true;
}

/* for <INSTRCTLST> ::= <INSTRUCTION><INSTRCTLST> | "}" */
bool check_INSTRCTLST(prog *p)
{
	prog *nextOne;

	/* check '}' */
	if(!strcmp(p->str, "}")) {
		if(p->next != NULL) {
			return check_INSTRCTLST(p->next);
		}
		else {
			return true;
		}
	}

	if(!check_INSTRCTION(p)) {
		return false;
	}

	/* if the prev is <FD> | <LT> | <RT> */
	if(!strcmp(p->str, "FD") || !strcmp(p->str, "LT") || !strcmp(p->str, "RT")) {
		nextOne = p->next;
		return check_INSTRCTLST(nextOne->next);
	}

	/* if the prev is <SET> */
	if(!strcmp(p->str, "SET")) {
		nextOne = p;

		do {
			nextOne = nextOne->next;
		} while(strcmp(nextOne->str, ";") != 0);

		if(!strcmp(nextOne->str, ";")) {
			return check_INSTRCTLST(nextOne->next);
		}
	}

	/* if the prev is <DO> */
	if(!strcmp(p->str, "DO")) {
		nextOne = p;
		do {
			nextOne = nextOne->next;
		} while(strcmp(nextOne->str, "}") != 0);

		return check_INSTRCTLST(nextOne->next);
	}

	return false;
}

/* for <INSTRUCTION> ::= <FD> | <LT> | <RT> | <DO> | <SET> */
bool check_INSTRCTION(prog *p)
{
	if(!strcmp(p->str, "FD")) {
		if(check_FDLTRT(p)) {
			return true;
		}
		fprintf(stderr, "!!ERROR!! Expected <FD> followed by <VARNUM>\n");
		return false;
	}

	if(!strcmp(p->str, "LT")) {
		if(check_FDLTRT(p)) {
			return true;
		}
		fprintf(stderr, "!!ERROR!! Expected <LT> followed by <VARNUM>\n");
		return false;
	}

	if(!strcmp(p->str, "RT")) {
		if(check_FDLTRT(p)) {
			return true;
		}
		fprintf(stderr, "!!ERROR!! Expected <RT> followed by <VARNUM>\n");
		return false;
	}

	if(!strcmp(p->str, "DO")) {
		if(check_DO(p)) {
			return true;
		}
		fprintf(stderr, "!!ERROR!! Expected <DO> followed by <VAR> FROM <VARNUM> TO <VARNUM> { <INSTRCTLST>\n");
		return false;
	}

	if(!strcmp(p->str, "SET")) {
		if(check_SET(p)) {
			return true;
		}
		fprintf(stderr, "!!ERROR!! Expected <SET> followed by <VAR> := <POLISH>\n");
		return false;
	}

	fprintf(stderr, "!!ERROR!! Expected INSTRUCTION to be <FD> | <LT> | <RT> | <DO> | <SET>\n");
	return false;
}

/* for <FD> ::= "FD" <VARNUM> */
/* for <LT> ::= "LT" <VARNUM> */
/* for <RT> ::= "RT" <VARNUM> */
bool check_FDLTRT(prog *p)
{
	prog *nextOne;

	nextOne = p->next;

	if(check_VARNUM(nextOne->str)) {
		return true;
	}

	return false;
}

/* for <DO> ::= "DO" <VAR> "FROM" <VARNUM> "TO" <VARNUM> "{" <INSTRCTLST> */
bool check_DO(prog *p)
{
	prog *current;
	current = p->next;

	if(!check_VAR(current->str)) {
		fprintf(stderr, "!!ERROR!! Expected <VAR> ::= [A−Z]\n");
		return false;
	}
	else {
		current = current->next;

		if(strcmp(current->str, "FROM") != 0) {
			return false;
		}
		else {
			current = current->next;

			if(!check_VARNUM(current->str)) {
				return false;
			}
			else {
				current = current->next;

				if(strcmp(current->str, "TO") != 0) {
					return false;
				}
				else {
					current = current->next;

					if(!check_VARNUM(current->str)) {
						return false;
					}
					else {
						current = current->next;

						if(strcmp(current->str, "{") != 0) {
							return false;
						}
						else {
							current = current->next;
							while(strcmp(current->str, "}") != 0) {
								if(!check_INSTRCTION(current)) {
									return false;
								}
								/*current = current->next;*/
								if(!strcmp(current->str, "FD") || !strcmp(current->str, "LT") || !strcmp(current->str, "RT")) {

									current = current->next->next;
								}

								/* if the prev is <SET> */
								if(!strcmp(current->str, "SET")) {
									do {
										current = current->next;
									} while(strcmp(current->str, ";") != 0);
									current = current->next;
								}

								/* if the prev is <DO> */
								if(!strcmp(current->str, "DO")) {
									if(check_DO(current)){
										do {
											current = current->next;
										} while(strcmp(current->str, "}") != 0);
										current = current->next;
									}
								}
							}

							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

/* for <SET> ::= "SET" <VAR> ":=" <POLISH> */
bool check_SET(prog *p)
{
	prog *current;

	current = p->next;

	if(!check_VAR(current->str)) {
		fprintf(stderr, "!!ERROR!! Expected <VAR> ::= [A−Z]\n");
		return false;
	}
	else {
		current = current->next;

		if(strcmp(current->str, ":=") != 0) {
			return false;
		}
		else {
			current = current->next;
			if(check_POLISH(current)) {
				return true;
			}
		}
	}

	return false;
}

/* for <POLISH> ::= <OP> <POLISH> | <VARNUM> <POLISH> | ";" */
bool check_POLISH(prog *p)
{
	prog *current;

	current = p;

	if(!strcmp(current->str, ";")) {
		return true;
	}

	if(check_OP(current->str)) {
		current = current->next;
		return check_POLISH(current);
	}

	if(check_VARNUM(current->str)) {
		current = current->next;
		return check_POLISH(current);
	}

	fprintf(stderr, "!!ERROR!! Expected <SET> end with ';'\n");
	return false;
}


/* for <VARNUM> ::= number | <VAR>*/
bool check_VARNUM(char *operand)
{
	double op;

	if(check_VAR(operand) || sscanf(operand, "%lf", &op)) {
		return true;
	}

	fprintf(stderr, "!!ERROR!! Expected <VARNUM> ::= [A−Z] or numbers\n");
	return false;
}

/* for <VAR> ::= [A−Z]*/
bool check_VAR(char *var)
{
	char v;
	sscanf(var, "%c", &v);

	if(strlen(var) == 1 && v >= 'A' && v <= 'Z') {
		return true;
	}

	return false;
}

/* for <OP> ::= "+" | "−" | "*" | "/" */
bool check_OP(char *op)
{
	if(!strcmp(op, "+") || !strcmp(op, "-") || !strcmp(op, "*") || !strcmp(op, "/")) {
		return true;
	}

	return false;
}

/* check DO and program end with '}' */
bool check_curlybracket(prog *p)
{
	prog *nextOne;
	int countDO = 0;
	int countBracket = 0;

	nextOne = p;
	while(nextOne != NULL) {
		if(!strcmp(nextOne->str, "}")) {
			countBracket++;
		}
		if(!strcmp(nextOne->str, "DO")) {
			countDO++;
		}
		nextOne = nextOne->next;
	}

	if(countDO + 1 != countBracket) {
		return false;
	}

	return true;
}

/* for linked list*/
prog* prog_init(int strSize)
{
	prog *program;

	program = (prog*) calloc(1, sizeof(prog));
	if(program == NULL){
		ERROR("Cannot create a Code Space\n");
	}

	program->str = (char*) calloc(strSize+1, sizeof(char));
	if(program->str == NULL){
		ERROR("Cannot create a String Space\n");
	}

	return program;
}

prog* prog_insert(char *instrc)
{
	prog *new;

	new = prog_init(strlen(instrc));
	strcpy(new->str, instrc);

	return new;
}

void print_list(prog *head)
{
	printf("\n");
	do {
		printf("%s\n", head->str);
		head = head->next;
	} while(head != NULL);
}

void prog_free(prog** s)
{
	prog* new = *s;

	if(new != NULL) {
		free(new->str);
		prog_free(&(new->next));
		free(new);
		*s = NULL;
	}
}
