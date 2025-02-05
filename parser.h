#pragma once

#include "AST.h"
#include "token_array.h"

struct Statement* parse_test(struct TokenArray* arr);

struct Statement* parse_statement();

struct Expr* parse_expr();

struct Expr* parse_unary();

struct Expr* parse_bin_expr();

struct Expr* parse_factor();

struct Expr* parse_var();

struct AbstractDeclarator* parse_abstract_declarator();