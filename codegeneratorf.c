#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "lexerf.h"
#include "parserf.h"

void traverse_tree(Node *node, int is_left, FILE *file)
{
    if (node == NULL)
    {
        return;
    }

    if (strcmp(node->value, "EXIT") == 0)
    {
        // Set up for exit system call
        fprintf(file, "    mov x16, #0x1\n"); // Exit syscall number for ARM
    }

    if (node->type == INT)
    {
        // Move integer value into x0 for the syscall
        fprintf(file, "    mov x0, %s\n", node->value); // Assuming node->value is a string representation of the integer
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
    traverse_tree(node->left, 1, file);
    traverse_tree(node->right, 0, file);
}

int generate_code(Node *root)
{
    FILE *file = fopen("generated.asm", "w");
    assert(file != NULL && "FILE COULD NOT BE OPENED\n");

    // Change section declaration and directives to the correct syntax
    fprintf(file, ".section __TEXT,__text\n");
    fprintf(file, ".global _main\n");
    fprintf(file, ".extern _exit\n\n");
    fprintf(file, "_main:\n");

    // Traverse the tree to generate the assembly code
    traverse_tree(root, 0, file);

    // Call _exit at the end of the program
    fprintf(file, "    bl _exit\n");
    fclose(file);

    return 0;
}