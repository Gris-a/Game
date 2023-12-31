#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

#include "log.h"
#include "stack.h"
#include "constants.h"

struct Node
{
    char *data;

    Node *left;
    Node *right;
};

struct Tree
{
    Node *root;

    size_t size;
};

enum PlacePref
{
    LEFT  = -1,
    AUTO  =  0,
    RIGHT =  1
};

#define TREE_DUMP(tree_ptr) LOG("%s:%s:%d:\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                            TreeDump(tree_ptr, __func__, __LINE__);\

#ifdef PROTECT
#define TREE_VERIFICATION(tree_ptr, ret_val_on_fail) if(!IsTreeValid(tree_ptr))\
                                                     {\
                                                         LOG("%s:%s:%d: Error: invalid tree.\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);\
                                                         TREE_DUMP(tree_ptr);\
                                                         return ret_val_on_fail;\
                                                     }
#else
#define TREE_VERIFICATION(tree_ptr, ...)
#endif


Tree TreeCtor(char *const init_val);

int TreeDtor(Tree *tree, Node *root);

Node *AddNode(Tree *tree, Node *tree_node, const char *const val, PlacePref pref = AUTO);

Node *TreeSearchVal(Tree *const tree, const char *const val);

Stack TreePath(Tree *const tree, const char *const val);

Node *TreeSearchParent(Tree *const tree, Node *const search_node);

Node *NodeCtor(const char *const val, Node *const left = NULL, Node *const right = NULL);

int NodeDtor(Node *node);

void TreeTextDump(Tree *const tree, FILE *dump_file = LOG_FILE);

void TreeDot(Tree *const tree, const char *png_file_name);

void TreeDump(Tree *tree, const char *func, const int line);

Tree ReadTree(const char *const file_name);

#ifdef PROTECT
bool IsTreeValid(Tree *const tree);
#endif

#endif //TREE_H