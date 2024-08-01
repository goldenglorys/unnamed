#ifndef LEXER_H_
#define LEXER_H_

// Enum to represent different types of tokens
typedef enum
{
  BEGINNING,
  INT,
  KEYWORD,
  SEPARATOR,
  OPERATOR,
  IDENTIFIER,
  STRING,
  COMP,
  END_OF_TOKENS,
} TokenType;

// Struct to represent a token
typedef struct
{
  TokenType type;
  char *value;
  size_t line_num;
} Token;

// Function to print a token
void print_token(Token token);

// Function to generate a number token
Token *generate_number(char *current, int *current_index);

// Function to generate a keyword token
Token *generate_keyword(char *current, int *current_index);

// Function to generate a separator token
Token *generate_separator_or_operator(char *current, int *current_index);

// Main lexer function
Token *lexer(FILE *file);

#endif
