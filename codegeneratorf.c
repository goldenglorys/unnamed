#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "lexerf.h"
#include "parserf.h"
#include "./hashmap/hashmap.h"

Node *generate_operator_code(Node *node, int syscall_number, FILE *file)
{
    Node *tmp = node;
    fprintf(file, "  ldr x0, =%s\n", node->left->value);
    int did_loop = 0;
    while (tmp->right->type == OPERATOR)
    {
        did_loop = 1;
        char *oper = search(tmp->value[0])->data;
        tmp = tmp->right;
        fprintf(file, "  ldr x1, =%s\n", tmp->left->value);
        if (strcmp(oper, "mul") == 0 || strcmp(oper, "sdiv") == 0)
        {
            fprintf(file, "  %s x0, x0, x1\n", oper);
        }
        else
        {
            fprintf(file, "  %s x0, x0, x1\n", oper);
        }
    }
    if (did_loop)
    {
        if (tmp->value[0] == '*' || tmp->value[0] == '/')
        {
            fprintf(file, "  ldr x1, =%s\n", tmp->right->value);
            fprintf(file, "  %s x0, x0, x1\n", search(tmp->value[0])->data);
        }
        else
        {
            fprintf(file, "  ldr x1, =%s\n", tmp->right->value);
            fprintf(file, "  %s x0, x0, x1\n", search(tmp->value[0])->data);
        }
    }
    else
    {
        fprintf(file, "  ldr x1, =%s\n", tmp->right->value);
        fprintf(file, "  %s x0, x0, x1\n", search(tmp->value[0])->data);
    }

    fprintf(file, "  mov x16, %d\n", syscall_number); // Syscall number in x16
    node->left = NULL;
    node->right = NULL;
    return node;
}

void traverse_tree(Node *node, int is_left, FILE *file, int syscall_number)
{
    if (node == NULL)
    {
        return;
    }

    if (strcmp(node->value, "EXIT") == 0)
    {
        syscall_number = 60;
    }

    if (node->type == OPERATOR)
    {

        generate_operator_code(node, syscall_number, file);
    }
    if (node->type == INT)
    {
        // Move integer value into x0 for the syscall
        fprintf(file, "    mov x0, %s\n", node->value);
    }

    if (strcmp(node->value, ";") == 0)
    {
        // Invoke the syscall
        fprintf(file, "    svc #0x80\n");
    }

    // Print the value for debugging, if necessary
    for (size_t i = 0; node->value[i] != '\0'; i++)
    {
        printf("%c", node->value[i]);
    }
    printf("\n");

    // Recursive calls to traverse through the tree
    traverse_tree(node->left, 1, file, syscall_number);
    traverse_tree(node->right, 0, file, syscall_number);
}

int generate_code(Node *root)
{
    insert('-', "sub");
    insert('+', "add");
    insert('*', "mul");
    insert('/', "div");
    FILE *file = fopen("generated.asm", "w");
    assert(file != NULL && "FILE COULD NOT BE OPENED\n");

    // Change section declaration and directives to the correct syntax
    fprintf(file, ".section __TEXT,__text\n");
    fprintf(file, ".global _main\n");
    fprintf(file, ".extern _exit\n\n");
    fprintf(file, "_main:\n");

    // Traverse the tree to generate the assembly code
    traverse_tree(root, 0, file, 0);

    // Call _exit at the end of the program
    fprintf(file, "    bl _exit\n");
    fclose(file);

    return 0;
}