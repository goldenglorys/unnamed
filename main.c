#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Enum to define types of separators
typedef enum
{
    SEMI,        // Semicolon
    OPEN_PAREN,  // Open parenthesis
    CLOSE_PAREN, // Close parenthesis
} TypeSeparator;

// Enum to define types of keywords
typedef enum
{
    EXIT, // Keyword "exit"
} TypeKeyword;

// Enum to define types of literals
typedef enum
{
    INT, // Integer literal
} TypeLiteral;

// Struct to represent a keyword token
typedef struct
{
    TypeKeyword type;
} TokenKeyword;

// Struct to represent a separator token
typedef struct
{
    TypeSeparator type;
} TokenSeparator;

// Struct to represent a literal token
typedef struct
{
    TypeLiteral type;
    char *value; // Value of the literal
} TokenLiteral;

/**
 * Function to generate a number token.
 * @param current Pointer to the current character in the input buffer.
 * @param current_index Pointer to the current index in the input buffer.
 * @return Pointer to the generated TokenLiteral.
 */
TokenLiteral *generate_number(char *current, int *current_index)
{
    TokenLiteral *token = malloc(sizeof(TokenLiteral));
    token->type = INT;
    char *value = malloc(sizeof(char) * 8);
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
    token->value = value;
    return token;
}

/**
 * Function to generate a keyword token.
 * @param current Pointer to the current character in the input buffer.
 * @param current_index Pointer to the current index in the input buffer.
 * @return Pointer to the generated TokenKeyword.
 */
TokenKeyword *generate_keyword(char *current, int *current_index)
{
    TokenKeyword *token = malloc(sizeof(TokenKeyword));
    char *keyword = malloc(sizeof(char) * 8);
    int keyword_index = 0;
    while (isalpha(current[*current_index]) && current[*current_index] != '\0')
    {
        keyword[keyword_index] = current[*current_index];
        printf("keyword %c\n", current[*current_index]);
        keyword_index++;
        (*current_index)++;
    }
    printf("keyword_index: %d\n", keyword_index);
    if (strcmp(keyword, "exit") == 0)
    {
        printf("TYPE EXIT\n");
        token->type = EXIT;
    }
    return token;
}

/**
 * Lexer function to process the input file and identify tokens.
 * @param file Pointer to the input file.
 */
void lexer(FILE *file)
{
    int length;
    char *buffer = 0;
    fseek(file, 0, SEEK_END);
    length = ftell(file);
    fseek(file, 0, SEEK_SET);
    buffer = malloc(sizeof(char) * length);
    fread(buffer, 1, length, file);
    fclose(file);
    buffer[length + 1] = '\0';
    char *current = malloc(sizeof(char) * length + 1);
    current = buffer;
    int current_index = 0;

    while (current[current_index] != '\0')
    {
        if (current[current_index] == ';')
        {
            printf("FOUND SEMICOLON\n");
        }
        else if (current[current_index] == '(')
        {
            printf("FOUND OPEN PAREN\n");
        }
        else if (current[current_index] == ')')
        {
            printf("FOUND CLOSE PAREN\n");
        }
        else if (isdigit(current[current_index]))
        {
            TokenLiteral *test_token = generate_number(current, &current_index);
            printf("TEST TOKEN VALUE: %s\n", test_token->value);
        }
        else if (isalpha(current[current_index]))
        {
            TokenKeyword *test_keyword = generate_keyword(current, &current_index);
        }
        current_index++;
    }
}

/**
 * Main function to open the input file and call the lexer function.
 */
int main()
{
    FILE *file;
    file = fopen("test.un", "r");

    lexer(file);
}