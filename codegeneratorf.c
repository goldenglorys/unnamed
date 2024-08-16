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

#define MAX_STACK_SIZE_SIZE 1024

size_t stack_size = 0;
int current_stack_size_size = 0;
int label_number = 0;
size_t current_stack_size[MAX_STACK_SIZE_SIZE];
const unsigned initial_size = 100;
struct hashmap_s hashmap;

typedef enum
{
    ADD,
    SUB,
    DIV,
    MUL,
    NOT_OPERATOR
} OperatorType;

void create_label(FILE *file)
{
    fprintf(file, ".L%d:\n", label_number);
    label_number++;
}

void stack_push(size_t value)
{
    printf("current stack: %zu\n", current_stack_size[current_stack_size_size]);
    current_stack_size[current_stack_size_size] = value;
    current_stack_size_size++;
    printf("current stack: %zu\n", current_stack_size[current_stack_size_size]);
}

size_t stack_pop()
{
    printf("POP\n");
    if (current_stack_size_size == 0)
    {
        printf("ERROR: stack is already empty\n");
        exit(1);
    }
    size_t result = current_stack_size[current_stack_size_size];
    current_stack_size_size--;
    return result;
}

static int log_and_free_out_of_scope(void *const context, struct hashmap_element_s *const e)
{
    (void)(context);
    if (*(size_t *)e->data > current_stack_size[current_stack_size_size])
    {
        if (hashmap_remove(&hashmap, e->key, strlen(e->key)) != 0)
        {
            printf("COULD NOT REMOVE ELEMENT\n");
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

OperatorType check_operator(Node *node)
{
    if (node->type != OPERATOR)
    {
        return NOT_OPERATOR;
    }

    if (strcmp(node->value, "+") == 0)
    {
        return ADD;
    }
    if (strcmp(node->value, "-") == 0)
    {
        return SUB;
    }
    if (strcmp(node->value, "/") == 0)
    {
        return DIV;
    }
    if (strcmp(node->value, "*") == 0)
    {
        return MUL;
    }
    return NOT_OPERATOR;
}

int mov_if_var_or_not(char *reg, Node *node, FILE *file)
{
    if (node->type == IDENTIFIER)
    {
        int *value = malloc(sizeof(int));
        value = hashmap_get(&hashmap, node->value, strlen(node->value));
        if (value == NULL)
        {
            printf("ERROR: Variable %s not declared in current scope\n", node->value);
            exit(1);
        }
        push_var(*value, file);
        pop(reg, file);
        return 0;
    }
    if (node->type == INT)
    {
        fprintf(file, "  mov %s, %s\n", reg, node->value);
        return 0;
    }
    return -1;
}

Node *generate_operator_code(Node *node, FILE *file)
{
    mov_if_var_or_not("x0", node->left, file);
    push("x0", file);
    Node *tmp = node;
    OperatorType oper_type = check_operator(tmp);
    while (tmp->type == OPERATOR)
    {
        pop("x0", file);
        oper_type = check_operator(tmp);
        tmp = tmp->right;
        if (tmp->type != OPERATOR)
        {
            break;
        }
        mov_if_var_or_not("x1", tmp->left, file);
        switch (oper_type)
        {
        case ADD:
            fprintf(file, "  add x0, x0, x1\n");
            break;
        case SUB:
            fprintf(file, "  sub x0, x0, x1\n");
            break;
        case DIV:
            fprintf(file, "  udiv x0, x0, x1\n");
            break;
        case MUL:
            fprintf(file, "  mul x0, x0, x1\n");
            break;
        case NOT_OPERATOR:
            printf("ERROR: Invalid Syntax\n");
            exit(1);
        }
        push("x0", file);
        oper_type = check_operator(tmp);
    }
    mov_if_var_or_not("x1", tmp, file);
    switch (oper_type)
    {
    case ADD:
        fprintf(file, "  add x0, x0, x1\n");
        break;
    case SUB:
        fprintf(file, "  sub x0, x0, x1\n");
        break;
    case DIV:
        fprintf(file, "  udiv x0, x0, x1\n");
        break;
    case MUL:
        fprintf(file, "  mul x0, x0, x1\n");
        break;
    case NOT_OPERATOR:
        printf("ERROR: Invalid Syntax\n");
        exit(1);
    }
    push("x0", file);
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
        if (hashmap_get(&hashmap, node->left->value, strlen(node->left->value)) != NULL)
        {
            printf("ERROR: Variable %s is already declared in current scope\n", node->left->value);
            exit(1);
        }
        if (hashmap_put(&hashmap, node->left->value, strlen(node->left->value), cur_size) != 0)
        {
            printf("ERROR: Could not insert into hash table!\n");
            exit(1);
        }
        node->left = NULL;
    }
    // if (strcmp(node->value, "IF") == 0)
    // {
    //     Node *current = malloc(sizeof(Node));
    //     current = node->left->left;
    //     printf("MADE IT HERE IF\n");

    //     if (current->left->type == INT || current->left->type == IDENTIFIER)
    //     {
    //         mov_if_var_or_not("x0", current->left, file);
    //         push("x0", file);
    //     }
    //     else
    //     {
    //         generate_operator_code(current->left, file);
    //     }

    //     printf("MADE IT HERE IF\n");

    //     if (current->right->type == INT || current->right->type == IDENTIFIER)
    //     {
    //         mov_if_var_or_not("x1", current->right, file);
    //         push("x1", file);
    //     }
    //     else
    //     {
    //         generate_operator_code(current->right, file);
    //     }

    //     pop("x1", file);
    //     pop("x0", file);
    //     fprintf(file, "  cmp x0, x1\n");
    //     fprintf(file, "  b.ne 1f\n");
    //     fprintf(file, "  // IF body goes here\n");
    //     fprintf(file, "  b 2f\n");
    //     fprintf(file, "1:\n");
    //     fprintf(file, "  // ELSE body goes here (if any)\n");
    //     fprintf(file, "2:\n");
    //     node->left->left = NULL;
    // }

    if (strcmp(node->value, "IF") == 0)
    {
        Node *current = node->left->left;
        printf("MADE IT HERE IF\n");

        if (current->left->type == INT || current->left->type == IDENTIFIER)
        {
            mov_if_var_or_not("x0", current->left, file);
            push("x0", file);
        }
        else
        {
            generate_operator_code(current->left, file);
        }

        printf("MADE IT HERE IF\n");

        if (current->right->type == INT || current->right->type == IDENTIFIER)
        {
            mov_if_var_or_not("x1", current->right, file);
            push("x1", file);
        }
        else
        {
            generate_operator_code(current->right, file);
        }

        pop("x1", file);
        pop("x0", file);

        // Generate unique labels for each IF statement
        int if_label = label_number++;
        int else_label = label_number++;
        int end_label = label_number++;

        // Handle different comparison operators
        if (strcmp(current->value, "==") == 0)
        {
            fprintf(file, "  cmp x0, x1\n");
            fprintf(file, "  b.ne .L%d\n", else_label);
        }
        else if (strcmp(current->value, "!=") == 0)
        {
            fprintf(file, "  cmp x0, x1\n");
            fprintf(file, "  b.eq .L%d\n", else_label);
        }
        else if (strcmp(current->value, "<") == 0)
        {
            fprintf(file, "  cmp x0, x1\n");
            fprintf(file, "  b.ge .L%d\n", else_label);
        }
        else if (strcmp(current->value, ">") == 0)
        {
            fprintf(file, "  cmp x0, x1\n");
            fprintf(file, "  b.le .L%d\n", else_label);
        }
        else if (strcmp(current->value, "<=") == 0)
        {
            fprintf(file, "  cmp x0, x1\n");
            fprintf(file, "  b.gt .L%d\n", else_label);
        }
        else if (strcmp(current->value, ">=") == 0)
        {
            fprintf(file, "  cmp x0, x1\n");
            fprintf(file, "  b.lt .L%d\n", else_label);
        }
        else
        {
            printf("ERROR: Unsupported comparison operator\n");
            exit(1);
        }

        fprintf(file, "  // IF body goes here\n");
        fprintf(file, "  b .L%d\n", end_label);
        fprintf(file, ".L%d:\n", else_label);
        fprintf(file, "  // ELSE body goes here (if any)\n");
        fprintf(file, ".L%d:\n", end_label);

        node->left->left = NULL;
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
        else
        {
            printf("VARIABLE IS HERE\n");
            if (hashmap_get(&hashmap, node->value, strlen(node->value)) == NULL)
            {
                printf("ERROR: Variable %s is not declared in current scope\n", node->value);
                exit(1);
            }

            Node *value = node->left->left;
            if (value->type == IDENTIFIER)
            {
                size_t *var_value = malloc(sizeof(size_t));
                var_value = hashmap_get(&hashmap, value->value, strlen(value->value));
                if (var_value == NULL)
                {
                    printf("ERROR: %s Not Declared In Current Context\n", value->value);
                    exit(1);
                }
                push_var(*var_value, file);
            }
            else if (value->type == INT)
            {
                push(value->value, file);
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
            if (hashmap_put(&hashmap, node->value, strlen(node->value), cur_size) != 0)
            {
                printf("ERROR: Could not insert into hash table!\n");
                exit(1);
            }
            node->left = NULL;
        }
    }
    if (strcmp(node->value, ")") == 0)
    {
    }

    if (strcmp(node->value, "{") == 0)
    {
        stack_push(stack_size);
    }

    if (strcmp(node->value, "}") == 0)
    {
        create_label(file);
        void *log = malloc(sizeof(char));
        if (hashmap_iterate_pairs(&hashmap, log_and_free_out_of_scope, (void *)log) != 0)
        {
            printf("ERROR: Could not free\n");
            exit(1);
        }
        size_t stack_value = stack_pop();
        for (; stack_size > stack_value;)
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