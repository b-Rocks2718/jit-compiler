#include <stdio.h>
#include <stdlib.h>

#include "AST.h"
#include "slice.h"

void print_param_type_list(struct ParamTypeList* type_list){
  if (type_list == NULL) return;
  print_type(&type_list->type);
  printf(", ");
  print_param_type_list(type_list->next);
}

void print_type(struct Type* type){
  switch (type->type){
    case INT_TYPE:
      printf("int");
      break;
    case UINT_TYPE:
      printf("uint");
      break;
    case LONG_TYPE:
      printf("long");
      break;
    case ULONG_TYPE:
      printf("ulong");
      break;
    case POINTER_TYPE:
      print_type(type->type_data.pointer_type.referenced_type);
      printf("*");
      break;
    case FUN_TYPE:
      printf("func(ret_type=");
      print_type(type->type_data.fun_type.return_type);
      printf("param_types=[");
      print_param_type_list(type->type_data.fun_type.param_types);
      printf("]");
      break;
  }
}

void print_expr(struct Expr* expr){
  switch (expr->type){
    case BINARY:
      print_bin_expr(&expr->expr.bin_expr);
      break;
    case UNARY:
      print_un_expr(&expr->expr.un_expr);
      break;
    case ASSIGN:
      print_assign_expr(&expr->expr.assign_expr);
      break;
    case POST_ASSIGN:
      print_post_assign_expr(&expr->expr.post_assign_expr);
      break;
    case CONDITIONAL:
      print_conditional_expr(&expr->expr.conditional_expr);
      break;
    case LIT:
      print_lit_expr(&expr->expr.lit_expr);
      break;
    case VAR:
      print_var_expr(&expr->expr.var_expr);
      break;
    case FUNCTION_CALL:
      print_fun_call_expr(&expr->expr.fun_call_expr);
      break;
    case CAST:
      print_cast_expr(&expr->expr.cast_expr);
      break;
    case ADDR_OF:
      print_addr_of_expr(&expr->expr.addr_of_expr);
      break;
    case DEREFERENCE:
      print_dereference_expr(&expr->expr.deref_expr);
      break;
  }
}

void print_bin_expr(struct BinaryExpr* bin_expr){
  switch (bin_expr->op){
    case ADD_OP:
      printf("AddOp(");
      break;
    case SUB_OP:
      printf("SubOp(");
      break;
    case MUL_OP:
      printf("MulOp(");
      break;
    case DIV_OP:
      printf("DivOp(");
      break;
    case MOD_OP:
      printf("ModOp(");
      break;
    case BIT_AND:
      printf("BitAnd(");
      break;
    case BIT_OR:
      printf("BitOr(");
      break;
    case BIT_XOR:
      printf("BitXor(");
      break;
    case BIT_SHL:
      printf("BitShl(");
      break;
    case BIT_SHR:
      printf("BitShr(");
      break;
    case BOOL_AND:
      printf("BoolAnd(");
      break;
    case BOOL_OR:
      printf("BoolOr(");
      break;
    case BOOL_EQ:
      printf("BoolEq(");
      break;
    case BOOL_NEQ:
      printf("BoolNeq(");
      break;
    case BOOL_LE:
      printf("BoolLe(");
      break;
    case BOOL_GE:
      printf("BoolGe(");
      break;
    case BOOL_LEQ:
      printf("BoolLeq(");
      break;
    case BOOL_GEQ:
      printf("BoolGeq(");
      break;
    case ASSIGN_OP:
      printf("AssignOp(");
      break;
    case PLUS_EQ_OP:
      printf("PlusEqOp(");
      break;
    case MINUS_EQ_OP:
      printf("MinusEqOp(");
      break;
    case MUL_EQ_OP:
      printf("MulEqOp(");
      break;
    case DIV_EQ_OP:
      printf("DivEqOp(");
      break;
    case MOD_EQ_OP:
      printf("ModEqOp(");
      break;
    case AND_EQ_OP:
      printf("AndEqOp(");
      break;
    case OR_EQ_OP:
      printf("OrEqOp(");
      break;
    case XOR_EQ_OP:
      printf("XorEqOp(");
      break;
    case SHL_EQ_OP:
      printf("ShlEqOp(");
      break;
    case SHR_EQ_OP:
      printf("ModOp(");
      break;
    case TERNARY_OP:
      printf("TernaryOp(");
      break;
  }
  print_expr(bin_expr->left);
  printf(", ");
  print_expr(bin_expr->right);
  printf(")");
}

void print_un_expr(struct UnaryExpr* un_expr){
  switch (un_expr->op){
    case COMPLEMENT:
      printf("Complement(");
      break;
    case NEGATE:
      printf("Negate(");
      break;
    case BOOL_NOT:
      printf("BoolNot(");
      break;
  }
  print_expr(un_expr->expr);
  printf(")");
}

void print_assign_expr(struct AssignExpr* assign_expr){
  printf("Assign(");
  print_expr(assign_expr->left);
  printf(", ");
  print_expr(assign_expr->right);
  printf(")");
}

void print_post_assign_expr(struct PostAssignExpr* post_expr){
  printf("PostAssign(");
  switch (post_expr->op){
    case POST_INC:
      printf("Inc");
      break;
    case POST_DEC:
      printf("Dec");
      break;
  }
  printf(", ");
  print_expr(post_expr->expr);
  printf(")");
}

void print_conditional_expr(struct ConditionalExpr* c_expr){
  printf("ConditionalExpr(");
  print_expr(c_expr->condition);
  printf(", ");
  print_expr(c_expr->left);
  printf(", ");
  print_expr(c_expr->right);
  printf(")");
}

void print_lit_expr(struct LitExpr* lit_expr){
  printf("LitExpr(");
  switch (lit_expr->type){
    case INT_CONST:
      printf("int, %d", lit_expr->value.int_val);
      break;
    case UINT_CONST:
      printf("uint, %u", lit_expr->value.uint_val);
      break;
    case LONG_CONST:
      printf("long, %ld", lit_expr->value.long_val);
      break;
    case ULONG_CONST:
      printf("ulong, %lu", lit_expr->value.ulong_val);
      break;
  }
  printf(")");
}

void print_var_expr(struct VarExpr* var_expr){
  printf("Var(");
  print_slice(var_expr->name);
  printf(")");
}

void print_args_list(struct ArgList* args_list){
  if (args_list == NULL) return;
  print_expr(&args_list->arg);
  printf(", ");
  print_args_list(args_list->next);
}

void print_fun_call_expr(struct FunctionCallExpr* fun_expr){
  printf("FunCallExpr(");
  print_slice(fun_expr->func_name);
  printf(", [");
  print_args_list(fun_expr->args);
  printf(")]");
}

void print_cast_expr(struct CastExpr* cast_expr){
  printf("Cast(");
  print_type(cast_expr->target);
  printf(", ");
  print_expr(cast_expr->expr);
  printf(")");
}

void print_addr_of_expr(struct AddrOfExpr* expr){
  printf("AddrOf(");
  print_expr(expr->expr);
  printf(")");
}

void print_dereference_expr(struct DereferenceExpr* expr){
  printf("Dereference(");
  print_expr(expr->expr);
  printf(")");
}

void destroy_expr(struct Expr* expr){
  switch (expr->type){
    case BINARY:
      destroy_bin_expr(&expr->expr.bin_expr);
      break;
    case UNARY:
      destroy_un_expr(&expr->expr.un_expr);
      break;
    case ASSIGN:
      destroy_assign_expr(&expr->expr.assign_expr);
      break;
    case POST_ASSIGN:
      destroy_post_assign_expr(&expr->expr.post_assign_expr);
      break;
    case CONDITIONAL:
      destroy_conditional_expr(&expr->expr.conditional_expr);
      break;
    case LIT:
      destroy_lit_expr(&expr->expr.lit_expr);
      break;
    case VAR:
      destroy_var_expr(&expr->expr.var_expr);
      break;
    case FUNCTION_CALL:
      destroy_fun_call_expr(&expr->expr.fun_call_expr);
      break;
    case CAST:
      destroy_cast_expr(&expr->expr.cast_expr);
      break;
    case ADDR_OF:
      destroy_addr_of_expr(&expr->expr.addr_of_expr);
      break;
    case DEREFERENCE:
      destroy_dereference_expr(&expr->expr.deref_expr);
      break;
  }
  free(expr);
}

void destroy_bin_expr(struct BinaryExpr* expr){
  destroy_expr(expr->left);
  destroy_expr(expr->right);
}

void destroy_un_expr(struct UnaryExpr* expr){
  destroy_expr(expr->expr);
}

void destroy_assign_expr(struct AssignExpr* expr){
  destroy_expr(expr->left);
  destroy_expr(expr->right);
}

void destroy_post_assign_expr(struct PostAssignExpr* expr){
  destroy_expr(expr->expr);
}

void destroy_conditional_expr(struct ConditionalExpr* expr){

}

void destroy_lit_expr(struct LitExpr* expr){

}

void destroy_var_expr(struct VarExpr* expr){

}

void destroy_fun_call_expr(struct FunctionCallExpr* expr){

}

void destroy_cast_expr(struct CastExpr* expr){

}

void destroy_addr_of_expr(struct AddrOfExpr* expr){

}

void destroy_dereference_expr(struct DereferenceExpr* expr){

}

void destroy_type(struct Type* type){

}