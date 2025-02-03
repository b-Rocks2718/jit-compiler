#pragma once

#include "AST.h"
#include "token_array.h"

struct Expr* parse_expr_test(struct TokenArray* arr);

struct Expr* parse_expr();

struct Expr* parse_unary();

struct Expr* parse_bin_expr();

struct Expr* parse_factor();

struct Expr* parse_var();