#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "lexerf.h"

/**
 * Main function that opens a file and calls the lexer function.
 */
int main(void)
{
    FILE *file;
    file = fopen("test.un", "r");

    Token *tokens = lexer(file);

    for (size_t i = 0; tokens[i].type != END_OF_TOKENS; i++)
    {
        print_token(tokens[i]);
    }
    
}
