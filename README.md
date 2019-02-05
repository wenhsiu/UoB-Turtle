# Introduction
This is an C assignment in UoB. The goal is to create a paser and interpreter. Therefore the files contain 3 parts: parser, interpreter and extension.

## Parser
Create a parser that could read a different formal grammar and check if it is correct.
Files: parser, turtleP, test

## Interpreter
The interpreter will understand what INSTRCTION means and executes. The output of the codes is via SDL.
Files: interpreter, turtleI

## Extension
In this section, I extended the formal grammar to provide more INSTRUCTIONs.
Files: extension, turtleE

# Formal Grammar

## Basic
```
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
```
## Extension
```
<MAIN> ::= "{" <INSTRCTLST>
<INSTRCTLST> ::= <INSTRUCTION><INSTRCTLST> | "}"
<INSTRUCTION> ::= <FD> | <LT> | <RT> | <DO> | <SET> | <RE> | <TR> | <SP>
<FD> ::= "FD" <VARNUM>
<LT> ::= "LT" <VARNUM>
<RT> ::= "RT" <VARNUM>
<RE> ::= "RE" <VARNUM>
<TR> ::= "TR" <VARNUM>
<SP> ::= "SP" <VARNUM> <VARNUM> ";"
<CL> ::= "CL" number number number ";"
<DO> ::= "DO" <VAR> "FROM" <VARNUM> "TO" <VARNUM> "{" <INSTRCTLST>
<VAR> ::= [A−Z]
<VARNUM> ::= number | <VAR>
<SET> ::= "SET" <VAR> ":=" <POLISH>
<POLISH> ::= <OP> <POLISH> | <VARNUM> <POLISH> | ";"
<OP> ::= "+" | "−" | "*" | "/"
```
# Examples
```
{
	DO A FROM 1 TO 12 {
		TR 40
		LT 30
		FD 40
	}
}
```
(/Test1.png)

```
{
	DO A FROM 1 TO 4 {
		FD 40
		LT 45
		CL 255 0 0 ;
		FD 40
		LT 45
		CL 255 255 255 ;
	}
}
```
(/Test2.png)

```
{
	CL 255 255 0 ;
	SP 100 150 ;
	DO A FROM 1 TO 12 {
		TR 20
		LT 30
		FD 20
	}

	CL 0 0 255 ;
	SP 50 400 ;
	LT 45
	DO B FROM 1 TO 20 {
		FD 20
		RT 90
		FD 20
		LT 90
	}

	SP 50 450 ;
	DO C FROM 1 TO 17 {
		FD 20
		RT 90
		FD 20
		LT 90
	}

	SP 50 500 ;
	DO D FROM 1 TO 15 {
		FD 20
		RT 90
		FD 20
		LT 90
	}

	CL 255 191 88 ;
	SP 500 520 ;
	DO E FROM 1 TO 3 {
		FD 100
		RT 10
	}

	CL 175 255 255 ;
	SP 300 350 ;
	RT 15
	FD 80
	RT 120
	FD 30
	RT 60
	FD 50
	RT 60
	FD 30

	SP 340 350 ;
	RT 30
	FD 20
	TR 30

	CL 132 79 0 ;
	SP 670 370 ;
	FD 50
	SP 685 365 ;
	FD 45

	CL 0 127 0 ;
	SP 665 330 ;
	LT 90
	FD 3
	DO F FROM 1 TO 7 {
		RT 45
		FD 30
	}
	RT 45
	FD 3

	CL 255 255 255 ;
	SP 300 150 ;
	RT 180
	FD 20
	RT 60
	FD 10
	LT 120
	FD 10
	RT 60
	FD 20

	SP 500 100 ;
	LT 30
	FD 20
	RT 60
	FD 10
	LT 120
	FD 10
	RT 60
	FD 20

	SP 450 200 ;
	RT 45
	FD 20
	RT 60
	FD 10
	LT 120
	FD 10
	RT 60
	FD 20
}
```
(/Test3.png)


