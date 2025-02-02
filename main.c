#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "preprocessor.h"
#include "token_array.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, const char *const *const argv) {

    if (argc != 2) {
        fprintf(stderr,"usage: %s <file name>\n",argv[0]);
        exit(1);
    }

    // open the file
    int fd = open(argv[1],O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    // determine its size (std::filesystem::get_size?)
    struct stat file_stats;
    int rc = fstat(fd,&file_stats);
    if (rc != 0) {
        perror("fstat");
        exit(1);
    }

    // map the file in my address space
    char const* prog = (char const *)mmap(
        0,
        file_stats.st_size,
        PROT_READ,
        MAP_PRIVATE,
        fd,
        0);
    if (prog == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    char* preprocessed = preprocess(prog);

    struct TokenArray* tokens = lex(preprocessed);
    if (tokens == NULL) return 1;

    print_token_array(tokens);

    printf("\n");

    struct Expr* expr = parse_expr_test(tokens);
    destroy_token_array(tokens);
    if (expr == NULL) return 2; 

    print_expr(expr);
    printf("\n");
    destroy_expr(expr);

    free(preprocessed);
    
    return 0;
}