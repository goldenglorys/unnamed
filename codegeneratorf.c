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
    if (node == NULL)
    {
        fprintf(stderr, "generate_operator_code: node is NULL\n");
        return NULL;
    }

    if (strcmp(node->value, "=") == 0)
    {
        if (node->right == NULL)
        {
            fprintf(stderr, "generate_operator_code: node->right is NULL\n");
            return node;
        }

        fprintf(file, "  ldr x0, =x\n");                      // Load the address of x into x0
        fprintf(file, "  ldr x1, =%s\n", node->right->value); // Load the value of the right operand into x1
        fprintf(file, "  str x1, [x0]\n");                    // Store the value in x1 to the address in x0
        printf("EQUALS\n");
    }
    else
    {
        Node *tmp = node;
        fprintf(file, "  ldr x0, =x\n");   // Load the address of x into x0
        fprintf(file, "  ldr x0, [x0]\n"); // Load the value at the address in x0 into x0
        int did_loop = 0;
        while (tmp->right != NULL && tmp->right->type == OPERATOR)
        {
            did_loop = 1;
            char *oper = search(tmp->value[0])->data;
            tmp = tmp->right;
            if (tmp->left == NULL)
            {
                fprintf(stderr, "generate_operator_code: tmp->left is NULL\n");
                return node;
            }
            fprintf(file, "  ldr x1, =%s\n", tmp->left->value);
            if (strcmp(oper, "mul") == 0 || strcmp(oper, "div") == 0)
            {
                fprintf(file, "  %s x0, x0, x1\n", oper);
                fprintf(file, "  mov x8, x0\n");
            }
            else
            {
                fprintf(file, "  %s x0, x0, x1\n", oper);
                fprintf(file, "  mov x8, x0\n");
            }
        }
        if (did_loop)
        {
            if (tmp->value[0] == '*' || tmp->value[0] == '/')
            {
                fprintf(file, "  mov x0, x8\n");
                if (tmp->right == NULL)
                {
                    fprintf(stderr, "generate_operator_code: tmp->right is NULL\n");
                    return node;
                }
                fprintf(file, "  ldr x1, =%s\n", tmp->right->value);
                fprintf(file, "  %s x0, x0, x1\n", search(tmp->value[0])->data);
                fprintf(file, "  mov x8, x0\n");
            }
            else
            {
                if (tmp->right == NULL)
                {
                    fprintf(stderr, "generate_operator_code: tmp->right is NULL\n");
                    return node;
                }
                fprintf(file, "  %s x8, x8, %s\n", search(tmp->value[0])->data, tmp->right->value);
            }
        }
        else
        {
            if (tmp->right == NULL)
            {
                fprintf(stderr, "generate_operator_code: tmp->right is NULL\n");
                return node;
            }
            fprintf(file, "  ldr x1, =%s\n", tmp->right->value);
            fprintf(file, "  %s x0, x0, x1\n", search(tmp->value[0])->data);
            fprintf(file, "  mov x8, x0\n");
        }
        fprintf(file, "  mov x0, %d\n", syscall_number);
        fprintf(file, "  svc #0\n");
        node->left = NULL;
        node->right = NULL;
    }
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
        syscall_number = 93;
    }
    if (node->type == OPERATOR)
    {
        generate_operator_code(node, syscall_number, file);
        printf("OPERATOR ERRORS\n");
    }
    if (node->type == INT)
    {
        fprintf(file, "    mov x8, %d\n", syscall_number);
        fprintf(file, "    mov x0, %s\n", node->value);
        fprintf(file, "    svc #0x80\n");
    }
    if (node->type == IDENTIFIER)
    {
        if (syscall_number == 93)
        {
            fprintf(file, "    mov x8, %d\n", syscall_number);
            fprintf(file, "    ldr x8, [sp], #8\n");
            fprintf(file, "    svc #0x80\n");
        }
    }
    for (size_t i = 0; node->value[i] != '\0'; i++)
    {
        printf("%c", node->value[i]);
    }
    printf("\n");
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
    fprintf(file, ".data\n");
    fprintf(file, "x: .quad 0\n");
    fprintf(file, "_main:\n");

    // Traverse the tree to generate the assembly code
    traverse_tree(root, 0, file, 0);

    // Call _exit at the end of the program
    fprintf(file, "    bl _exit\n");
    fclose(file);

    return 0;
}