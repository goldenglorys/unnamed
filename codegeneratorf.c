#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <ctype.h>
#include <assert.h>

#include "lexerf.h"
#include "parserf.h"
#include "./hashmap/hashmapoperators.h"
#include "./hashmap/hashmap.h"

size_t stack_size = 0;
size_t current_stack_size = 0;
const unsigned initial_size = 100;
struct hashmap_s hashmap;

static int log_and_free_out_of_scope(void *const context, struct hashmap_element_s *const e)
{
    (void)(context);
    if (*(size_t *)e->data > current_stack_size)
    {
        if (hashmap_remove(&hashmap, e->key, strlen(e->key)) != 0)
        {
            printf("COULD NOT REMOVE ELEMENT\n");
        }
        else
        {
            printf("REMOVED %s\n", (char *)e->key);
        }
    }
    return -1;
}

void push(char *reg, FILE *file)
{
    fprintf(file, "  str %s, [sp, #-16]!\n", reg);
    stack_size++;
}

void push_var(size_t stack_pos, FILE *file)
{
    fprintf(file, "  ldr x0, [sp, #%zu]\n", (stack_size - stack_pos) * 16);
    fprintf(file, "  str x0, [sp, #-16]!\n");
    stack_size++;
}

void pop(char *reg, FILE *file)
{
    fprintf(file, "  ldr %s, [sp], #16\n", reg);
    stack_size--;
}

void mov(char *reg1, char *reg2, FILE *file)
{
    fprintf(file, "  mov %s, %s\n", reg1, reg2);
}

Node *handle_adding(Node *tmp, FILE *file)
{
    pop("x0", file);
    tmp = tmp->right;
    if (tmp->left->type == INT)
    {
        fprintf(file, "  mov x1, #%s\n", tmp->left->value);
    }
    else if (tmp->left->type == IDENTIFIER)
    {
        int *element = hashmap_get(&hashmap, tmp->left->value, strlen(tmp->left->value));
        if (element == NULL)
        {
            printf("ERROR: Not in hashmap!\n");
            exit(1);
        }
        push_var(*element, file);
        pop("x1", file);
    }

    fprintf(file, "  add x0, x0, x1\n");
    push("x0", file);
    return tmp;
}

Node *handle_subbing(Node *tmp, FILE *file)
{
    pop("x0", file);
    tmp = tmp->right;
    if (tmp->left->type == INT)
    {
        fprintf(file, "  mov x1, #%s\n", tmp->left->value);
    }
    else if (tmp->left->type == IDENTIFIER)
    {
        int *element = hashmap_get(&hashmap, tmp->left->value, strlen(tmp->left->value));
        if (element == NULL)
        {
            printf("ERROR: Not in hashmap!\n");
            exit(1);
        }
        push_var(*element, file);
        pop("x1", file);
    }
    fprintf(file, "  sub x0, x0, x1\n");
    push("x0", file);
    return tmp;
}

Node *handle_mul(Node *tmp, FILE *file)
{
    pop("x0", file);
    tmp = tmp->right;
    if (tmp->left->type == INT)
    {
        fprintf(file, "  mov x1, #%s\n", tmp->left->value);
    }
    else if (tmp->left->type == IDENTIFIER)
    {
        int *element = hashmap_get(&hashmap, tmp->left->value, strlen(tmp->left->value));
        if (element == NULL)
        {
            printf("ERROR: Not in hashmap!\n");
            exit(1);
        }
        push_var(*element, file);
        pop("x1", file);
    }
    fprintf(file, "  mul x0, x0, x1\n");
    push("x0", file);
    return tmp;
}

Node *handle_div(Node *tmp, FILE *file)
{
    pop("x0", file);
    tmp = tmp->right;
    if (tmp->left->type == INT)
    {
        fprintf(file, "  mov x1, #%s\n", tmp->left->value);
    }
    else if (tmp->left->type == IDENTIFIER)
    {
        int *element = hashmap_get(&hashmap, tmp->left->value, strlen(tmp->left->value));
        if (element == NULL)
        {
            printf("ERROR: Not in hashmap!\n");
            exit(1);
        }
        push_var(*element, file);
        pop("x1", file);
    }
    fprintf(file, "  udiv x0, x0, x1\n");
    push("x0", file);
    return tmp;
}

Node *generate_operator_code(Node *node, FILE *file)
{
    Node *tmp = node;
    if (tmp->left->type == INT)
    {
        fprintf(file, "  mov x0, #%s\n", tmp->left->value);
    }
    else if (tmp->left->type == IDENTIFIER)
    {
        int *element = hashmap_get(&hashmap, tmp->left->value, strlen(tmp->left->value));
        if (element == NULL)
        {
            printf("ERROR: Not in hashmap!\n");
            exit(1);
        }
        push_var(*element, file);
        pop("x0", file);
    }
    push("x0", file);
    int did_loop = 0;
    while (tmp->right->type == OPERATOR)
    {
        did_loop = 1;
        char oper = tmp->value[0];
        if (oper == '+')
        {
            tmp = handle_adding(tmp, file);
        }
        else if (oper == '-')
        {
            tmp = handle_subbing(tmp, file);
        }
        else if (oper == '*')
        {
            tmp = handle_mul(tmp, file);
        }
        else if (oper == '/')
        {
            tmp = handle_div(tmp, file);
        }
    }
    if (did_loop)
    {
        if (tmp->value[0] == '*' || tmp->value[0] == '/')
        {
            pop("x0", file);
            if (tmp->right->type == INT)
            {
                fprintf(file, "  mov x1, #%s\n", tmp->right->value);
            }
            else if (tmp->right->type == IDENTIFIER)
            {
                int *element = hashmap_get(&hashmap, tmp->right->value, strlen(tmp->right->value));
                if (element == NULL)
                {
                    printf("ERROR: Not in hashmap!\n");
                    exit(1);
                }
                push_var(*element, file);
                pop("x1", file);
            }
            fprintf(file, "  %s x0, x0, x1\n", tmp->value[0] == '*' ? "mul" : "udiv");
            push("x0", file);
        }
        else
        {
            pop("x0", file);
            if (tmp->right->type == INT)
            {
                fprintf(file, "  mov x1, #%s\n", tmp->right->value);
            }
            else if (tmp->right->type == IDENTIFIER)
            {
                int *element = hashmap_get(&hashmap, tmp->right->value, strlen(tmp->right->value));
                if (element == NULL)
                {
                    printf("ERROR: Not in hashmap!\n");
                    exit(1);
                }
                push_var(*element, file);
                pop("x1", file);
            }
            fprintf(file, "  %s x0, x0, x1\n", tmp->value[0] == '+' ? "add" : "sub");
            push("x0", file);
        }
    }
    else
    {
        if (tmp->value[0] == '-')
        {
            if (tmp->right->type == INT)
            {
                fprintf(file, "  mov x1, #%s\n", tmp->right->value);
            }
            else if (tmp->right->type == IDENTIFIER)
            {
                int *element = hashmap_get(&hashmap, tmp->right->value, strlen(tmp->right->value));
                if (element == NULL)
                {
                    printf("ERROR: Not in hashmap!\n");
                    exit(1);
                }
                push_var(*element, file);
                pop("x1", file);
            }
            pop("x0", file);
            fprintf(file, "  sub x0, x0, x1\n");
            push("x0", file);
        }
        else if (tmp->value[0] == '+')
        {
            if (tmp->right->type == INT)
            {
                fprintf(file, "  mov x1, #%s\n", tmp->right->value);
            }
            else if (tmp->right->type == IDENTIFIER)
            {
                int *element = hashmap_get(&hashmap, tmp->right->value, strlen(tmp->right->value));
                if (element == NULL)
                {
                    printf("ERROR: Not in hashmap!\n");
                    exit(1);
                }
                push_var(*element, file);
                pop("x1", file);
            }
            fprintf(file, "  add x0, x0, x1\n");
            push("x0", file);
        }
        else if (tmp->value[0] == '*')
        {
            if (tmp->right->type == INT)
            {
                fprintf(file, "  mov x1, #%s\n", tmp->right->value);
            }
            else if (tmp->right->type == IDENTIFIER)
            {
                int *element = hashmap_get(&hashmap, tmp->right->value, strlen(tmp->right->value));
                if (element == NULL)
                {
                    printf("ERROR: Not in hashmap!\n");
                    exit(1);
                }
                push_var(*element, file);
                pop("x1", file);
            }
            fprintf(file, "  mul x0, x0, x1\n");
            push("x0", file);
        }
        else if (tmp->value[0] == '/')
        {
            if (tmp->right->type == INT)
            {
                fprintf(file, "  mov x1, #%s\n", tmp->right->value);
            }
            else if (tmp->right->type == IDENTIFIER)
            {
                int *element = hashmap_get(&hashmap, tmp->right->value, strlen(tmp->right->value));
                if (element == NULL)
                {
                    printf("ERROR: Not in hashmap!\n");
                    exit(1);
                }
                push_var(*element, file);
                pop("x1", file);
            }
            fprintf(file, "  udiv x0, x0, x1\n");
            push("x0", file);
        }
    }
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
        syscall_number = 1;
    }
    if (strcmp(node->value, "INT") == 0)
    {
        Node *value = node->left->left->left;
        if (value->type == IDENTIFIER)
        {
            size_t *var_value = hashmap_get(&hashmap, value->value, strlen(value->value));
            if (var_value == NULL)
            {
                printf("ERROR: %s Not Declared In Current Context\n", value->value);
                exit(1);
            }
            push_var(*var_value, file);
        }
        else if (value->type == INT)
        {
            fprintf(file, "  mov x0, #%s\n", value->value);
            push("x0", file);
        }
        else if (value->type == OPERATOR)
        {
            generate_operator_code(value, file);
        }
        else
        {
            printf("ERROR\n");
            exit(1);
        }
        size_t *cur_size = malloc(sizeof(size_t));
        *cur_size = stack_size;
        if (hashmap_put(&hashmap, node->left->value, strlen(node->left->value), cur_size) != 0)
        {
            printf("ERROR: Could not insert into hash table!\n");
            exit(1);
        }
        node->left = NULL;
    }
    if (strcmp(node->value, "(") == 0)
    {
    }
    if (node->type == OPERATOR)
    {
        if (node->value[0] == '=')
        {
        }
        else
        {
            generate_operator_code(node, file);
        }
    }
    if (node->type == INT)
    {
        fprintf(file, "  mov x0, #%s\n", node->value);
        push("x0", file);
    }
    if (node->type == IDENTIFIER)
    {
        if (syscall_number == 1)
        {
            fprintf(file, "  mov x16, #%d\n", syscall_number);
            size_t *var_value = hashmap_get(&hashmap, node->value, strlen(node->value));
            if (var_value == NULL)
            {
                printf("ERROR: Not Declared in current scope: %s\n", node->value);
                exit(1);
            }
            push_var(*var_value, file);
            pop("x0", file);
            fprintf(file, "  svc #0x80\n");
        }
    }
    if (strcmp(node->value, ")") == 0)
    {
    }

    if (strcmp(node->value, "{") == 0)
    {
        current_stack_size = stack_size;
    }

    if (strcmp(node->value, "}") == 0)
    {
        void *log = malloc(sizeof(char));
        if (hashmap_iterate_pairs(&hashmap, log_and_free_out_of_scope, (void *)log) != 0)
        {
            printf("ERROR: Could not free\n");
            exit(1);
        }
        for (; stack_size > current_stack_size;)
        {
            pop("x1", file);
        }
    }

    if (strcmp(node->value, ";") == 0)
    {
        if (syscall_number == 1)
        {
            fprintf(file, "  mov x16, #%d\n", syscall_number);
            fprintf(file, "  ldr x0, [sp], #16\n");
            fprintf(file, "  svc #0x80\n");
        }
    }
    if (is_left)
    {
    }
    else
    {
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
    insert('/', "udiv");
    FILE *file = fopen("generated.asm", "w");
    assert(file != NULL && "FILE COULD NOT BE OPENED\n");

    assert(hashmap_create(initial_size, &hashmap) == 0 && "ERROR: Could not create hashmap\n");

    fprintf(file, ".global _main\n\n");
    fprintf(file, ".text\n");
    fprintf(file, "_main:\n");

    traverse_tree(root, 0, file, 0);
    fclose(file);

    return 0;
}