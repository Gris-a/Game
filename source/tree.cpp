#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>

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

    NodeDtor(sub_tree);

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

    NodeDtor(root);

    tree->size--;

    return EXIT_SUCCESS;
}


Node *AddNode(Tree *tree, Node *tree_node, const char *const val, PlacePref pref)
{
    TREE_VERIFICATION(tree, NULL);

    ASSERT(val, return NULL);
    ASSERT(tree_node == tree->root || (TreeSearchParent(tree, tree_node) != NULL), return NULL);

    Node **next = &tree_node;
    while(*next)
    {
        switch(pref)
        {
            case LEFT:
                next = &((*next)->left);
                break;
            case RIGHT:
                next = &((*next)->right);
                break;
            case AUTO:
                if(strcmp(val, (*next)->data) <= 0) next = &((*next)->left );
                else                                next = &((*next)->right);
                break;
            default: return NULL;
        }
    }

    (*next) = NodeCtor(val);
    ASSERT((*next), return NULL);

    tree->size++;

    return (*next);
}


Node *NodeCtor(const char *const val, Node *const left, Node *const right)
{
    ASSERT(val, return NULL);

    Node *node = (Node *)calloc(1, sizeof(Node));
    ASSERT(node, return NULL);

    node->data = strndup(val, MAX_DATA_LEN - 1);
    ASSERT(node->data, free(node); return NULL);

    node->left  = left;
    node->right = right;

    return node;
}

int NodeDtor(Node *node)
{
    ASSERT(node, return EXIT_FAILURE);

    free(node->data);
    free(node);

    return EXIT_SUCCESS;
}


static Node *SubTreeSearchVal(Node *const tree_node, const char *const val)
{
    if(!tree_node) return NULL;
    else if(strncmp(tree_node->data, val, MAX_DATA_LEN) == 0) return tree_node;

    Node *find  = SubTreeSearchVal(tree_node->left , val);

    return (find ? find : SubTreeSearchVal(tree_node->right, val));
}

Node *TreeSearchVal(Tree *const tree, const char *const val)
{
    TREE_VERIFICATION(tree, NULL);

    ASSERT(val, return NULL);

    return SubTreeSearchVal(tree->root, val);
}


static bool SubTreePath(Node *const tree_node, const char *const val, Stack *path)
{
    if(!tree_node) return false;
    else if(strncmp(tree_node->data, val, MAX_DATA_LEN - 1) == 0) return true;

    if(SubTreePath(tree_node->left, val, path))
    {
        PushStack(path, 0);

        return true;
    }
    else if(SubTreePath(tree_node->right, val, path))
    {
        PushStack(path, 1);

        return true;
    }

    return false;
}

Stack TreePath(Tree *const tree, const char *const val)
{
    TREE_VERIFICATION(tree, {});

    ASSERT(val, return {});

    Stack path = StackCtor();

    ASSERT(path.data, return {});

    if(!SubTreePath(tree->root, val, &path))
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


static Node *ReadSubTree(char *buf, size_t *counter)
{
    static char *buffer = buf;
    static int offset   = 0;
    static char ch      = 0;

    sscanf(buffer, " %c%n", &ch, &offset);
    buffer += offset;

    switch(ch)
    {
        case '(':
        {
            char data[MAX_DATA_LEN] = {};

            sscanf(buffer, " %c%n", &ch, &offset);
            buffer += offset;

            if(ch != '<')
            {
                LOG("Invalid data.\n");
                return NULL;
            }

            bool is_scaned = sscanf(buffer, " %[^>]%*c%n", data, &offset);
            buffer += offset;

            if(!is_scaned)
            {
                LOG("Invalid data.\n");
                return NULL;
            }

            Node *left  = ReadSubTree(buffer, counter);
            Node *right = ReadSubTree(buffer, counter);

            sscanf(buffer, " %c%n", &ch, &offset);
            buffer += offset;

            if(ch != ')')
            {
                LOG("Invalid data.\n");

                NodeDtor(left );
                NodeDtor(right);

                return NULL;
            }

            (*counter)++;

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

static size_t FileSize(const char *const file_name)
{
    struct stat file_info = {};
    stat(file_name, &file_info);

    return (size_t)file_info.st_size;
}

Tree ReadTree(const char *const file_name)
{
    ASSERT(file_name, return {});

    FILE *file = fopen(file_name, "rb");
    if(!file)
    {
        LOG("No such file: \"%s\"", file_name);
        return {};
    }

    size_t buf_size = FileSize(file_name);
    char *buffer = (char *)calloc(buf_size + 1, sizeof(char));
    ASSERT(buffer, fclose(file); return {});

    fread(buffer, buf_size, sizeof(char), file);
    fclose(file);

    size_t counter = 0;
    Tree tree      = {};

    tree.root = ReadSubTree(buffer, &counter);
    tree.size = counter;

    free(buffer);

    return tree;
}


static void SubTreeTextDump(Node *const tree_node, FILE *dump_file)
{
    if(!tree_node) {fputc('*', dump_file); return;}

    fprintf(dump_file, "\n\t(");

    fprintf(dump_file, "<%s>", tree_node->data);

    SubTreeTextDump(tree_node->left , dump_file);
    SubTreeTextDump(tree_node->right, dump_file);

    fputc(')', dump_file);
}

void TreeTextDump(Tree *const tree, FILE *dump_file)
{
    ASSERT(dump_file, return);

    fprintf(dump_file, "TREE[%p]:\n", tree);

    if(!tree) return;

    if(dump_file == LOG_FILE)
    {
        LOG("\troot: %p \n"
            "\tsize: %zu\n", tree->root, tree->size);
    }

    if(!tree->root) return;

    SubTreeTextDump(tree->root, dump_file);
    fputc('\n', dump_file);
}


static void DotTreeCtor(Node *const node, Node *const node_next, const char *direction, FILE *file)
{
    if(!node_next) return;

    fprintf(file, "node%p[label = \"{<data> data: %s | {<left> NO | <right> YES}}\"];\n",
                                                             node_next, node_next->data);

    fprintf(file, "node%p:<%s>:s -> node%p:<data>:n;\n", node, direction, node_next);

    DotTreeCtor(node_next, node_next->left , "left" , file);
    DotTreeCtor(node_next, node_next->right, "right", file);
}

static void TreeDotGeneral(Tree *const tree, FILE *dot_file)
{
    fprintf(dot_file, "digraph\n"
                      "{\n"
                      "bgcolor = \"grey\";\n"
                      "ranksep = \"equally\";\n"
                      "node[shape = \"Mrecord\"; style = \"filled\"; fillcolor = \"#58CD36\"];\n"
                      "{rank = source;");
    fprintf(dot_file, "nodel[label = \"<root> root: %p | <size> size: %zu\"; fillcolor = \"lightblue\"];",
                                                                                  tree->root, tree->size);

    fprintf(dot_file, "node%p[label = \"{<data> %s | {<left> No | <right> Yes}}\"; fillcolor = \"orchid\"]};\n",
                                                                                  tree->root, tree->root->data);
}

void TreeDot(Tree *const tree, const char *png_file_name)
{
    ASSERT(png_file_name, return);

    if(!(tree && tree->root)) return;

    FILE *dot_file = fopen("tree.dot", "wb");
    ASSERT(dot_file, return);

    TreeDotGeneral(tree, dot_file);
    DotTreeCtor(tree->root, tree->root->left , "left" , dot_file);
    DotTreeCtor(tree->root, tree->root->right, "right", dot_file);

    fprintf(dot_file, "}\n");

    fclose(dot_file);

    char sys_cmd[MAX_STR_LEN] = {};
    sprintf(sys_cmd, "dot tree.dot -T png -o %s", png_file_name);
    system(sys_cmd);

    remove("tree.dot");
}


static void MakeDumpDir(void)
{
    system("rm -rf dump_tree");
    system("mkdir dump_tree");
}

void TreeDump(Tree *tree, const char *func, const int line)
{
    static int num = 0;

    if(num == 0) MakeDumpDir();

    char file_name[MAX_STR_LEN] = {};

    TreeTextDump(tree);

    sprintf(file_name, "dump_tree/tree_dump%d__%s:%d__.png", num, func, line);
    TreeDot(tree, file_name);

    num++;
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
    ASSERT(tree && tree->root   , return false);
    ASSERT(tree->size <= INT_MAX, return false);

    size_t counter = 0;
    TreeSizeValidation(tree, tree->root, &counter);

    ASSERT(counter == tree->size, return false);

    return true;
}
#endif