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

struct node *create_node(int x);
void rb_insert(struct node **root, int d);
void delete_tree(struct node **root);

void inorder(struct node *root);
void preorder(struct node *root);
void to_dot(struct node *root, const char filename[]);
void r_to_dot(struct node *root, FILE *f);

int main(int argc, char *argv[])
{
    srand(time(NULL));

    struct node *my_tree = NULL;

    rb_insert(&my_tree, 81);
    rb_insert(&my_tree, 49);
    rb_insert(&my_tree, 14);
    rb_insert(&my_tree, 76);
    rb_insert(&my_tree, 56);

    rb_insert(&my_tree, 41);
    rb_insert(&my_tree, 72);
    rb_insert(&my_tree, 42);
    rb_insert(&my_tree, 60);
    rb_insert(&my_tree, 6);

    rb_insert(&my_tree, 66);
    rb_insert(&my_tree, 75);
    rb_insert(&my_tree, 47);
    rb_insert(&my_tree, 58);
    rb_insert(&my_tree, 19);

    /*
    int i = 0;
    for (i = 0; i < 50; i++)
    {
        rb_insert(&my_tree, rand() % 100);
    }
    */

    if (argc == 2)
    {
        to_dot(my_tree, argv[1]);
    }
    else
    {
        printf("Inorder:\n");
        inorder(my_tree);
        printf("\n");

        printf("Preorder: ");
        preorder(my_tree);
        printf("\n");
    }

    delete_tree(&my_tree);

    return 0;
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
    struct node **x = root;
    struct node *p = NULL;

    while (*x != NULL && (*x)->data != d)
    {
        p = *x;
        if (d < (*x)->data)
            x = &(*x)->left;
        else if (d > (*x)->data)
            x = &(*x)->right;
    }

    if (*x == NULL)
        *x = create_node(p, d);
    else
        printf("Since %d is already in the tree, it was not inserted\n", d);

    while (p != NULL && p->parent != NULL)
    {
        // all of the rebalancing goes somewhere in here
        struct node *c = p;
        p = p->parent;

        if (get_color(c) == RED)
        {
            struct node *u = p->left;
            if (c == u)
                u = p->right;

            if (get_color(u) == RED)
            {
                // CASE 1
                p->color = RED;
                c->color = BLACK;
                u->color = BLACK;
            }
            else
            {
                if (c == p->right)
                {
                    // CASE A:
                    struct node *gc = c->right;
                    if (get_color(gc) == BLACK) // CASE 2: zig-zag
                        right_rotate(&c);
                    // CASE 3: zig-zig
                    // recolor first for simplicity
                    p->color = RED;
                    p->right->color = BLACK;
                    left_rotate(&p);
                }
                else
                {
                    // CASE B:
                    struct node *gc = c->left;
                    if (get_color(gc) == BLACK) // CASE 2: zig-zag
                        left_rotate(&c);
                    // CASE 3: zig-zig
                    // recolor first for simplicity
                    p->color = RED;
                    p->left->color = BLACK;
                    right_rotate(&p);
                }
            }
        }
    }

    *root = (p == NULL) ? *root : p;

    // Fix violation 1 errors
    (*root)->color = BLACK;

    return;
}

void delete_tree(struct node **root)
{
    if (*root == NULL)
    {
        return;
    }

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
        printf("%d ", root->data);
        preorder(root->link[LEFT]);
        preorder(root->link[RIGHT]);
    }
}

void to_dot(struct node *root, const char filename[])
{
    FILE *f = fopen(filename, "w");

    fprintf(f, "digraph tree {\n");
    r_to_dot(root, f);
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
