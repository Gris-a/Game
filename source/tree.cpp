#include <stdio.h>
#include <stdlib.h>
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
    TREE_VERIFICATION(tree, EXIT_FAILURE);

    ASSERT(root, return EXIT_FAILURE);
    ASSERT(root == tree->root || (TreeSearchParent(tree, root) != NULL), return EXIT_FAILURE);

    SubTreeDtor(tree, root->left);
    root->left  = NULL;

    SubTreeDtor(tree, root->right);
    root->right = NULL;

    if(root == tree->root)
    {
        tree->root = NULL;
    }
    else
    {
        Node *parent = TreeSearchParent(tree, root);

        if(parent->left == root) parent->left  = NULL;
        else                     parent->right = NULL;
    }

    free(root);
    tree->size--;

    return EXIT_SUCCESS;
}


Node *AddNode(Tree *tree, Node *tree_node, char *val, PlacePref pref)
{
    TREE_VERIFICATION(tree, NULL);

    ASSERT(val, return NULL);

    ASSERT(tree_node, return NULL);
    ASSERT(tree_node == tree->root || (TreeSearchParent(tree, tree_node) != NULL), return NULL);

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

    (*next) = NodeCtor(val);

    ASSERT((*next), return NULL);

    tree->size++;

    return (*next);
}


static Node *SubTreeSearchVal(Node *const tree_node, char *val)
{
    if(!tree_node) return NULL;
    else if(strncmp(tree_node->data, val, MAX_DATA_LEN) == 0) return tree_node;

    Node *find  = SubTreeSearchVal(tree_node->left , val);

    return (find ? find : SubTreeSearchVal(tree_node->right, val));
}

Node *TreeSearchVal(Tree *const tree, char *val)
{
    TREE_VERIFICATION(tree, NULL);

    ASSERT(val, return NULL);

    return SubTreeSearchVal(tree->root, val);
}


static bool SubTreeValPath(Node *const tree_node, char *const val, Stack *path)
{
    if(!tree_node) return false;
    else if(strncmp(tree_node->data, val, MAX_DATA_LEN) == 0) return true;

    if(SubTreeValPath(tree_node->left, val, path))
    {
        PushStack(path, 0);

        return true;
    }
    else if(SubTreeValPath(tree_node->right, val, path))
    {
        PushStack(path, 1);

        return true;
    }

    return false;
}

Stack TreeValPath(Tree *const tree, char *const val)
{
    TREE_VERIFICATION(tree, {});

    ASSERT(val, return {});

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
    else if(tree_node->left  == search_node ||
            tree_node->right == search_node) return tree_node;

    Node *find  = SubTreeSearchParent(tree_node->left , search_node);

    return (find ? find : SubTreeSearchParent(tree_node->right, search_node));
}

Node *TreeSearchParent(Tree *const tree, Node *const search_node)
{
    TREE_VERIFICATION(tree, NULL);

    ASSERT(search_node, return NULL);

    return SubTreeSearchParent(tree->root, search_node);
}


Node *NodeCtor(char *const val, Node *const left, Node *const right)
{
    ASSERT(val, return NULL);

    Node *node = (Node *)calloc(1, sizeof(Node));

    ASSERT(node, return NULL);

    strncpy(node->data, val, MAX_DATA_LEN - 1);
    node->left  = left;
    node->right = right;

    return node;
}


static void MakeDumpDir(void)
{
    system("rm -rf dump_tree");
    system("mkdir dump_tree");
}


static void SubTreeTextDump(Node *const tree_node, FILE *dump_file)
{
    if(!tree_node) {fprintf(dump_file, "*"); return;}

    fprintf(dump_file, "\n\t(");

    fprintf(dump_file, "<%s>", tree_node->data);

    SubTreeTextDump(tree_node->left , dump_file);
    SubTreeTextDump(tree_node->right, dump_file);

    fprintf(dump_file, ")");
}

void TreeTextDump(Tree *const tree, FILE *dump_file)
{
    ASSERT(tree     , return);
    ASSERT(dump_file, return);

    if(dump_file == LOG_FILE)
    {
        fprintf(dump_file, "TREE[%p]:  \n"
                           "\troot: %p \n"
                           "\tsize: %zu\n", tree, tree->root, tree->size);
    }

    if(!tree->root) return;

    SubTreeTextDump(tree->root, dump_file);
    fprintf(dump_file, "\n\n");
}


static void DotTreeCtor(Node *const node, Node *const node_next, const char *direction, FILE *file)
{
    if(!node_next) return;

    fprintf(file, "node%p[label = \"{<data> data: %s | {<left> l: %p| <right> r: %p}}\"];\n",
                                    node_next, node_next->data, node_next->left, node_next->right);

    fprintf(file, "node%p:<%s>:s -> node%p:<data>:n;\n", node, direction, node_next);

    DotTreeCtor(node_next, node_next->left , "left" , file);
    DotTreeCtor(node_next, node_next->right, "right", file);
}

void TreeDot(Tree *const tree, const char *png_file_name)
{
    ASSERT(tree && tree->root, return);
    ASSERT(png_file_name     , return);

    FILE *dot_file = fopen("tree.dot", "wb");
    ASSERT(dot_file, return);

    fprintf(dot_file, "digraph\n"
                  "{\n"
                  "bgcolor = \"grey\";\n"
                  "ranksep = \"equally\";\n"
                  "node[shape = \"Mrecord\"; style = \"filled\"; fillcolor = \"#58CD36\"];\n"
                  "{rank = source;");
    fprintf(dot_file, "nodel[label = \"<root> root: %p | <size> size: %zu\"; fillcolor = \"lightblue\"];", tree->root, tree->size);

    fprintf(dot_file, "node%p[label = \"{<data> %s | {<left> Nein | <right> Ja}}\"; fillcolor = \"orchid\"]};\n",
                                                                                    tree->root, tree->root->data);
    DotTreeCtor(tree->root, tree->root->left , "left" , dot_file);
    DotTreeCtor(tree->root, tree->root->right, "right", dot_file);

    fprintf(dot_file, "}\n");

    fclose(dot_file);

    char sys_cmd[MAX_MESSAGE_LEN] = {};
    snprintf(sys_cmd, MAX_MESSAGE_LEN - 1, "dot tree.dot -T png -o %s", png_file_name);
    system(sys_cmd);

    remove("tree.dot");
}


void TreeDump(Tree *tree, const char *func, const int line)
{
    static int num = 0;

    ASSERT(tree, return);

    if(num == 0) MakeDumpDir();

    char file_name[MAX_FILE_NAME_LEN] = {};

    TreeTextDump(tree);

    sprintf(file_name, "dump_tree/tree_dump%d__%s:%d__.png", num, func, line);
    TreeDot(tree, file_name);

    num++;
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

            char data[MAX_DATA_LEN] = {};

            fscanf(file, " %c", &ch);

            ASSERT(ch == '<', return NULL);

            bool is_scaned = fscanf(file, " %[^>]%*c", data);

            if(!is_scaned)
            {
                LOG("Invalid data.\n");

                return NULL;
            }

            Node *left  = ReadSubTree(file, counter);
            Node *right = ReadSubTree(file, counter);

            fscanf(file, " %c", &ch);

            if(ch != ')')
            {
                LOG("Invalid data.\n");

                free(left );
                free(right);

                return NULL;
            }

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

Tree ReadTree(const char *const file_name)
{
    FILE *file = fopen(file_name, "rb");
    ASSERT(file, return {});

    size_t counter = 0;
    Tree tree      = {};

    tree.root = ReadSubTree(file, &counter);
    tree.size = counter;

    fclose(file);

    return tree;
}

#ifdef PROTECT
static void TreeSizeValidation(Tree *const tree, Node *const tree_node, size_t *counter)
{
    if(!tree_node || (*counter) >= tree->size) return;

    (*counter)++;

    TreeSizeValidation(tree, tree_node->left , counter);
    TreeSizeValidation(tree, tree_node->right, counter);
}

bool IsTreeValid(Tree *const tree)
{
    ASSERT(tree && tree->root    , return false);
    ASSERT(tree->size <= UINT_MAX, return false);

    size_t counter = 0;
    TreeSizeValidation(tree, tree->root, &counter);

    ASSERT(counter == tree->size, return false);

    return true;
}
#endif