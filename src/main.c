#include "KariLang/src/common.h"
#include <stdio.h>
#include <string.h>

void *yy_scan_string(const char *);

char *STDOUT_REDIRECT_STRING;
char *STDERR_REDIRECT_STRING;

IMPLEMENT_HASH_FUNCTION;
DS_TABLE_DEF(ast, AST, clear_ast);

ast_table_t *ast;
const char *filename;

bool cli_interpretation_mode = false;
int interactive_interpretation();
int file_interpretation(const char *file_name, int input);

int init_language() {
    cli_interpretation_mode = true;

    ast = ast_table_new(100);

    globalBooleans = boolean_table_new(100);
    globalIntegers = integer_table_new(100);

    STDOUT_REDIRECT_STRING = calloc(STDOUT_STRING_LENGTH, 1);
    STDERR_REDIRECT_STRING = calloc(STDERR_STRING_LENGTH, 1);

    return 0;
}

int execute_string(const char *string) {
    yy_scan_string(string);
    return yyparse();
}
