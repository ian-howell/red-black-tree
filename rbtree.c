#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct node
{
    int data;
    struct node *left;
    struct node *right;
    struct node *parent;
    int color;
};

enum
{
    BLACK,
    RED
};

int get_color(struct node *root)
{
    return (root == NULL) ? BLACK : root->color;
}

void left_rotate(struct node **root);
void right_rotate(struct node **root);

struct node *create_node(struct node *p, int x);
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
    struct node *y = x->right;

    x->right = y->left;
    if (x->right != NULL)
        x->right->parent = x;

    y->parent = x->parent;

    if (x->parent == NULL)
    {
        *root = y;
    }
    else
    {
        if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void right_rotate(struct node **root)
{
    struct node *x = *root;
    struct node *y = x->left;

    x->left = y->right;
    if (y->right != NULL)
        y->right->parent = x;
    y->parent = x->parent;

    if (x->parent == NULL)
    {
        *root = y;
    }
    else
    {
        if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
    }
    y->right = x;
    x->parent = y;

    return;
}

struct node *create_node(struct node *p, int x)
{
    struct node *new_node = (struct node *)malloc(sizeof(struct node));

    if (new_node != NULL)
    {
        new_node->data = x;
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->parent = p;
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

    delete_tree(&((*root)->left));
    delete_tree(&((*root)->right));

    free(*root);
    *root = NULL;

    return;
}

void inorder(struct node *root)
{
    if (root != NULL)
    {
        inorder(root->left);
        printf("parent = %p\n", (void*)root->parent);
        printf("%d\n", root->data);
        inorder(root->right);
    }
}

void preorder(struct node *root)
{
    if (root != NULL)
    {
        printf("%d ", root->data);
        preorder(root->left);
        preorder(root->right);
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

    // Generate the data for this node's parent
    if (root->parent != NULL)
    {
        fprintf(f, "\t%ld -> %ld\n", (long)root, (long)root->parent);
    }
    else
    {
        fprintf(f, "\tROOT -> %ld [dir=\"back\"]\n", (long)root);
    }

    // Generate the data for this node's left child
    if (root->left != NULL)
    {
        fprintf(f, "\t%ld -> %ld\n", (long)root, (long)root->left);
        r_to_dot(root->left, f);
    }
    else
    {
        // Create a new null point
        //fprintf(f, "\tnull%d [shape=point color=\"black\"];\n", null_count);

        // Point at the null point
        fprintf(f, "\t%ld -> null%d\n", (long)root, null_count);
        fprintf(f, "\tnull%d -> %ld\n", null_count, (long)root);

        // Increment the null count
        null_count++;
    }

    if (root->right != NULL)
    {
        fprintf(f, "\t%ld -> %ld\n", (long)root, (long)root->right);
        r_to_dot(root->right, f);
    }
    else
    {
        // Create a new null point
        //fprintf(f, "\tnull%d [shape=point color=\"black\"];\n", null_count);

        // Point at the null point
        fprintf(f, "\t%ld -> null%d\n", (long)root, null_count);
        fprintf(f, "\tnull%d -> %ld\n", null_count, (long)root);

        // Increment the null count
        null_count++;
    }

    return;
}
