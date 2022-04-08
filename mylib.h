#pragma once
#include "symbols.h"
#include "stack.h"
#include "expressions.h"
#include "list.h"
#include "trans_errors.h"

const char *empty_string = "";
int streql(char *s1, char *s2);
extern void trans_err(int type, int ln);
extern void trans_err_x(int type, int ln, char *expr);