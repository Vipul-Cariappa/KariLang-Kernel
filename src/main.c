#include "KariLang/src/common.h"
#include <stdio.h>
#include <string.h>

void* yy_scan_string(const char *);

IMPLEMENT_HASH_FUNCTION;
DS_TABLE_DEF(ast, AST, clear_ast);

ast_table_t *ast;
const char *filename;

bool cli_interpretation_mode = false;
int interactive_interpretation();
int file_interpretation(const char *file_name, int input);
