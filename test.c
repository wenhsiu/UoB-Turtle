#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

#include "turtleP.h"

#define ERROR(STR) fprintf(stderr, STR); exit(EXIT_FAILURE)

void testing(void);

void testing(void)
{
	prog *start, *end;
	prog *FD;
	prog *LT;
	prog *RT;
	prog *SET;
	prog *DO;

	prog *var1, *var2;
	prog *equ;
	prog *po1;
	prog *po2;
	prog *semi;
	prog *varnum1, *varnum2, *varnum3, *varnum4, *varnum5;
	prog *from;
	prog *to;
	prog *curlyr1, *curlyr2;

	start = prog_insert("{");
	end = prog_insert("}");
	FD = prog_insert("FD");
	LT = prog_insert("LT");
	RT = prog_insert("RT");
	varnum1 = prog_insert("45");
	varnum2 = prog_insert("45");
	varnum3 = prog_insert("45");
	SET = prog_insert("SET");
	var1 = prog_insert("A");
	equ = prog_insert(":=");
	po1 = prog_insert("4");
	po2 = prog_insert("*");
	semi = prog_insert(";");
	DO = prog_insert("DO");
	var2 = prog_insert("B");
	from = prog_insert("FROM");
	varnum4 = prog_insert("1");
	to = prog_insert("TO");
	varnum5 = prog_insert("5");
	curlyr1 = prog_insert("{");
	curlyr2 = prog_insert("}");



	start->next = FD;
	FD->next = varnum1;
	varnum1->next = RT;
	RT->next = varnum3;
	varnum3->next = SET;
	SET->next = var1;
	var1->next = equ;
	equ->next = po1;
	po1->next = po2;
	po2->next = semi;
	semi->next = DO;
	DO->next = var2;
	var2->next = from;
	from->next = varnum4;
	varnum4->next = to;
	to->next = varnum5;
	varnum5->next = curlyr1;
	curlyr1 ->next = LT;
	LT->next = varnum2;
	varnum2->next = curlyr2;
	curlyr2->next = end;
	

	assert(check_main(start));
	assert(check_INSTRCTLST(FD));
	assert(check_INSTRCTION(FD));
	assert(check_FDLTRT(FD));
	assert(check_INSTRCTION(RT));
	assert(check_FDLTRT(RT));
	assert(check_INSTRCTION(SET));
	assert(check_SET(SET));
	assert(check_POLISH(po1));
	assert(check_INSTRCTION(DO));
	assert(check_DO(DO));



	assert(check_VARNUM("30"));
	assert(check_VARNUM("A"));
	assert(check_VAR("?") == false);
	assert(check_VAR("30") == false);
	assert(check_VAR("A"));
	assert(check_VAR("a") == false);
	assert(check_OP("+"));
	assert(check_OP("-"));
	assert(check_OP("*"));
	assert(check_OP("/"));
	assert(check_OP("?") == false);

	prog_free(&start);
	assert(start == NULL);

}
