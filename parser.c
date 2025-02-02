#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "AST.h"
#include "parser.h"
#include "token.h"
#include "token_array.h"

static struct Token const * program;
static size_t prog_size;
static struct Token const * current;

static void print_error() {
  printf("parser failed at offset %ld\n", (size_t)(current-program));
}

static bool consume(const enum TokenType expected) {
  struct Token const found = *current;
  if (expected == found.type) {
    current++;
    return true;
  } else {
    return false;
  }
}

static bool peek(const enum TokenType expected) {
  struct Token const found = *current;
  if (expected == found.type) {
    return true;
  } else {
    return false;
  }
}

static union TokenVariant* consume_with_data(const enum TokenType expected) {
  if (expected == current->type) {
    current++;
    return &((current - 1)->data);
  } else {
    return NULL;
  }
}

static enum UnOp consume_unary_op(){
  if (consume(TILDE)) return COMPLEMENT;
  if (consume(MINUS)) return NEGATE;
  if (consume(EXCLAMATION)) return BOOL_NOT;
  return 0;
}

static enum BinOp consume_binary_op(){
  if (consume(PLUS)) return ADD_OP;
  if (consume(MINUS)) return SUB_OP;
  if (consume(ASTERISK)) return MUL_OP;
  if (consume(SLASH)) return DIV_EQ;
  if (consume(PERCENT)) return MOD_OP;
  if (consume(AMPERSAND)) return BIT_AND;
  if (consume(PIPE)) return BIT_OR;
  if (consume(CARAT)) return BIT_XOR;
  if (consume(DOUBLE_AMPERSAND)) return BOOL_AND;
  if (consume(DOUBLE_PIPE)) return BOOL_OR;
  if (consume(DOUBLE_EQUALS)) return BOOL_EQ;
  if (consume(NOT_EQUAL)) return BOOL_NEQ;
  if (consume(LESS_THAN)) return BOOL_LE;
  if (consume(LESS_THAN_EQ)) return BOOL_LEQ;
  if (consume(GREATER_THAN)) return BOOL_GE;
  if (consume(GREATER_THAN_EQ)) return BOOL_GEQ;
  if (consume(EQUALS)) return ASSIGN_OP;
  if (consume(PLUS_EQ)) return PLUS_EQ_OP;
  if (consume(MINUS_EQ)) return MINUS_EQ_OP;
  if (consume(TIMES_EQ)) return MUL_EQ_OP;
  if (consume(DIV_EQ)) return DIV_EQ_OP;
  if (consume(MOD_EQ)) return MOD_EQ_OP;
  if (consume(AND_EQ)) return AND_EQ_OP;
  if (consume(OR_EQ)) return OR_EQ_OP;
  if (consume(XOR_EQ)) return XOR_EQ_OP;
  if (consume(SHL_EQ)) return SHL_EQ_OP;
  if (consume(SHR_EQ)) return SHR_EQ_OP;
  if (consume(QUESTION)) return TERNARY_OP;
  return 0;
}

struct Expr* parse_pre_op(){

}

struct Expr* parse_cast(){
  
}

struct Expr* parse_post_op(){
  
}

struct Expr* parse_parens(){
  
}

struct Expr* parse_func_call(){
  
}

/*
liftA2 Unary
  (getUnaryOp <$> satisfy isUnaryOp)
  parseFactor <|>
  parsePreIncDec <|>
  char Ampersand *> (AddrOf <$> parseExpr) <|>
  char Asterisk *> (Dereference <$> (parseUnary <|> parseVar))
*/

struct Expr* parse_unary(){
  enum UnOp op;
  if (op = consume_unary_op()){
    struct Expr* inner = parse_factor();
    if (inner == NULL) {
      current--;
      return NULL;
    }
    struct UnaryExpr expr = {op, inner};
    struct Expr* result = malloc(sizeof(struct Expr));
    result->expr.un_expr = expr;
    result->type = UNARY;
    return result;
  }

  struct Expr* expr;
  if (expr = parse_pre_op()){
    return expr;
  } 
  // Dereference and AddrOf, need "expect()" function
  //else if (){
  //
  //} else if (){
  //   
  //}
  return NULL;
}

struct Expr* parse_factor(){
  union TokenVariant* data;
  if (data = consume_with_data(INT_LIT)){
    union ConstVariant const_data;
    const_data.int_val = data->int_val;

    struct LitExpr lit_expr = {INT_CONST, const_data};

    struct Expr* expr = malloc(sizeof(struct Expr));
    expr->expr.lit_expr = lit_expr;
    expr->type = LIT;
    return expr;
  } else if (data = consume_with_data(U_INT_LIT)){
    union ConstVariant const_data;
    const_data.uint_val = data->uint_val;

    struct LitExpr lit_expr = {UINT_CONST, const_data};

    struct Expr* expr = malloc(sizeof(struct Expr*));
    expr->expr.lit_expr = lit_expr;
    expr->type = LIT;
    return expr;
  } else if (data = consume_with_data(LONG_LIT)){
    union ConstVariant const_data;
    const_data.long_val = data->long_val;

    struct LitExpr lit_expr = {LONG_CONST, const_data};

    struct Expr* expr = malloc(sizeof(struct Expr*));
    expr->expr.lit_expr = lit_expr;
    expr->type = LIT;
    return expr;
  } else if (data = consume_with_data(U_LONG_LIT)){
    union ConstVariant const_data;
    const_data.ulong_val = data->ulong_val;

    struct LitExpr lit_expr = {U_LONG_LIT, const_data};

    struct Expr* expr = malloc(sizeof(struct Expr*));
    expr->expr.lit_expr = lit_expr;
    expr->type = LIT;
    return expr;
  } else if (data = consume_with_data(IDENT)){
    struct VarExpr var_expr = { data->ident_name };

    struct Expr* expr = malloc(sizeof(struct Expr*));
    expr->expr.var_expr = var_expr;
    expr->type = VAR;
    return expr;
  }
  
  struct Expr* expr;
  if (expr = parse_unary()){
    return expr;
  } else if (expr = parse_cast()){
    return expr;
  } else if (expr = parse_post_op()){
    return expr;
  } else if (expr = parse_parens()){
    return expr;
  } else if (expr = parse_func_call()){
    return expr;
  } else {
    return NULL;
  }
}

static unsigned get_prec(enum BinOp op){
  switch (op){
    case DIV_OP:
    case MUL_OP:
    case MOD_OP:
      return 50;
    case ADD_OP:
    case SUB_OP:
      return 45;
    case BIT_SHL:
    case BIT_SHR:
      return 40;
    case BOOL_LE:
    case BOOL_GE:
    case BOOL_LEQ:
    case BOOL_GEQ:
      return 35;
    case BOOL_EQ:
    case BOOL_NEQ:
      return 30;
    case BIT_AND:
      return 25;
    case BIT_XOR:
      return 20;
    case BIT_OR:
      return 15;
    case BOOL_AND:
      return 10;
    case BOOL_OR:
      return 5;
    case TERNARY_OP:
      return 3;
    case ASSIGN_OP:
    case PLUS_EQ_OP:
    case MINUS_EQ_OP:
    case MUL_EQ_OP:
    case DIV_EQ_OP:
    case MOD_EQ_OP:
    case AND_EQ_OP:
    case OR_EQ_OP:
    case XOR_EQ_OP:
    case SHL_EQ_OP:
    case SHR_EQ_OP:
      return 1;
  }
}

struct Expr* parse_bin_expr(unsigned min_prec){
  struct Expr* lhs = parse_factor();

  if (lhs == NULL) return NULL;

  while (current - program < prog_size){ 
    enum BinOp op = consume_binary_op();
    unsigned prec = get_prec(op);

    if (prec < min_prec) break; // stop expansion if a lower precedence operator is encountered

    struct Expr* rhs = parse_bin_expr(prec + 1);

    if (rhs == NULL){
      print_error();
      return NULL;
    }

    union ExprVariant expr_union;
    struct BinaryExpr bin_expr = {op, lhs, rhs};
    expr_union.bin_expr = bin_expr;
    lhs = malloc(sizeof(struct Expr));
    lhs->type = BINARY;
    lhs->expr = expr_union;
  }

  return lhs;
}

struct Expr* parse_expr(){
  return parse_bin_expr(0);
}

struct Expr* parse_expr_test(struct TokenArray* arr){
  program = arr->tokens;
  current = program;
  prog_size = arr->size;
  return parse_expr();
}

struct Declaration* parse_dclr(){

}

struct Program* parse_prog(struct TokenArray* arr){
  // parse declarations and put them in a linked list
  program = arr->tokens;
  current = program;
  prog_size = arr->size;

  struct Program* prog = malloc(sizeof(struct Program));
  struct DeclarationList* dclrs = malloc(sizeof(struct DeclarationList));

  prog->dclrs = dclrs;

  struct DeclarationList* current_dclr = dclrs;

  struct Declaration* dclr = parse_dclr();

  if (dclr == NULL) {
    // there were 0 declarations
    prog->dclrs = NULL;
    free(dclrs);
    return prog;
  }

  while (dclr != NULL){
    // TODO: this order is wrong
    struct DeclarationList* next_dclr = malloc(sizeof(struct DeclarationList));
    next_dclr->dclr = *dclr;
    next_dclr->next = NULL;

    current_dclr->next = next_dclr;
    current_dclr = next_dclr;

    free(dclr);
    dclr = parse_dclr();
  }

  return prog;
}