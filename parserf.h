#ifndef PARSER_H_
#define PARSER_H_

#include "lexerf.h"

typedef enum
{
    INT_LIT,
    STATEMENT,
    EXTRA,
    BEGINNING,
} NodeTypes;

typedef struct Node
{
    char *value;
    NodeTypes type;
    struct Node *right;
    struct Node *left;
} Node;

// Function to print the syntax tree
void print_tree(Node *node);

// Function to initialize a node
Node *init_node(Node *node, char *value, NodeTypes type);

// Main parser function
Token *parser(Token *tokens);

#endif
