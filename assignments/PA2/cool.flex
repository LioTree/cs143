/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */
%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
/* #define yylex  cool_yylex */
extern "C" int yylex(void);
extern int cool_yylex(void) {
    return yylex();
}

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */
int comment_count = 0;

%}

/*
 * Define names for regular expressions here.
 */

DARROW          =>
ASSIGN          <-
LE              <=
SYNTAX          \{|\}|\(|\)|:|\+|-|\*|\/|~|<|=|;|,|\.|@
INTEGERS        [0-9]+
TYPE_IDENTIFIER [A-Z][a-zA-Z0-9_]*    
OBJECT_IDENTIFIER [a-z][a-zA-Z0-9_]*    
NEWLINE         \n
OTHER_WHITESPACE     [ ]|\f|\r|\t|\v

%s IN_COMMENT
%s IN_COMMENT2
%s IN_STRING
%%

 /*
  *  Nested comments
  */
<INITIAL>{
  "(*"              { comment_count++;BEGIN(IN_COMMENT); }
  "--"              BEGIN(IN_COMMENT2);
}

<IN_COMMENT>{
  "(*"      comment_count++;
  "*)"      { 
              comment_count--;
              if(!comment_count) {
                BEGIN(INITIAL); 
              }
            }
  [^*\(\n]+   // eat comment in chunks
  "*"       // eat the lone star
  "("       // eat the lone left bracket
  \n        curr_lineno++;
}

<IN_COMMENT2>{
  .+       // eat the lone star
  \n       { curr_lineno++;BEGIN(INITIAL); }
}

 /*
  *  The multiple-character operators.
  */
<INITIAL>{
  {DARROW}		{ return (DARROW); }
  {ASSIGN}		{ return (ASSIGN); }
  {LE}		    { return (LE); }
}

 /*
  * The single-character operators. 
  */
<INITIAL>{
  {SYNTAX}     { return int(yytext[0]); }
}

 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */
<INITIAL>{
  (?i:class)  { return CLASS; }
  (?i:else)   { return ELSE; }
  (?i:fi)       { return FI; }
  (?i:if)       { return IF; }
  (?i:in)       { return IN; }
  (?i:inherits) { return INHERITS; }
  (?i:isvoid)   { return ISVOID; }
  (?i:let)      { return LET; }
  (?i:loop)     { return LOOP; }
  (?i:pool)     { return POOL; }
  (?i:then)     { return THEN; }
  (?i:while)    { return WHILE; }
  (?i:case)     { return CASE; }
  (?i:esac)     { return ESAC; }
  (?i:new)      { return NEW; }
  (?i:of)       { return OF; }
  (?i:not)      { return NOT; }
  t(?i:rue)    { cool_yylval.boolean = 1;return BOOL_CONST; }
  f(?i:alse)    { cool_yylval.boolean = 0;return BOOL_CONST; }
}


 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for 
  *  \n \t \b \f, the result is c.
  *
  */
<INITIAL>{
  "\""       { string_buf_ptr = string_buf;*string_buf_ptr = '\0';BEGIN(IN_STRING); }
}
<IN_STRING>{
  \x00        { printf("\\0 error\n");exit(0); }
  \\(.|\n)       { 
                  if((string_buf_ptr - string_buf) >= (MAX_STR_CONST - 2)) {
                    printf("string too long");
                    exit(0);
                  }
                  switch(yytext[1]) {
                    case 'b':
                      *string_buf_ptr++ = '\b';
                      break;
                    case 't':
                      *string_buf_ptr++ = '\t';
                      break;
                    case 'n':
                      *string_buf_ptr++ = '\n';
                      break;
                    case 'f':
                      *string_buf_ptr++ = '\f';
                      break;
                    default:
                      *string_buf_ptr++ = yytext[1];
                  }
                  *string_buf_ptr = '\0'; 
                }
  "\""       { cool_yylval.symbol = stringtable.add_string(string_buf);BEGIN(INITIAL);return STR_CONST; }
  .          { 
              if((string_buf_ptr - string_buf) >= (MAX_STR_CONST - 2)) {
                printf("string too long");
                exit(0);
              }
              *string_buf_ptr++ = yytext[0];
              *string_buf_ptr = '\0'; 
             }
}

<INITIAL>{
  {INTEGERS}    { cool_yylval.symbol = inttable.add_string(yytext);return INT_CONST; }
  {TYPE_IDENTIFIER}  { cool_yylval.symbol = idtable.add_string(yytext);return TYPEID; }
  {OBJECT_IDENTIFIER}  { cool_yylval.symbol = idtable.add_string(yytext);return OBJECTID; }

  {NEWLINE}     { curr_lineno++; }
  {OTHER_WHITESPACE} {}
}

%%
