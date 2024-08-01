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
    printf("'");
    for (int i = 0; token.value[i] != '\0'; i++)
    {
        printf("%c", token.value[i]);
    }
    printf("'");
    switch (token.type)
    {
    case INT:
        printf(" TOKEN TYPE: INT\n");
        break;
    case KEYWORD:
        printf(" TOKEN TYPE: KEYWORD\n");
        break;
    case SEPARATOR:
        printf(" TOKEN TYPE: SEPARATOR\n");
        break;
    case END_OF_TOKENS:
        printf(" END OF TOKENS\n");
        break;
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
        if (!isdigit(current[*current_index]))
        {
            break;
        }
        value[value_index] = current[*current_index];
        value_index++;
        (*current_index)++;
    }
    value[value_index] = '\0';
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
    return token;
}

/**
 * Generates a separator token from the current input string.
 * @param current The input string.
 * @param current_index Pointer to the current index in the input string.
 * @return A pointer to the generated separator token.
 */
Token *generate_separator(char *current, int *current_index)
{
    Token *token = malloc(sizeof(Token));
    token->value = malloc(sizeof(char) * 2);
    token->value[0] = current[*current_index];
    token->value[1] = '\0';
    token->type = SEPARATOR;
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
    char *current = 0;
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);
    current = malloc(sizeof(char) * (length + 1));
    fread(current, 1, length, file);
    fclose(file);
    current[length] = '\0';
    int current_index = 0;

    Token *tokens = malloc(sizeof(Token) * 1024);
    int tokens_index = 0;

    while (current_index < length && current[current_index] != '\0')
    {
        Token *token = malloc(sizeof(Token));
        if (current[current_index] == ';' || current[current_index] == '(' || current[current_index] == ')')
        {
            token = generate_separator(current, &current_index);
            tokens[tokens_index] = *token;
            tokens_index++;
        }
        else if (isdigit(current[current_index]))
        {
            token = generate_number(current, &current_index);
            printf("TOKEN VALUE: %s\n", token->value);
            tokens[tokens_index] = *token;
            tokens_index++;
            current_index--;
        }
        else if (isalpha(current[current_index]))
        {
            token = generate_keyword(current, &current_index);
            printf("TOKEN KEYWORD: %s\n", token->value);
            tokens[tokens_index] = *token;
            tokens_index++;
            current_index--;
        }
        free(token);
        current_index++;
    }

    tokens[tokens_index].value = strdup("\0");
    tokens[tokens_index].type = END_OF_TOKENS;

    return tokens;
}
