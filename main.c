#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "lexerf.h"

/**
 * Main function that opens a file and calls the lexer function.
 */
int main()
{
    FILE *file;
    file = fopen("test.un", "r");

    Token *tokens = lexer(file);
}