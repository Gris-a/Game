#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#include "../include/tree.h"

Tree TreeCtor(char *init_val)
{
    ASSERT(init_val, return {});

    Node *root = NodeCtor(init_val);
    ASSERT(root, return {});

    Tree tree = {root, 1};

    return tree;
}


static void SubTreeDtor(Tree *tree, Node *sub_tree)
{
    if(!sub_tree) return;

    SubTreeDtor(tree, sub_tree->left );
    SubTreeDtor(tree, sub_tree->right);

    free(sub_tree);

    tree->size--;
}

int TreeDtor(Tree *tree, Node *root)
{
    TREE_VER(tree, EXIT_FAILURE);

    ASSERT(root, return EXIT_FAILURE);
    ASSERT(root == tree->root || TreeSearchParent(tree, root), return EXIT_FAILURE);

    SubTreeDtor(tree, root->left );
    SubTreeDtor(tree, root->right);

    if(tree->root != root)
    {
        root->left  = NULL;
        root->right = NULL;

        Node *parent = TreeSearchParent(tree, root);

        if(parent->left == root) parent->left  = NULL;
        else                     parent->right = NULL;
    }
    else
    {
        tree->root = NULL;
    }

    free(root);
    tree->size--;

    return EXIT_SUCCESS;
}


Node *AddNode(Tree *tree, Node *tree_node, char *val, PlacePref pref)
{
    ASSERT(val, return NULL);

    TREE_VER(tree, NULL);

    ASSERT(tree_node, return NULL);
    ASSERT(tree_node == tree->root || TreeSearchParent(tree, tree_node), return NULL);

    Node **next = &tree_node;
    while(*next)
    {
        switch(pref)
        {
            case LEFT:
            {
                next = &((*next)->left);

                break;
            }
            case RIGHT:
            {
                next = &((*next)->right);

                break;
            }
            case AUTO:
            {
                if(strcmp(val, (*next)->data) <= 0) next = &((*next)->left );
                else                                next = &((*next)->right);

                break;
            }
            default: return NULL;
        }
    }

    EXEC_ASSERT((*next) = NodeCtor(val), return NULL);
    tree->size++;

    return (*next);
}


static Node *SubTreeSearchVal(Node *const tree_node, char *val)
{
    if(!tree_node) return NULL;

    if(strncmp(tree_node->data, val, MAX_LEN) == 0) return tree_node;

    Node *find  = SubTreeSearchVal(tree_node->left , val);

    return (find ? find : SubTreeSearchVal(tree_node->right, val));
}

Node *TreeSearchVal(Tree *const tree, char *val)
{
    ASSERT(val, return NULL);

    TREE_VER(tree, NULL);

    return SubTreeSearchVal(tree->root, val);
}


static bool SubTreeValPath(Node *const tree_node, char *const val, Stack *path)
{
    if(!tree_node) return false;

    if(strncmp(tree_node->data, val, MAX_LEN) == 0) return true;

    if(SubTreeValPath(tree_node->left, val, path))
    {
        PushStack(path, false);

        return true;
    }
    else if(SubTreeValPath(tree_node->right, val, path))
    {
        PushStack(path, true);

        return true;
    }

    return false;
}

Stack TreeValPath(Tree *const tree, char *const val)
{
    ASSERT(val, return {});

    TREE_VER(tree, {});

    Stack path = StackCtor();
    ASSERT(path.data, return {});

    if(!SubTreeValPath(tree->root, val, &path))
    {
        StackDtor(&path);

        return {};
    }

    return path;
}


static Node *SubTreeSearchParent(Node *const tree_node, Node *const search_node)
{
    if(!tree_node) return NULL;

    if(tree_node->left  == search_node ||
       tree_node->right == search_node) return tree_node;

    Node *find  = SubTreeSearchParent(tree_node->left , search_node);

    return (find ? find : SubTreeSearchParent(tree_node->right, search_node));
}

Node *TreeSearchParent(Tree *const tree, Node *const search_node)
{
    ASSERT(search_node, return NULL);

    TREE_VER(tree, NULL);

    return SubTreeSearchParent(tree->root, search_node);
}


Node *NodeCtor(char *const val, Node *const left, Node *const right)
{
    ASSERT(val, return NULL);

    Node *node = (Node *)calloc(1, sizeof(Node));
    ASSERT(node, return NULL);

    strncpy(node->data, val, MAX_LEN - 1);
    node->left  = left;
    node->right = right;

    return node;
}


static void NodeEdgeGen(Node *const node, Node *const node_next, char const *direction, FILE *file)
{
    if(node_next)
    {
        fprintf(file, "node%p[label = \"{<data> data: %s | {<left> l: %p| <right> r: %p}}\"];\n",
                                        node_next, node_next->data, node_next->left, node_next->right);

        fprintf(file, "node%p:<%s>:s -> node%p:<data>:n;\n", node, direction, node_next);

        NodeEdgeGen(node_next, node_next->left , "left" , file);
        NodeEdgeGen(node_next, node_next->right, "right", file);
    }
}

void TreeDot(Tree *const tree, const char *path)
{
    ASSERT(tree, return);
    ASSERT(path, return);

    FILE *file = fopen(path, "wb");
    ASSERT(file, return);
    setbuf(file, NULL);

    fprintf(file, "digraph\n"
                  "{\n"
                  "bgcolor = \"grey\";\n"
                  "ranksep = \"equally\";\n"
                  "node[shape = \"Mrecord\"; style = \"filled\"; fillcolor = \"#58CD36\"];\n"
                  "{rank = source;");

    ASSERT(tree->root, return);

    fprintf(file, "node%p[label = \"{<data> %s | {<left> Nein | <right> Ja}}\"; fillcolor = \"orchid\"]};\n",
                                                                                tree->root, tree->root->data);

    NodeEdgeGen(tree->root, tree->root->left , "left" , file);
    NodeEdgeGen(tree->root, tree->root->right, "right", file);

    fprintf(file, "}\n");

    fclose(file);

    char sys_cmd[MAX_LEN] = {};
    sprintf(sys_cmd, "dot %s -T png -o data/tree.png", path);
    system(sys_cmd);
}


static void SubTreeDump(Node *const tree_node, FILE *dump_file)
{
    if(!tree_node) {fprintf(dump_file, "*"); return;}

    fprintf(dump_file, "\n(");
    fprintf(dump_file, "<%s>", tree_node->data);
    SubTreeDump(tree_node->left , dump_file);
    SubTreeDump(tree_node->right, dump_file);
    fprintf(dump_file, ")");
}

void TreeDump(Tree *const tree, FILE *dump_file)
{
    ASSERT(tree, return);

    fprintf(dump_file, "\n\n");
    SubTreeDump(tree->root, dump_file);
    fprintf(dump_file, "\n\n");
}


static Node *ReadSubTree(FILE *file, size_t *counter)
{
    char ch = 0;
    fscanf(file, " %c", &ch);

    switch(ch)
    {
        case '(':
        {
            (*counter)++;

            char data[MAX_LEN] = {};

            fscanf(file, " %c", &ch);
            ASSERT(ch == '<', return NULL);

            fscanf(file, " %[^>]%*c", data);

            Node *left  = ReadSubTree(file, counter);
            Node *right = ReadSubTree(file, counter);

            fscanf(file, " %c", &ch);
            ASSERT(ch == ')', free(left );
                              free(right);
                              return NULL);

            return NodeCtor(data, left, right);
        }
        case '*':
        {
            return NULL;
        }
        default:
        {
            LOG("Invalid data.\n");

            return NULL;
        }
    }
}

Tree ReadTree(const char *const path)
{
    FILE *file = fopen(path, "rb");
    ASSERT(file, return {});

    size_t counter = 0;
    Tree tree      = {};

    tree.root = ReadSubTree(file, &counter);
    tree.size = counter;

    fclose(file);

    return tree;
}

#ifdef PROTECT
static void TreeSizeVer(Tree *const tree, Node *const tree_node, size_t *counter)
{
    if(!tree_node || ++(*counter) > tree->size) return;

    TreeSizeVer(tree, tree_node->left , counter);
    TreeSizeVer(tree, tree_node->right, counter);
}

int TreeVer(Tree *const tree)
{
    ASSERT(tree && tree->root    , return EXIT_FAILURE);
    ASSERT(tree->size <= UINT_MAX, return EXIT_FAILURE);

    size_t counter = 0;
    TreeSizeVer(tree, tree->root, &counter);

    ASSERT(counter == tree->size, return EXIT_FAILURE);

    return EXIT_SUCCESS;
}
#endif