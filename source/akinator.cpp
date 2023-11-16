#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../include/akinator.h"
#include "../include/tree.h"

static bool ProcessingYesNoAnswer(void)
{
    char ans[MAX_LEN] = {};

    while(true)
    {
        scanf("%s", ans);
        if(ans[1] != '\0') {printf("Try again.\n"); continue;}

        switch(tolower(ans[0]))
        {
            case 'y':
            {
                return true;
            }
            case 'n':
            {
                return false;
            }
            default:
            {
                printf("Try again.\n");

                continue;
            }
        }
    }
}

static void Quit(const char *const data_base, Tree *tree)
{
    printf("Do you want to save your progress?[Y/n]: ");
    if(ProcessingYesNoAnswer())
    {
        FILE *db_file = fopen(data_base, "wb");
        ASSERT(db_file, return);

        TreeDump(tree, db_file);
        fclose(db_file);
    }

    return;
}


static void ShowTree(Tree *tree)
{
    TreeDot(tree, "data/tree.dot");

    system("xdg-open data/tree.png");
    system("clear");
}


static void Game(Tree *tree)
{
    Node *cur_pos = tree->root;
    while(cur_pos->left != NULL)
    {
        printf("%s?[Y/n]: ", cur_pos->data);
        if(ProcessingYesNoAnswer())
        {
            cur_pos = cur_pos->right;
        }
        else
        {
            cur_pos = cur_pos->left;
        }
    }

    printf("Is \'%s\' the correct answer?[Y/n]: ", cur_pos->data);
    if(ProcessingYesNoAnswer())
    {
        printf("GG.\n");
    }
    else
    {
        char ans[MAX_LEN] = {};
        printf("What is correct answer then?\n");
        scanf(" %[^\n]", ans);

        EXEC_ASSERT(AddNode(tree, cur_pos, cur_pos->data, LEFT), return);
        EXEC_ASSERT(AddNode(tree, cur_pos, ans, RIGHT)         , return);

        printf("what property distinguishes \'%s\' from \'%s\'?\n", ans, cur_pos->data);
        scanf(" %[^\n]", ans);

        strcpy(cur_pos->data, ans);
    }

    return;
}


static void PropertiesDump(Node *tree_pos, Stack *path)
{
    data_t path_dir = 0;

    while(path->size)
    {
        PopStack(path, &path_dir);

        if(path_dir)
        {
            printf("\t\033[1;32m%s\033[0m\n", tree_pos->data);

            tree_pos = tree_pos->right;
        }
        else
        {
            printf("\t\033[1;31m%s\033[0m\n", tree_pos->data);

            tree_pos = tree_pos->left;
        }
    }
}

static void Definition(Tree *tree)
{
    printf("Definition of: ");

    char str[MAX_LEN] = {};
    scanf(" %[^\n]", str);

    Stack path = TreeValPath(tree, str);
    if(!path.data)
    {
        printf("There is no %s in data base.\n", str);

        return;
    }

    PropertiesDump(tree->root, &path);
    printf(" - this is \'%s\'.\n", str);

    StackDtor(&path);
}

static Node *SimilarPropertiesDump(Node *tree_pos, Stack *path1, Stack *path2)
{
    data_t path1_dir = 0;
    data_t path2_dir = 0;

    while(path1->size && path2->size)
    {
        PopStack(path1, &path1_dir);
        PopStack(path2, &path2_dir);

        if(path1_dir && path2_dir)
        {
            printf("\033[1;32m%s,\033[0m ", tree_pos->data);

            tree_pos = tree_pos->right;
        }
        else if(!(path1_dir || path2_dir))
        {
            printf("\033[1;31m%s,\033[0m ", tree_pos->data);

            tree_pos = tree_pos->left;
        }
        else
        {
            PushStack(path1, path1_dir);
            PushStack(path2, path2_dir);

            break;
        }
    }

    return tree_pos;
}

static void Compare(Tree *tree)
{
    printf(" First to compare: ");

    char str1[MAX_LEN] = {};
    scanf(" %[^\n]", str1);

    Stack path1 = TreeValPath(tree, str1);
    if(!path1.data)
    {
        printf("There is no %s in data base.\n", str1);

        return;
    }

    printf("Second to compare: ");

    char str2[MAX_LEN] = {};
    scanf(" %[^\n]", str2);

    Stack path2 = TreeValPath(tree, str2);
    if(!path2.data)
    {
        printf("There is no %s in data base.\n", str2);

        StackDtor(&path1);

        return;
    }

    Node  *tree_pos = SimilarPropertiesDump(tree->root, &path1, &path2);
    if(tree_pos != tree->root) printf(" - similarities of \'%s\' и \'%s\'.\n", str1, str2);

    printf("\n'%s':\n", str1);
    PropertiesDump(tree_pos, &path1);

    printf("\n'%s':\n", str2);
    PropertiesDump(tree_pos, &path2);

    StackDtor(&path1);
    StackDtor(&path2);
}


void Akinator(const char *const data_base)
{
    ASSERT(data_base, return);

    Tree tree = ReadTree(data_base);
    ASSERT(tree.root, return);

    char ans[MAX_LEN] = {};

    while(true)
    {
        printf("[G] - Guess, [T] - Tree, [D] - Definition, [C] - compare, [Q] - Quit\n");

        scanf("%s", ans);
        if(ans[1] != '\0') {printf("Try again.\n"); continue;}

        switch(tolower(ans[0]))
        {
            case 'g':
            {
                Game(&tree);

                continue;
            }
            case 't':
            {
                ShowTree(&tree);

                continue;
            }
            case 'd':
            {
                Definition(&tree);

                continue;
            }
            case 'c':
            {
                Compare(&tree);

                continue;
            }
            case 'q':
            {
                Quit(data_base, &tree);

                break;
            }
            default:
            {
                printf("Try again.\n");

                continue;
            }
        }

        break;
    }

    TreeDtor(&tree, tree.root);
}