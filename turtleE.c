#include "turtleE.h"


#include <math.h>
#include <assert.h>

#define PI 3.14159265
#define VAL PI/180.0
#define CHARACTERCODE 65
#define STACKSIZE 50
#define ERROR(STR) fprintf(stderr, STR); exit(EXIT_FAILURE)


/* PARSER */
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
	DOloop *d = NULL;
	int i;

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

	/* if the prev is <DO> */
	if(!strcmp(p->str, "DO")) {
		nextOne = p;
		do {
			nextOne = nextOne->next;
		} while(strcmp(nextOne->str, "{") != 0);

		d = DO_loop(p);

		for(i = d->from; i <= d->to; i++) {
			p->var_array[d->var - CHARACTERCODE] = (double) i;
			execute_DO(nextOne->next);
		}

		do {
			nextOne = nextOne->next;
		} while(!check_curlybracket(nextOne));

		free(d);

		return check_INSTRCTLST(nextOne);
	}

	/* if the prev is <SET> */
	if(!strcmp(p->str, "SET")) {
		nextOne = p;

		set_value(nextOne);

		do {
			nextOne = nextOne->next;
		} while(strcmp(nextOne->str, ";") != 0);

		if(!strcmp(nextOne->str, ";")) {
			return check_INSTRCTLST(nextOne->next);
		}
	}

	/* if the prev is <FD> | <LT> | <RT> | <RE> | <TR> */
	if(!strcmp(p->str, "FD") || !strcmp(p->str, "LT") || !strcmp(p->str, "RT") ||
	 !strcmp(p->str, "RE") ||  !strcmp(p->str, "TR")) {
		execute_code(p, p->sw, p->loc, p->vec);

		nextOne = p->next->next;
		return check_INSTRCTLST(nextOne);
	}

	/* if the prev is <SP> */
	if(!strcmp(p->str, "SP")) {
		nextOne = p;

		start_point(nextOne);

		do {
			nextOne = nextOne->next;
		} while(strcmp(nextOne->str, ";") != 0);

		if(!strcmp(nextOne->str, ";")) {
			return check_INSTRCTLST(nextOne->next);
		}
	}

	/* if the prev is <CL> */
	if(!strcmp(p->str, "CL")) {
		nextOne = p;

		change_color(nextOne);

		do {
			nextOne = nextOne->next;
		} while(strcmp(nextOne->str, ";") != 0);

		if(!strcmp(nextOne->str, ";")) {
			return check_INSTRCTLST(nextOne->next);
		}
	}

	/* if the prev is <VC> */
	if(!strcmp(p->str, "VC")) {
		nextOne = p;

		change_vector(nextOne);

		do {
			nextOne = nextOne->next;
		} while(strcmp(nextOne->str, ";") != 0);

		if(!strcmp(nextOne->str, ";")) {
			return check_INSTRCTLST(nextOne->next);
		}
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

	if(!strcmp(p->str, "RE")) {
		if(check_RETR(p)) {
			return true;
		}
		fprintf(stderr, "!!ERROR!! Expected <RE> followed by <VARNUM>\n");
		return false;
	}

	if(!strcmp(p->str, "TR")) {
		if(check_RETR(p)) {
			return true;
		}
		fprintf(stderr, "!!ERROR!! Expected <TR> followed by <VARNUM>\n");
		return false;
	}

	if(!strcmp(p->str, "SP")) {
		if(check_SP(p)) {
			return true;
		}

		fprintf(stderr, "!!ERROR!! Expected <SP> followed by <VARNUM> <VARNUM> ';'\n");
		return false;
	}

	if(!strcmp(p->str, "CL")) {
		if(check_CL(p)) {
			return true;
		}

		fprintf(stderr, "!!ERROR!! Expected <CL> followed by number number number ';'\n");
		return false;
	}

	if(!strcmp(p->str, "VC")) {
		if(check_VC(p)) {
			return true;
		}

		fprintf(stderr, "!!ERROR!! Expected <VC> followed by <VARNUM> <VARNUM> ';'\n");
		return false;
	}

	fprintf(stderr, "!!ERROR!! Expected INSTRUCTION to be <FD> | <LT> | <RT> | <DO> | <SET> | <RE> | <TR> | <SP> | <VC>\n");
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

								if(!strcmp(current->str, "FD") || !strcmp(current->str, "LT") || !strcmp(current->str, "RT") ||
								 !strcmp(current->str, "RE") ||  !strcmp(current->str, "TR")) {
									current = current->next->next;
								}

								if(!strcmp(current->str, "SET") || !strcmp(current->str, "SP") || !strcmp(current->str, "CL") ||
								 !strcmp(current->str, "VC")) {
									do {
										current = current->next;
									} while(strcmp(current->str, ";") != 0);
									current = current->next;
								}

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
prog* prog_init(int strSize, location *l, vector *v, double *var, SDL_Simplewin *sw)
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

	program->var_array = var;

	program->loc = l;

	program->vec = v;

	program->sw = sw;

	return program;
}

prog* prog_insert(char *instrc, location *l, vector *v, double *var, SDL_Simplewin *sw)
{
	prog *new;

	new = prog_init(strlen(instrc), l, v, var, sw);
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

/* INTERPRETER */
DOloop* DO_loop(prog *p)
{
	prog *nextOne;
	DOloop *d;

	d = (DOloop*) calloc(1, sizeof(DOloop));
	if(d == NULL){
		ERROR("Cannot create a Code Space\n");
	}

	/* jump to var */
	nextOne = p->next;
	sscanf(nextOne->str, "%c", &d->var);

	/* jump to from varnum */
	nextOne = nextOne->next->next;
	sscanf(nextOne->str, "%d", &d->from);

	/* jump to to varnum */
	nextOne = nextOne->next->next;
	sscanf(nextOne->str, "%d", &d->to);

	/* save into the array */
	p->var_array[d->var - CHARACTERCODE] = (double) d->from;


	return d;
}

location* draw_line(SDL_Simplewin *sw, double operand, location *loc, vector *vec)
{
	double nextX, nextY;

	nextX = loc->x + vec->x * operand;
	nextY = loc->y + vec->y * operand;
	printf("start coordinate：%f, %f\n", loc->x, loc->y);
	printf("length：%f\n", operand);

	SDL_RenderDrawLine(sw->renderer, loc->x, loc->y, nextX, nextY);

	loc->x = nextX;
	loc->y = nextY;
	printf("new coordinate：%f, %f\n", nextX, nextY);

	return loc;
}

vector* turn_LToRT(char *str, double operand, vector *vec)
{
	double nextVecX, nextVecY;

	if(!strcmp(str, "LT")) {
	  operand = operand * -1;
	}

	nextVecX = cos(operand * VAL) * vec->x - sin(operand * VAL) * vec->y;
	nextVecY = sin(operand * VAL) * vec->x + cos(operand * VAL) * vec->y;

	vec->x = nextVecX;
	vec->y = nextVecY;
	printf("angle：%f\n", operand);
	printf("directional vector：%f, %f\n", nextVecX, nextVecY);

	return vec;
}

void execute_code(prog *p, SDL_Simplewin *sw, location *loc, vector *vec)
{
	double operand = 0.0;
	char op = '\0';

	if(!strcmp(p->str, "FD")) {
		if(!check_VAR(p->next->str)) {
			sscanf(p->next->str, "%lf", &operand);
		}
		else {
			sscanf(p->next->str, "%c", &op);
			operand = p->var_array[op - CHARACTERCODE];
		}
		loc = draw_line(sw, operand, loc, vec);
	}
	if(!strcmp(p->str, "LT")) {
		if(!check_VAR(p->next->str)) {
			sscanf(p->next->str, "%lf", &operand);
		}
		vec = turn_LToRT(p->str, operand, vec);
	}
	if(!strcmp(p->str, "RT")) {
		if(!check_VAR(p->next->str)) {
			sscanf(p->next->str, "%lf", &operand);
		}
		vec = turn_LToRT(p->str, operand, vec);
	}

	if(!strcmp(p->str, "RE")) {
		if(!check_VAR(p->next->str)) {
			sscanf(p->next->str, "%lf", &operand);
		}
		else {
			sscanf(p->next->str, "%c", &op);
			operand = p->var_array[op - CHARACTERCODE];
		}
		draw_rect(p, sw, operand);
	}

	if(!strcmp(p->str, "TR")) {
		if(!check_VAR(p->next->str)) {
			sscanf(p->next->str, "%lf", &operand);
		}
		else {
			sscanf(p->next->str, "%c", &op);
			operand = p->var_array[op - CHARACTERCODE];
		}
		draw_tri(p, sw, operand);
	}
}

void execute_DO(prog *p)
{
	prog *nextOne;
	DOloop *d;
	int i;

	nextOne = p;

	while(strcmp(nextOne->str, "}") != 0) {
		if(!strcmp(nextOne->str, "FD") || !strcmp(nextOne->str, "LT") || !strcmp(nextOne->str, "RT") ||
		 !strcmp(nextOne->str, "RE") ||  !strcmp(nextOne->str, "TR")) {
			execute_code(nextOne, p->sw, p->loc, p->vec);

			nextOne = nextOne->next->next;
		}

		if(!strcmp(nextOne->str, "SET")) {
			set_value(nextOne);

			do {
				nextOne = nextOne->next;
			} while(strcmp(nextOne->str, ";") != 0);

			if(!strcmp(nextOne->str, ";")) {
				nextOne = nextOne->next;
			}
		}

		if(!strcmp(nextOne->str, "DO")) {
			d = DO_loop(nextOne);

			do {
				nextOne = nextOne->next;
			} while(strcmp(nextOne->str, "{") != 0);

			for(i = d->from; i <= d->to; i++) {
				p->var_array[d->var - CHARACTERCODE] = (double) i;
				execute_DO(nextOne->next);
			}

			do {
				nextOne = nextOne->next;
			} while(strcmp(nextOne->str, "}") != 0);

			nextOne = nextOne->next;
		}

		if(!strcmp(nextOne->str, "SP")) {
			start_point(nextOne);

			do {
				nextOne = nextOne->next;
			} while(strcmp(nextOne->str, ";") != 0);

			nextOne = nextOne->next;
		}

		if(!strcmp(nextOne->str, "CL")) {
			change_color(nextOne);

			do {
				nextOne = nextOne->next;
			} while(strcmp(nextOne->str, ";") != 0);

			nextOne = nextOne->next;
		}

		if(!strcmp(nextOne->str, "VC")) {
			change_vector(nextOne);

			do {
				nextOne = nextOne->next;
			} while(strcmp(nextOne->str, ";") != 0);

			nextOne = nextOne->next;
		}
	}
}

/* stack */
void stack_init(stack *s)
{
	s->top = 0;
}

void push(stack *s, double value)
{
	if(s->top < STACKSIZE) {
		s->stk[s->top].value = value;
		s->top = s->top + 1;
	}
}

double pop(stack *s)
{
	if(s->top > 0) {
		s->top = s->top - 1;

		return s->stk[s->top].value;
	}

	return 0.0;
}

void set_value(prog *p)
{
	prog *nextOne;
	stack s;
	double a, b, ans;
	char d, op;

	stack_init(&s);

	/* jump to var */
	nextOne = p->next;
	sscanf(nextOne->str, "%c", &op);

	/* jump to polish */
	nextOne = nextOne->next->next;

	if(check_VAR(nextOne->str) || sscanf(nextOne->str, "%lf", &a)) {
		if(check_VAR(nextOne->next->str) || sscanf(nextOne->next->str, "%lf", &a)) {
			do {
				if(sscanf(nextOne->str, "%lf", &a)) {
				}
				if(check_VAR(nextOne->str)) {
					sscanf(nextOne->str, "%c", &d);
					a = p->var_array[d - CHARACTERCODE];
				}
				if(!strcmp(nextOne->str, "+")) {
					a = pop(&s);
					b = pop(&s);
					a = a + b;
				}
				if(!strcmp(nextOne->str, "-")) {
					a = pop(&s);
					b = pop(&s);
					a = a - b;
				}
				if(!strcmp(nextOne->str, "/")) {
					a = pop(&s);
					b = pop(&s);
					a = b / a;
				}
				if(!strcmp(nextOne->str, "*")) {
					a = pop(&s);
					b = pop(&s);
					a = a * b;
				}
				push(&s, a);
				nextOne = nextOne->next;
			} while(strcmp(nextOne->str, ";") != 0 );
		}
	}
	else {

		fprintf(stderr, "!!ERROR!! <POLISH> should be Reverse Polish Notation\n");

	}
	ans = pop(&s);
	p->var_array[op - CHARACTERCODE] = ans;
}

/* EXTENSION */
/* for <RE> ::= "RE" <VARNUM> */
/* for <TR> ::= "TR" <VARNUM> */
bool check_RETR(prog *p)
{
	prog *nextOne;

	nextOne = p->next;

	if(check_VARNUM(nextOne->str)) {
		return true;
	}

	return false;
}

/* for <SP> ::= "SP" <VARNUM> <VARNUM> ";" */
bool check_SP(prog *p)
{
	prog *current;

	current = p;

	current = p->next;

	if(check_VARNUM(current->str)) {
		current = current->next;
		if(check_VARNUM(current->str)) {
			current = current->next;
			if(!strcmp(current->str, ";")) {
				return true;
			}
		}
	}

	return false;
}

/* for <CL> ::= "CL" number number number ";" */
bool check_CL(prog *p)
{
	prog *current;
	int r, g, b;

	current = p->next;

	if(sscanf(current->str, "%d", &r)) {
		current = current->next;

		if(sscanf(current->str, "%d", &g)) {
			current = current->next;

			if(sscanf(current->str, "%d", &b)) {
				current = current->next;

				if(!strcmp(current->str, ";")) {
					return true;
				}
			}
		}
	}

	return false;
}

bool check_VC(prog *p)
{
	prog *current;

	current = p;

	current = p->next;

	if(check_VARNUM(current->str)) {
		current = current->next;
		if(check_VARNUM(current->str)) {
			current = current->next;
			if(!strcmp(current->str, ";")) {
				return true;
			}
		}
	}

	return false;
}

void draw_rect(prog *p, SDL_Simplewin *sw, double operand)
{
	SDL_Rect rectangle;
	rectangle.w = operand;
	rectangle.h = operand;
	rectangle.x = p->loc->x;
	rectangle.y = p->loc->y;
	SDL_RenderDrawRect(sw->renderer, &rectangle);
	p->vec->x = 0;
	p->vec->y = -1;
}

void draw_tri(prog *p, SDL_Simplewin *sw, double operand)
{
	vector *newVec;
	location *newLoc;
	int i;
	double angle = 120.0;
	char *str = "RT";

	newVec = p->vec;

	newLoc = p->loc;

	for(i = 0; i < 3; i++) {
		newLoc = draw_line(sw, operand, newLoc, newVec);
		newVec = turn_LToRT(str, angle, newVec);
	}
	printf("%f\n", newVec->x);
}

void start_point(prog *p)
{
	prog *current;
	double operandX, operandY;
	char op = '\0';

	current = p->next;


	if(!check_VAR(current->str)) {
		sscanf(current->str, "%lf", &operandX);
	}
	else {
		sscanf(current->str, "%c", &op);
		operandX = p->var_array[op - CHARACTERCODE];
	}

	current = current->next;

	if(!check_VAR(current->str)) {
		sscanf(current->str, "%lf", &operandY);
	}
	else {
		sscanf(current->str, "%c", &op);
		operandY = p->var_array[op - CHARACTERCODE];
	}

	p->loc->x = operandX;
	p->loc->y = operandY;

	printf("new starting coordinate：%f, %f\n", operandX, operandY);
}

void change_color(prog *p)
{
	prog *current;
	int r, g, b;

	current = p->next;

	sscanf(current->str, "%d", &r);
	current = current->next;

	sscanf(current->str, "%d", &g);
	current = current->next;

	sscanf(current->str, "%d", &b);
	current = current->next;

	Neill_SDL_SetDrawColour(p->sw, r, g, b);
}

void change_vector(prog *p)
{
	prog *current;
	double x, y;

	current = p->next;

	sscanf(current->str, "%lf", &x);
	current = current->next;

	sscanf(current->str, "%lf", &y);
	current = current->next;

	p->vec->x = x;
	p->vec->y = y;
}
