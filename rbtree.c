#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct node
{
    int data;
    struct node *link[2];
    int color;
};

enum
{
    BLACK,
    RED
};

enum
{
    LEFT,
    RIGHT
};

int get_color(struct node *root)
{
    return (root == NULL) ? BLACK : root->color;
}

void left_rotate(struct node **root);
void right_rotate(struct node **root);
void rotate(struct node **root, int dir);

struct node *create_node(int x);
void rb_insert_r(struct node **root, int d);
void rb_insert(struct node **root, int d);
void balance(struct node **root, int dir);
void delete_tree(struct node **root);

void inorder(struct node *root);
void preorder(struct node *root);
void to_dot(struct node *root, const char filename[]);
void r_to_dot(struct node *root, FILE *f);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <output_type> [filename]\n", argv[0]);
        return 0;
    }

    struct node *my_tree = NULL;

    if (argv[1][0] == '0')
    {
        srand(time(NULL));

        int i = 0;
        for (i = 0; i < 500; i++)
        {
            rb_insert(&my_tree, rand() % 100000);
        }
    }
    else if (argv[1][0] == '1')
    {
        int my_input[15] = { 81, 49, 14, 76, 56,
                             41, 72, 42, 60,  6,
                             66, 75, 47, 58, 19 };

        int i = 0;
        for (i = 0; i < 15; i++)
        {
            rb_insert(&my_tree, my_input[i]);
        }
    }
    else
    {
        printf("<output_type> may be '0' for random data or '1' for "
               "pregenerated data\n");
        return 0;
    }

    if (argc == 3)
    {
        to_dot(my_tree, argv[2]);
    }
    else
    {
        printf("Inorder:\n");
        inorder(my_tree);
        printf("\n");

        printf("Preorder:\n");
        preorder(my_tree);
        printf("\n");
    }

    delete_tree(&my_tree);

    return 0;
}

void rotate(struct node **root, int dir)
{
    if (dir == LEFT)
        left_rotate(root);
    else
        right_rotate(root);
}

void left_rotate(struct node **root)
{
    struct node *x = *root;
    struct node *y = x->link[RIGHT];

    x->link[RIGHT] = y->link[LEFT];
    y->link[LEFT] = x;

    *root = y;

    return;
}

void right_rotate(struct node **root)
{
    struct node *x = *root;
    struct node *y = x->link[LEFT];

    x->link[LEFT] = y->link[RIGHT];
    y->link[RIGHT] = x;

    *root = y;

    return;
}

struct node *create_node(int x)
{
    struct node *new_node = (struct node *)malloc(sizeof(struct node));

    if (new_node != NULL)
    {
        new_node->data = x;
        new_node->link[LEFT] = NULL;
        new_node->link[RIGHT] = NULL;
        new_node->color = RED;
    }

    return new_node;
}

void rb_insert(struct node **root, int d)
{
    rb_insert_r(root, d);
    (*root)->color = BLACK;

    return;
}

void rb_insert_r(struct node **root, int d)
{
    if (*root == NULL)
    {
        *root = create_node(d);
    }
    else if ((*root)->data != d)
    {
        int dir = d > (*root)->data;

        rb_insert_r(&(*root)->link[dir], d);

        /* Begin rebalancing */
        balance(root, dir);
    }
    else
    {
        printf("Since %d is already in the tree, it was not inserted\n", d);
    }

    return;
}

void balance(struct node **root, int dir)
{
    if (get_color((*root)->link[dir]) == RED)
    {
        if (get_color((*root)->link[!dir]) == RED)
        {
            /* CASE 1 */
            (*root)->color = RED;
            (*root)->link[LEFT]->color = BLACK;
            (*root)->link[RIGHT]->color = BLACK;
        }
        else
        {
            /* CASE 2 and 3 */
            if (get_color((*root)->link[dir]->link[!dir]) == RED)
            {
                /* CASE 2 */
                rotate(&(*root)->link[dir], dir);
            }
            /* CASE 3 */
            if (get_color((*root)->link[dir]->link[dir]) == RED)
            {
                (*root)->link[dir]->color = BLACK;
                (*root)->color = RED;
                rotate(root, !dir);
            }
        }
    }
}

void delete_tree(struct node **root)
{
    if (*root == NULL)
        return;

    delete_tree(&((*root)->link[LEFT]));
    delete_tree(&((*root)->link[RIGHT]));

    free(*root);
    *root = NULL;

    return;
}

void inorder(struct node *root)
{
    if (root != NULL)
    {
        inorder(root->link[LEFT]);
        printf("%d\n", root->data);
        inorder(root->link[RIGHT]);
    }
}

void preorder(struct node *root)
{
    if (root != NULL)
    {
        printf("%d\n", root->data);
        preorder(root->link[LEFT]);
        preorder(root->link[RIGHT]);
    }
}

void to_dot(struct node *root, const char filename[])
{
    FILE *f = fopen(filename, "w");

    fprintf(f, "digraph tree {\n");
    if (root)
        r_to_dot(root, f);
    else
        fprintf(f, "\tGit rekt son;\n");
    fprintf(f, "}\n");

    fclose(f);
    return;
}

void r_to_dot(struct node *root, FILE *f)
{
    static int null_count = 0;
    const char *color = ((root->color == RED) ? "red" : "black");

    // Generate the data for this node
    fprintf(f, "\t%ld [label=\"%d\" color=\"%s\"];\n", (long)root,
            root->data, color);

    // Generate the data for this node's left child
    if (root->link[LEFT] != NULL)
    {
        fprintf(f, "\t%ld -> %ld\n", (long)root, (long)root->link[LEFT]);
        r_to_dot(root->link[LEFT], f);
    }
    else
    {
        // Create a new null point
        fprintf(f, "\tnull%d [shape=point color=\"black\"];\n", null_count);

        // Point at the null point
        fprintf(f, "\t%ld -> null%d\n", (long)root, null_count);

        // Increment the null count
        null_count++;
    }

    if (root->link[RIGHT] != NULL)
    {
        fprintf(f, "\t%ld -> %ld\n", (long)root, (long)root->link[RIGHT]);
        r_to_dot(root->link[RIGHT], f);
    }
    else
    {
        // Create a new null point
        fprintf(f, "\tnull%d [shape=point color=\"black\"];\n", null_count);

        // Point at the null point
        fprintf(f, "\t%ld -> null%d\n", (long)root, null_count);

        // Increment the null count
        null_count++;
    }

    return;
}
