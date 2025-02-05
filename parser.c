#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "AST.h"
#include "parser.h"
#include "token.h"
#include "token_array.h"

static struct Token * program;
static size_t prog_size;
static struct Token * current;

static void print_error() {
  printf("parser failed at offset %ld\n", (size_t)(current-program));
}

static bool consume(const enum TokenType expected) {
  if (current - program < prog_size && expected == current->type) {
    current++;
    return true;
  } else {
    return false;
  }
}

static union TokenVariant* consume_with_data(const enum TokenType expected) {
  if (current - program < prog_size && expected == current->type) {
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
  if (consume(SLASH)) return DIV_OP;
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

struct Expr* parse_paren_var(){
  struct Expr* expr;
  if ((expr = parse_var())) return expr;
  else if (consume(OPEN_P)){
    struct Token* old_current = current - 1;
    struct Expr* inner = parse_paren_var();
    if (inner == NULL) {
      current = old_current;
      return NULL;
    } else if (!consume(CLOSE_P)){
      current = old_current;
      destroy_expr(inner);
      return NULL;
    }
    return inner;
  } else return NULL;
}

struct Expr* parse_pre_op(){
  if (consume(INC_TOK)){
    struct Expr* inner = parse_paren_var();
    if (inner == NULL) {
      current--;
      return NULL;
    }
    union ConstVariant const_data = {.int_val = 1};
    struct LitExpr one = {INT_CONST, const_data};
    struct Expr* lit_expr = malloc(sizeof(struct Expr));
    lit_expr->type = LIT;
    lit_expr->expr.lit_expr = one;

    struct BinaryExpr add_one = {PLUS_EQ_OP, inner, lit_expr};

    struct Expr* result = malloc(sizeof(struct Expr));
    result->type = BINARY;
    result->expr.bin_expr = add_one;
    return result;
  } else if (consume(DEC_TOK)){
    struct Expr* inner = parse_paren_var();
    if (inner == NULL) {
      current--;
      return NULL;
    }
    union ConstVariant const_data = {.int_val = 1};
    struct LitExpr one = {INT_CONST, const_data};
    struct Expr* lit_expr = malloc(sizeof(struct Expr));
    lit_expr->type = LIT;
    lit_expr->expr.lit_expr = one;

    struct BinaryExpr sub_one = {MINUS_EQ_OP, inner, lit_expr};

    struct Expr* result = malloc(sizeof(struct Expr));
    result->type = BINARY;
    result->expr.bin_expr = sub_one;
    return result;
  } else {
    return NULL;
  }
}

struct Expr* parse_var(){
  union TokenVariant* data;
  if ((data = consume_with_data(IDENT))){
    struct VarExpr var_expr = { data->ident_name };

    struct Expr* expr = malloc(sizeof(struct Expr));
    expr->expr.var_expr = var_expr;
    expr->type = VAR;
    return expr;
  } else return NULL;
}

enum TypeSpecifier parse_type_spec(){
  if (consume(INT_TOK)) return INT_SPEC;
  if (consume(SIGNED_TOK)) return SINT_SPEC;
  if (consume(UNSIGNED_TOK)) return UINT_SPEC;
  if (consume(LONG_TOK)) return LONG_SPEC;
  else return 0;
}

struct TypeSpecList* parse_type_specs(){
  enum TypeSpecifier spec = parse_type_spec();
  if (spec == 0) return NULL;
  struct TypeSpecList* specs = malloc(sizeof(struct TypeSpecList));
  specs->spec = spec;
  specs->next = parse_type_specs();
  return specs;
}

bool spec_list_contains(struct TypeSpecList* types, enum TypeSpecifier spec){
  if (types->spec == spec) return true;
  else if (types->next == NULL) return false;
  else return spec_list_contains(types->next, spec);
}

bool spec_list_has_duplicates(struct TypeSpecList* types){
  unsigned num_ints = 0;
  unsigned num_uints = 0;
  unsigned num_sints = 0;
  unsigned num_longs = 0;
  struct TypeSpecList* cur = types;
  while (cur != NULL){
    switch (cur->spec){
      case INT_SPEC:
        num_ints++;
        break;
      case UINT_SPEC:
        num_uints++;
        break;
      case SINT_SPEC:
        num_sints++;
        break;
      case LONG_SPEC:
        num_longs++;
        break;
    }
    cur = cur->next;
  }
  if (num_ints > 1) return true;
  if (num_uints > 1) return true;
  if (num_sints > 1) return true;
  if (num_longs > 1) return true;
  return false;
}

struct Type* parse_param_type(){
  struct TypeSpecList* types = parse_type_specs();
  if (types == NULL) return NULL;
  else if (spec_list_has_duplicates(types)) {
    printf("Parse Error: Duplicate type specifiers");
    destroy_type_spec_list(types);
    return NULL;
  } else if (spec_list_contains(types, SINT_SPEC) &&
             spec_list_contains(types, UINT_SPEC)){
    printf("Parse Error: Invalid type specifiers");
    destroy_type_spec_list(types);
    return NULL;
  } else if (spec_list_contains(types, UINT_SPEC)){
    // ignoring long types for now
    destroy_type_spec_list(types);
    struct Type* type = malloc(sizeof(struct Type));
    type->type = UINT_TYPE;
    return type;
  } else {
    // ignoring long types for now
    destroy_type_spec_list(types);
    struct Type* type = malloc(sizeof(struct Type));
    type->type = INT_TYPE;
    return type;
  }
}

struct AbstractDeclarator* parse_direct_abstract_declarator(){
  struct Token* old_current = current;
  if (!consume(OPEN_P)) return NULL;
  struct AbstractDeclarator* declarator = parse_abstract_declarator();
  if (declarator == NULL || !consume(CLOSE_P)){
    current = old_current;
    return NULL;
  }
  return declarator;
}

struct AbstractDeclarator* parse_abstract_declarator(){
  struct Token* old_current = current;
  if (consume(ASTERISK)){
    struct AbstractDeclarator* declarator = parse_abstract_declarator();
    if (declarator == NULL){
      current = old_current;
      return NULL;
    }
    struct AbstractDeclarator* result = malloc(sizeof(struct AbstractDeclarator));
    result->type = ABSTRACT_POINTER;
    result->data = declarator;
    return result;
  }
  struct AbstractDeclarator* declarator = parse_direct_abstract_declarator();
  if (declarator != NULL){
    return declarator;
  } else {
    struct AbstractDeclarator* declarator = malloc(sizeof(struct AbstractDeclarator));
    declarator->type = ABSTRACT_BASE;
    return declarator;
  }
}

struct Type* process_abstract_declarator(
    struct AbstractDeclarator* declarator, 
    struct Type* base_type){
  struct Type* result;
  switch (declarator->type){
    case ABSTRACT_BASE:
      result = base_type;
      break;
    case ABSTRACT_POINTER:
      struct Type* ptr_type = malloc(sizeof(struct Type));
      ptr_type->type = POINTER_TYPE;
      ptr_type->type_data.pointer_type.referenced_type = base_type;
      result = process_abstract_declarator(declarator->data, ptr_type);
      break;
  }
  return result;
}

struct Expr* parse_cast(){
  struct Token* old_current = current;
  if (!consume(OPEN_P)) return NULL;
  struct Type* base_type = parse_param_type();
  if (base_type == NULL){
    current = old_current;
    return NULL;
  }
  struct AbstractDeclarator* declarator = parse_abstract_declarator();
  if (declarator == NULL || !consume(CLOSE_P)){
    current = old_current;
    return NULL;
  }
  struct Expr* expr = parse_expr();
  if (expr == NULL){
    current = old_current;
    return NULL;
  }
  struct Type* derived_type = process_abstract_declarator(declarator, base_type);
  destroy_abstract_declarator(declarator);

  struct CastExpr cast = {derived_type, expr};
  struct Expr* result = malloc(sizeof(struct Expr));
  result->type = CAST;
  result->expr.cast_expr = cast;
  return result;
}

struct Expr* parse_post_op(){
  struct Token* old_current = current;
  struct Expr* inner;
  if ((inner = parse_paren_var())){
    if (consume(INC_TOK)){
      struct PostAssignExpr add_one = {POST_INC, inner};
      struct Expr* result = malloc(sizeof(struct Expr));
      result->type = POST_ASSIGN;
      result->expr.post_assign_expr = add_one;
      return result;
    } else if (consume(DEC_TOK)){
      struct PostAssignExpr sub_one = {POST_DEC, inner};
      struct Expr* result = malloc(sizeof(struct Expr));
      result->type = POST_ASSIGN;
      result->expr.post_assign_expr = sub_one;
      return result;
    } else {
      current = old_current;
      destroy_expr(inner);
      return NULL;
    }
  } else return NULL;
}

struct Expr* parse_parens(){
  if (consume(OPEN_P)){
    struct Token* old_current = current - 1;
    struct Expr* inner = parse_expr();
    if (inner == NULL) {
      current = old_current;
      return NULL;
    } else if (!consume(CLOSE_P)){
      current = old_current;
      destroy_expr(inner);
      return NULL;
    }
    return inner;
  } else {
    return NULL;
  }
}

struct ArgList* parse_args(){
  struct Expr* arg;
  struct Token* old_current = current;
  if ((arg = parse_expr())){
    struct ArgList* args = malloc(sizeof(struct ArgList));
    args->arg = *arg;
    free(arg); // DONT RECURSIVELY DESTROY!!
    if (consume(COMMA)) args->next = parse_args();
    else if (consume(CLOSE_P)) args->next = NULL;
    else {
      current = old_current;
      free(args);
      return NULL;
    }
    return args;
  } else return NULL;
}

struct Expr* parse_func_call(){
  union TokenVariant* data;
  struct Token* old_current = current;
  if ((data = consume_with_data(IDENT))){
    if (!consume(OPEN_P)){
      current = old_current;
      return NULL;
    }
    struct ArgList* args;
    if (consume(CLOSE_P)) args = NULL;
    else {
      args = parse_args();
      if (args == NULL){
        current = old_current;
        return NULL;
      }
    }
    struct FunctionCallExpr call = {data->ident_name, args};

    struct Expr* expr = malloc(sizeof(struct Expr));
    expr->type = FUNCTION_CALL;
    expr->expr.fun_call_expr = call;
    return expr;
  } else return NULL;
}

struct Expr* parse_unary(){
  enum UnOp op;
  if ((op = consume_unary_op())){
    struct Token* old_current = current;
    struct Expr* inner = parse_factor();
    if (inner == NULL) {
      current = old_current - 1;
      return NULL;
    }
    struct UnaryExpr expr = {op, inner};
    struct Expr* result = malloc(sizeof(struct Expr));
    result->expr.un_expr = expr;
    result->type = UNARY;
    return result;
  }

  struct Expr* expr;
  if ((expr = parse_pre_op())){
    return expr;
  } else if (consume(ASTERISK)){ 
    struct Token* old_current = current - 1;
    struct Expr* inner = parse_expr();
    if (inner == NULL) {
      current = old_current;
      return NULL;
    }
    struct DereferenceExpr expr = {inner};
    struct Expr* result = malloc(sizeof(struct Expr));
    result->expr.deref_expr = expr;
    result->type = DEREFERENCE;
    return result;
  } else if (consume(AMPERSAND)){
    struct Token* old_current = current - 1;
    struct Expr* inner = parse_expr();
    if (inner == NULL) {
      current = old_current;
      return NULL;
    }
    struct AddrOfExpr expr = {inner};
    struct Expr* result = malloc(sizeof(struct Expr));
    result->expr.addr_of_expr = expr;
    result->type = ADDR_OF;
    return result;
  }
  return NULL;
}

struct Expr* parse_factor(){
  union TokenVariant* data;
  if ((data = consume_with_data(INT_LIT))){
    union ConstVariant const_data = {.int_val = data->int_val};
    struct LitExpr lit_expr = {INT_CONST, const_data};

    struct Expr* expr = malloc(sizeof(struct Expr));
    expr->expr.lit_expr = lit_expr;
    expr->type = LIT;
    return expr;
  } else if ((data = consume_with_data(U_INT_LIT))){
    union ConstVariant const_data = {.uint_val = data->uint_val};
    struct LitExpr lit_expr = {UINT_CONST, const_data};

    struct Expr* expr = malloc(sizeof(struct Expr));
    expr->expr.lit_expr = lit_expr;
    expr->type = LIT;
    return expr;
  } else if ((data = consume_with_data(LONG_LIT))){
    union ConstVariant const_data  = {.long_val = data->long_val};
    struct LitExpr lit_expr = {LONG_CONST, const_data};

    struct Expr* expr = malloc(sizeof(struct Expr));
    expr->expr.lit_expr = lit_expr;
    expr->type = LIT;
    return expr;
  } else if ((data = consume_with_data(U_LONG_LIT))){
    union ConstVariant const_data = {.ulong_val = data->ulong_val};
    struct LitExpr lit_expr = {U_LONG_LIT, const_data};

    struct Expr* expr = malloc(sizeof(struct Expr));
    expr->expr.lit_expr = lit_expr;
    expr->type = LIT;
    return expr;
  }
  
  struct Expr* expr;
  if ((expr = parse_cast())) return expr;
  else if ((expr = parse_unary())) return expr;
  else if ((expr = parse_post_op())) return expr;
  else if ((expr = parse_parens())) return expr;
  else if ((expr = parse_func_call())) return expr;
  else if ((expr = parse_var())) return expr;
  else return NULL;
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
  return 0;
}

// pratt parsing W
struct Expr* parse_bin_expr(unsigned min_prec){
  struct Expr* lhs = parse_factor();

  if (lhs == NULL) return NULL;

  while (current - program < prog_size){ 
    enum BinOp op = consume_binary_op();

    if (op == 0) {
      return lhs; 
    } 

    unsigned prec = get_prec(op);

    if (prec < min_prec) {
      current--;
      break; // stop expansion if a lower precedence operator is encountered
    }

    if (op == TERNARY_OP){
      struct Token* old_current = current;
      struct Expr* middle = parse_expr();
      if (middle == NULL) {
        current = old_current;
        destroy_expr(lhs);
        return NULL;
      }
      if (!consume(COLON)){
        destroy_expr(lhs);
        return NULL;
      }
      struct Expr* rhs = parse_bin_expr(prec);
      if (rhs == NULL) {
        current = old_current;
        destroy_expr(lhs);
        destroy_expr(middle);
        return NULL;
      }
      struct ConditionalExpr conditional_expr = {lhs, middle, rhs};
      lhs = malloc(sizeof(struct Expr));
      lhs->type = CONDITIONAL;
      lhs->expr.conditional_expr = conditional_expr;
      continue;
    }

    // assignment/compound ops are right-associative, everything else is left-associative
    unsigned next_prec = (ASSIGN_OP <= op && op <= SHR_EQ_OP) ? prec : prec + 1;
    struct Expr* rhs = parse_bin_expr(next_prec);

    if (rhs == NULL){
      current--;
      destroy_expr(lhs);
      return NULL;
    }

    if (op == ASSIGN_OP){
      struct AssignExpr assign_expr = {lhs, rhs};
      lhs = malloc(sizeof(struct Expr));
      lhs->type = ASSIGN;
      lhs->expr.assign_expr = assign_expr;
    } else {
      struct BinaryExpr bin_expr = {op, lhs, rhs};
      lhs = malloc(sizeof(struct Expr));
      lhs->type = BINARY;
      lhs->expr.bin_expr = bin_expr;
    }
  }

  return lhs;
}

struct Expr* parse_expr(){
  return parse_bin_expr(0);
}

/*
parseStmt :: Parser Token Stmt
parseStmt =
  liftA2 RetStmt (char ReturnTok *> parseExpr <* char Semi) (pure Nothing) <|>
  ExprStmt <$> (parseExpr <* char Semi) <|>
  liftA3 IfStmt
    (char IfTok *> char OpenP *> parseExpr <* char CloseP)
    parseStmt (optional (char ElseTok *> parseStmt)) <|>
  liftA2 LabeledStmt (identName <$> satisfy isIdent <* char Colon) parseStmt <|>
  GoToStmt . identName <$> (char GoToTok *> satisfy isIdent <* char Semi) <|>
  CompoundStmt <$> parseBlock <|>
  BreakStmt Nothing <$ (char BreakTok <* char Semi) <|>
  ContinueStmt Nothing <$ (char ContinueTok <* char Semi) <|>
  char WhileTok *> liftA3 WhileStmt (char OpenP *> parseExpr <* char CloseP)
    parseStmt (pure Nothing) <|>
  char DoTok *> liftA3 DoWhileStmt parseStmt
    (char WhileTok *> char OpenP *> parseExpr <* char CloseP <* char Semi) (pure Nothing) <|>
  char ForTok *> lift5 ForStmt
    (char OpenP *> parseForInit)
    (optional parseExpr <* char Semi)
    (optional parseExpr <* char CloseP)
    parseStmt (pure Nothing) <|>
  char SwitchTok *> lift4 SwitchStmt (char OpenP *> parseExpr <* char CloseP)
    parseStmt (pure Nothing) (pure Nothing) <|>
  char CaseTok *> liftA3 CaseStmt (parseExpr <* char Colon) parseStmt (pure Nothing) <|>
  char DefaultTok *> char Colon *> liftA2 DefaultStmt parseStmt (pure Nothing) <|>
  NullStmt <$ char Semi
*/

struct Statement* parse_return_stmt(){
  struct Token* old_current = current;
  if (!consume(RETURN_TOK)) return NULL;
  struct Expr* expr = parse_expr();
  if (expr == NULL){
    current = old_current;
    return NULL;
  }
  if (!consume(SEMI)){
    destroy_expr(expr);
    current = old_current;
    return NULL;
  }
  struct ReturnStmt ret_stmt = {expr, NULL};
  struct Statement* result = malloc(sizeof(struct Statement));
  result->type = RETURN_STMT;
  result->statement.ret_stmt = ret_stmt;
  return result;
}

struct Statement* parse_expr_stmt(){
  struct Token* old_current = current;
  struct Expr* expr = parse_expr();
  if (expr == NULL) return NULL;
  if (!consume(SEMI)){
    destroy_expr(expr);
    current = old_current;
    return NULL;
  }
  struct ExprStmt expr_stmt = {expr};
  struct Statement* result = malloc(sizeof(struct Statement));
  result->type = EXPR_STMT;
  result->statement.expr_stmt = expr_stmt;
  return result;
}

struct Statement* parse_if_stmt(){
  return NULL;
}

struct Statement* parse_labeled_stmt(){
  return NULL;
}

struct Statement* parse_goto_stmt(){
  if (!consume(GOTO_TOK)) return NULL;
  struct Token* old_current = current;
  union TokenVariant* data;
  if ((data = consume_with_data(IDENT)) && consume(SEMI)){
    struct GotoStmt goto_stmt = { data->ident_name };

    struct Statement* result = malloc(sizeof(struct Statement));
    result->type = GOTO_STMT;
    result->statement.goto_stmt = goto_stmt;
    return result;
  } else {
    current = old_current;
    return NULL; 
  }
}

struct Statement* parse_compound_stmt(){
  return NULL;
}

struct Statement* parse_break_stmt(){
  struct Token* old_current = current;
  if (consume(BREAK_TOK) && consume(SEMI)){
    struct BreakStmt break_stmt = {NULL};
    struct Statement* result = malloc(sizeof(struct Statement));
    result->type = BREAK_STMT;
    result->statement.break_stmt = break_stmt;
    return result;
  } else {
    current = old_current;
    return NULL;
  }
}

struct Statement* parse_continue_stmt(){
  struct Token* old_current = current;
  if (consume(CONTINUE_TOK) && consume(SEMI)){
    struct ContinueStmt continue_stmt = {NULL};
    struct Statement* result = malloc(sizeof(struct Statement));
    result->type = CONTINUE_STMT;
    result->statement.continue_stmt = continue_stmt;
    return result;
  } else {
    current = old_current;
    return NULL;
  }
}

struct Statement* parse_while_stmt(){
  return NULL;
}

struct Statement* parse_do_while_stmt(){
  return NULL;
}

struct Statement* parse_for_stmt(){
  return NULL;
}

struct Statement* parse_switch_stmt(){
  return NULL;
}

struct Statement* parse_case_stmt(){
  struct Token* old_current = current;
  if (!consume(CASE_TOK)) return NULL;
  struct Expr* expr = parse_expr();
  if (expr == NULL){
    current = old_current;
    return NULL;
  }
  if (!consume(COLON)){
    destroy_expr(expr);
    current = old_current;
    return NULL;
  }
  struct Statement* stmt = parse_statement();
  if (stmt == NULL){
    current = old_current;
    return NULL;
  }
  struct CaseStmt case_stmt = {expr, stmt, NULL};
  struct Statement* result = malloc(sizeof(struct Statement));
  result->type = CASE_STMT;
  result->statement.case_stmt = case_stmt;
  return result;
}

struct Statement* parse_default_stmt(){
  struct Token* old_current = current;
  if (!consume(DEFAULT_TOK)) return NULL;
  if (!consume(COLON)){
    current = old_current;
    return NULL;
  }
  struct Statement* stmt = parse_statement();
  if (stmt == NULL){
    current = old_current;
    return NULL;
  }
  struct DefaultStmt default_stmt = {stmt, NULL};
  struct Statement* result = malloc(sizeof(struct Statement));
  result->type = DEFAULT_STMT;
  result->statement.default_stmt = default_stmt;
  return result;
}

struct Statement* parse_null_stmt(){
  if (consume(SEMI)){
    struct NullStmt null_stmt;
    struct Statement* result = malloc(sizeof(struct Statement));
    result->type = NULL_STMT;
    result->statement.null_stmt = null_stmt;
    return result;
  } else return NULL;
}

struct Statement* parse_statement(){
  struct Statement* stmt;
  if ((stmt = parse_return_stmt())) return stmt;
  else if ((stmt = parse_expr_stmt())) return stmt;
  else if ((stmt = parse_if_stmt())) return stmt;
  else if ((stmt = parse_labeled_stmt())) return stmt;
  else if ((stmt = parse_goto_stmt())) return stmt;
  else if ((stmt = parse_compound_stmt())) return stmt;
  else if ((stmt = parse_break_stmt())) return stmt;
  else if ((stmt = parse_continue_stmt())) return stmt;
  else if ((stmt = parse_while_stmt())) return stmt;
  else if ((stmt = parse_do_while_stmt())) return stmt;
  else if ((stmt = parse_for_stmt())) return stmt;
  else if ((stmt = parse_switch_stmt())) return stmt;
  else if ((stmt = parse_case_stmt())) return stmt;
  else if ((stmt = parse_default_stmt())) return stmt;
  else if ((stmt = parse_null_stmt())) return stmt;
  else return NULL;
}

struct Statement* parse_test(struct TokenArray* arr){
  program = arr->tokens;
  current = program;
  prog_size = arr->size;
  struct Statement* result = parse_statement();
  if (result == NULL) {
    print_error();
    return NULL;
  }
  if (current - program != prog_size) {
    print_error();
    destroy_stmt(result);
    return NULL;
  }
  return result;
}

struct Declaration* parse_dclr(){
  return NULL;
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