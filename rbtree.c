#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int BUFFER = 80;

struct node
{
    int data;
    struct node *left;
    struct node *right;
    struct node *parent;
    int is_red;
};

struct node *create_node(struct node *p, int x);
void rb_insert(struct node **root, int x);
void delete_tree(struct node **root);

void inorder(struct node *root);
void preorder(struct node *root);
void to_dot(struct node *root, const char filename[]);
void r_to_dot(struct node *root, FILE *f);

int main(int argc, char *argv[])
{
    srand(time(NULL));

    struct node *my_tree = NULL;

    /*
    rb_insert(&my_tree, 54);
    rb_insert(&my_tree, 30);
    rb_insert(&my_tree, 84);
    rb_insert(&my_tree, 12);
    rb_insert(&my_tree, 23);
    rb_insert(&my_tree, 42);
    */

    int i = 0;
    for (i = 0; i < 50; i++)
    {
        rb_insert(&my_tree, rand() % 100);
    }



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

struct node *create_node(struct node *p, int x)
{
    struct node *new_node = (struct node *)malloc(sizeof(struct node));

    if (new_node != NULL)
    {
        new_node->data = x;
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->parent = p;
        new_node->is_red = 1;
    }

    return new_node;
}

void rb_insert(struct node **root, int x)
{
    struct node **p = root;
    struct node *pprime = NULL;

    while (*p != NULL && (*p)->data != x)
    {
        pprime = *p;

        if (x < (*p)->data)
        {
            p = &(*p)->left;
        }
        else if (x > (*p)->data)
        {
            p = &(*p)->right;
        }
    }

    if (*p == NULL)
    {
        *p = create_node(pprime , x);
    }
    else
    {
        printf("Since %d is already in the tree, it was not inserted\n", x);
    }

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

    return;
}

void r_to_dot(struct node *root, FILE *f)
{
    static int null_count = 0;
    const char *color = ((root->is_red) ? "red" : "black");

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
        fprintf(f, "\tnull%d [shape=point color=\"black\"];\n", null_count);

        // Point at the null point
        fprintf(f, "\t%ld -> null%d\n", (long)root, null_count);

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
        fprintf(f, "\tnull%d [shape=point color=\"black\"];\n", null_count);

        // Point at the null point
        fprintf(f, "\t%ld -> null%d\n", (long)root, null_count);

        // Increment the null count
        null_count++;
    }

    return;
}
