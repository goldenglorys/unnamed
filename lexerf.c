#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Enum to represent different types of tokens
typedef enum
{
    INT,
    KEYWORD,
    SEPARATOR,
    END_OF_TOKENS,
} TokenType;

// Struct to represent a token
typedef struct
{
    TokenType type;
    char *value;
} Token;

/**
 * Prints the value and type of a token.
 * @param token The token to print.
 */
void print_token(Token token)
{
    printf("TOKEN VALUE: ");
    for (int i = 0; token.value[i] != '\0'; i++)
    {
        printf("%c", token.value[i]);
    }
    if (token.type == INT)
    {
        printf(" TOKEN TYPE: INT\n");
    }
    if (token.type == KEYWORD)
    {
        printf(" TOKEN TYPE: KEYWORD\n");
    }
    if (token.type == SEPARATOR)
    {
        printf(" TOKEN TYPE: SEPARATOR\n");
    }
}

/**
 * Generates a number token from the current input string.
 * @param current The input string.
 * @param current_index Pointer to the current index in the input string.
 * @return A pointer to the generated number token.
 */
Token *generate_number(char *current, int *current_index)
{
    Token *token = malloc(sizeof(Token));
    token->type = INT;
    char *value = malloc(sizeof(char) * (strlen(current) + 1));
    int value_index = 0;
    while (isdigit(current[*current_index]) && current[*current_index] != '\0')
    {
        value[value_index] = current[*current_index];
        value_index++;
        (*current_index)++;
    }
    value[value_index] = '\0';
    printf("is digit: %d | is end of line: %d\n", isdigit(current[*current_index]), current[*current_index] == '\0');
    printf("VALUE INDEX: %d\n", value_index);
    token->value = value;
    return token;
}

/**
 * Generates a keyword token from the current input string.
 * @param current The input string.
 * @param current_index Pointer to the current index in the input string.
 * @return A pointer to the generated keyword token.
 */

Token *generate_keyword(char *current, int *current_index)
{
    Token *token = malloc(sizeof(Token));
    char *keyword = malloc(sizeof(char) * (strlen(current) + 1));
    int keyword_index = 0;
    while (isalpha(current[*current_index]) && current[*current_index] != '\0')
    {
        keyword[keyword_index] = current[*current_index];
        keyword_index++;
        (*current_index)++;
    }
    keyword[keyword_index] = '\0';
    if (strcmp(keyword, "exit") == 0)
    {
        token->type = KEYWORD;
        token->value = strdup("EXIT");
    }
    else
    {
        token->type = KEYWORD;
        token->value = keyword;
    }
    return token;
}

size_t tokens_index;

/**
 * Main lexer function that reads a file and tokenizes its content.
 * @param file The input file.
 * @return A pointer to the array of generated tokens.
 */
Token *lexer(FILE *file)
{
    int length;
    char *buffer = 0;
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);
    buffer = malloc(sizeof(char) * (length + 1));
    fread(buffer, 1, length, file);
    fclose(file);
    buffer[length] = '\0';
    char *current = buffer;
    int current_index = 0;

    Token *tokens = malloc(sizeof(Token) * 1024);
    int tokens_index = 0;

    while (current_index < length && current[current_index] != '\0')
    {
        Token *token = NULL;
        printf("current index %d, char '%c'\n", current_index, current[current_index]);

        if (current[current_index] == ';' || current[current_index] == '(' || current[current_index] == ')')
        {
            token = malloc(sizeof(Token));
            token->value = malloc(sizeof(char) * 2);
            token->value[0] = current[current_index];
            token->value[1] = '\0';
            token->type = SEPARATOR;
            current_index++;
        }
        else if (isdigit(current[current_index]))
        {
            token = generate_number(current, &current_index);
            printf("TOKEN VALUE: %s\n", token->value);
        }
        else if (isalpha(current[current_index]))
        {
            token = generate_keyword(current, &current_index);
            printf("TOKEN VALUE: %s\n", token->value);
        }
        else
        {
            current_index++;
            continue;
        }

        if (token != NULL)
        {
            tokens[tokens_index] = *token;
            tokens_index++;
            free(token);
        }
    }

    tokens[tokens_index].value = strdup("\0");
    tokens[tokens_index].type = END_OF_TOKENS;

    free(buffer);
    return tokens;
}
