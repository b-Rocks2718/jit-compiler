#include <stdbool.h>
#include <stdlib.h>

#include "AST.h"
#include "parser.h"
#include "token.h"
#include "token_array.h"

static struct Token const * program;
static struct Token const * current;

bool consume(const enum TokenType expected) {
  struct Token const found = *current;
  if (expected == found.type) {
    current++;
    return true;
  } else {
    return false;
  }
}

struct Declaration* parse_dclr(){

}

struct Program* parse_prog(struct TokenArray* arr){
  // parse declarations and put them in a linked list
  program = arr->tokens;

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