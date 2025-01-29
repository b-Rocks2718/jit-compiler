#pragma once

#include <stddef.h>

struct Program {
  struct DeclarationList* dclrs;
};

enum StorageClass {
  NONE,
  STATIC,
  EXTERN
};

enum TypeType {
  INT_TYPE,
  LONG_TYPE,
  UINT_TYPE,
  ULONG_TYPE,
  FUN_TYPE,
  POINTER_TYPE
};

struct FunType {
  size_t num_params;
  struct Type* param_types;
  struct Type* return_type;
};

struct PointerType {
  struct Type* referenced_type;
};

union TypeVariant {
  struct FunType fun_type;
  struct PointerType pointer_type;
  // no data for other types
};

struct Type {
  enum TypeType type;
  union TypeVariant type_data;
};


enum DclrType {
  VAR_DCLR,
  FUN_DCLR
};

struct VariableDclr {
  struct Slice* name;
  struct Expr* init;
};

struct FunctionDclr {
  struct Slice* name;
  enum StorageClass storage;
  struct ParamList* params;
  struct Block* body;
};

union DeclareVariant {
  struct VariableDclr var_dclr;
  struct FunctionDclr fun_dclr;
};

struct ParamList {
  struct VariableDclr param;
  struct ParamList* next;
};

struct Declaration {
  union DeclareVariant dclr;
  enum DclrType type;
};

struct DeclarationList {
  struct Declaration dclr;
  struct DeclarationList* next;
};

enum ExprType {
  BINARY,
  ASSIGN,
  POST_ASSIGN,
  CONDITIONAL,
  LIT,
  UNARY,
  VAR,
  FUNCTION_CALL,
  CAST,
  ADDR_OF,
  DEREFERENCE
};

enum BinOp {
  ADD_OP,
  SUB_OP,
  MUL_OP,
  DIV_OP,
  MOD_OP,
  BIT_AND,
  BIT_OR,
  BIT_XOR,
  BIT_SHR,
  BIT_SHL,
  BOOL_AND,
  BOOL_OR,
  BOOL_EQ,
  BOOL_NEQ,
  BOOL_LE,
  BOOL_GE,
  BOOL_LEQ,
  BOOL_GEQ,
  ASSIGN_OP,
  PLUS_EQ_OP,
  MINUS_EQ_OP,
  TIMES_EQ_OP,
  DIV_EQ_OP,
  MOD_EQ_OP,
  AND_EQ_OP,
  OR_EQ_OP,
  XOR_EQ_OP,
  SHL_EQ_OP,
  SHR_EQ_OP,
  TERNARY_OP
};

enum UnOp {
  COMPLEMENT,
  NEGATE,
  BOOL_NOT
};

struct BinaryExpr {
  enum BinOp op;
  struct Expr* left;
  struct Expr* right;
};

struct AssignExpr {
  struct Expr* left;
  struct Expr* right;
};

enum PostOp {
  POST_INC,
  POST_DEC
};

struct PostAssignExpr {
  enum PostOp op;
  struct Expr* expr;
};

struct ConditionalExpr {
  struct Expr* condition;
  struct Expr* left;
  struct Expr* right;
};

enum ConstType {
  INT_CONST,
  UINT_CONST,
  LONG_CONST,
  ULONG_CONST
};

union ConstVariant {
  int int_val;
  unsigned uint_val;
  long long_val;
  unsigned long ulong_val;
};

struct LitExpr {
  enum ConstType type;
  union ConstVariant value;
};

struct UnaryExpr {
  enum UnOp op;
  struct Expr* expr;
};

struct VarExpr {
  struct Slice* name;
};

struct FunctionCallExpr {
  struct Slice* func_name;
  struct ArgList* args;
};

struct CastExpr {
  struct Type* target;
  struct Expr* expr;
};

struct AddrOfExpr {
  struct Expr* expr;
};

struct DereferenceExpr {
  struct Expr* expr;
};

union ExprVariant {
  struct BinaryExpr bin_expr;
  struct AssignExpr assign_expr;
  struct PostAssignExpr post_assign_expr;
  struct ConditionalExpr conditional_expr;
  struct LitExpr lit_expr;
  struct UnaryExpr un_expr;
  struct VarExpr var_expr;
  struct FunctionCallExpr fun_call_expr;
  struct CastExpr cast_expr;
  struct AddrOfExpr addr_of_expr;
  struct DereferenceExpr deref_expr;
};

struct Expr {
  enum ExprType type;
  union ExprVariant expr;
};

struct ArgList {
  struct Expr arg;
  struct ArgList* next;
};

enum StatementType {
  RETURN_STMT,
  EXPR_STMT,
  IF_STMT,
  GOTO_STMT,
  LABELED_STMT,
  COMPOUND_STMT,
  BREAK_STMT,
  CONTINUE_STMT,
  WHILE_STMT,
  DO_WHILE_STMT,
  FOR_STMT,
  SWITCH_STMT,
  CASE_STMT,
  DEFAULT_STMT,
  NULL_STMT
};

struct ReturnStmt {
  struct Expr* expr;
  struct Slice* func;
};

struct ExprStmt {
  struct Expr* expr;
};

struct IfStmt {
  struct Expr* condition;
  struct Statement* if_stmt;
  struct Statement* else_stmt;
};

struct GotoStmt {
  struct Slice* label;
};

struct LabeledStmt {
  struct Slice* label;
  struct Statement* stmt;
};

struct CompoundStmt {
  struct Block* block;
};

struct BreakStmt {
  struct Slice* label;
};

struct ContinueStmt {
  struct Slice* label;
};

struct WhileStmt {
  struct Expr* condition;
  struct Statement* statement;
  struct Slice* label;
};

struct DoWhileStmt {
  struct Statement* statement;
  struct Expr* condition;
  struct Slice* label;
};

struct ForStmt {
  struct ForInit* init;
  struct Expr* condition;
  struct Expr* end;
  struct Statement* statement;
  struct Slice* label;
};

enum ForInitType {
  DCLR_INIT,
  EXPR_INIT,
};  

struct ForInit {
  enum ForInitType type;
  union ForInitVariant* init;
};

union ForInitVariant {
  struct Declaration dclr_init;
  struct Expr expr_init;
};

struct SwitchStmt {
  struct Expr* condition;
  struct Statement* Statement;
  struct Slice* label;
  struct CaseList* cases;
};

struct CaseStmt {
  struct Expr* expr;
  struct Statement* statement;
  struct Slice* label;
};

struct DefaultStmt {
  struct Statement* statement;
  struct Slice* label;
};

struct NullStmt {};

union StatementVariant {
  struct ReturnStmt ret_stmt;
  struct ExprStmt expr_stmt;
  struct IfStmt if_stmt;
  struct GotoStmt goto_stmt;
  struct LabeledStmt labeled_stmt;
  struct CompoundStmt compound_stmt;
  struct BreakStmt break_stmt;
  struct ContinueStmt continue_stmt;
  struct WhileStmt while_stmt;
  struct DoWhileStmt do_while_stmt;
  struct ForStmt for_stmt;
  struct SwitchStmt switch_stmt;
  struct CaseStmt case_stmt;
  struct DefaultStmt default_stmt;
  struct NullStmt null_stmt;
};

struct Statement {
  union StatementVariant statement;
  enum StatementType type;
};

enum BlockItemType {
  DCLR_ITEM,
  STMT_ITEM
};

union BlockItemVariant {
  struct Statement stmt;
  struct Declaration dclr;
};

struct BlockItem {
  union BlockItemVariant item;
  enum BlockItemType type;
};

struct Block {
  struct BlockItem item;
  struct Block* next;
};

enum CaseLabelType {
  INT_CASE,
  DEFAULT_CASE
};

struct CaseLabel {
  enum CaseLabelType type;
  int* data;
};

struct CaseList {
  struct CaseLabel case_label;
  struct CaseList* next;
};

enum DeclaratorType {
  IDENT_DEC,
  POINTER_DEC,
  FUN_DEC
};

struct IdentDec {
  struct Slice* name;
};

struct PointerDec {
  struct Declarator* decl;
};

struct FunDec {
  size_t num_params;
  struct ParamInfo* params;
  struct Declarator* decl;
};

union DeclaratorVariant {
  struct IdentDec ident_dec;
  struct PointerDec pointer_dec;
  struct FunDec fun_dec;
};

struct Declarator {
  enum DeclaratorType type;
  union DeclaratorVariant declarator;
};

struct ParamInfo {
  struct type* type;
  struct Declarator decl;
};

enum AbstractDeclaratorType {
  ABSTRACT_POINTER,
  ABSTRACT_BASE
};

struct AbstractDeclarator {
  enum AbstractDeclaratorType type;
  struct AbstractDeclarator* data;
};

enum TypeSpecifier {
  INT_SPEC,
  UINT_SPEC,
  SINT_SPEC,
  LONG_SPEC
};