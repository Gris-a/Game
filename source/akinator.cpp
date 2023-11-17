#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../include/akinator.h"
#include "../include/tree.h"

static bool ProcessingYesNoAnswer(const char *message)
{
    char ans[MAX_LEN] = {};

    while(true)
    {
        printf("%s", message);
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


static void Quit(Tree *tree)
{
    if(ProcessingYesNoAnswer("Do you want to save your progress?[Y/n]: "))
    {
        char name[MAX_LEN] = {};
        time_t cur_time = time(NULL);

        sprintf(name, "data/saved/data_%s", ctime(&cur_time));
        name[strlen(name) - 1] = '\0';
        strcat(name, ".txt");

        FILE *db_file = fopen(name, "wb");
        ASSERT(db_file, return);

        TreeTextDump(tree, db_file);
        fclose(db_file);
    }
}


static void ShowTree(Tree *tree)
{
    TreeDot(tree, "data/tree.png");

    system("xdg-open data/tree.png");
    system("clear");
}


static Node *GetAnswer(Tree *const tree)
{
    char message[MAX_LEN * 2] = {};

    Node *cur_pos = tree->root;
    while(cur_pos->right != NULL)
    {
        sprintf(message, "%s?[Y/n]: ", cur_pos->data);
        if(ProcessingYesNoAnswer(message))
        {
            cur_pos = cur_pos->right;
        }
        else
        {
            cur_pos = cur_pos->left;
        }
    }

    return cur_pos;
}

static void AddAnswer(Tree *tree, Node *prev_answer)
{
    char ans[MAX_LEN] = {};

    printf("What is correct answer then?\n");
    scanf(" %[^\n]", ans);

    EXEC_ASSERT(AddNode(tree, prev_answer, prev_answer->data, LEFT ), return);
    EXEC_ASSERT(AddNode(tree, prev_answer, ans              , RIGHT), return);

    printf("what property distinguishes \'%s\' from \'%s\'?\n", ans, prev_answer->data);
    scanf(" %[^\n]", ans);

    strncpy(prev_answer->data, ans, MAX_LEN - 1);
}

static void Game(Tree *tree)
{
    char message[MAX_LEN * 2] = {};

    Node *answer = GetAnswer(tree);

    sprintf(message, "Is \'%s\' the correct answer?[Y/n]: ", answer->data);
    if(ProcessingYesNoAnswer(message))
    {
        printf("GG.\n");
    }
    else
    {
        AddAnswer(tree, answer);
    }
}


static void PropertiesDump(Node *tree_pos, Stack *path)
{
    data_t path_dir = 0;

    while(path->size)
    {
        PopStack(path, &path_dir);

        if(path_dir)
        {
            printf(color_green("\t%s\n"), tree_pos->data);

            tree_pos = tree_pos->right;
        }
        else
        {
            printf(color_red("\t%s\n"), tree_pos->data);

            tree_pos = tree_pos->left;
        }
    }
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
            printf(color_green("%s, "), tree_pos->data);

            tree_pos = tree_pos->right;
        }
        else if(!(path1_dir || path2_dir))
        {
            printf(color_red("%s, "), tree_pos->data);

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

    Node *tree_pos = SimilarPropertiesDump(tree->root, &path1, &path2);
    if(tree_pos != tree->root)
    {
        printf(" - similarities of \'%s\' и \'%s\'.\n", str1, str2);
    }

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

    system("mkdir data");
    system("mkdir data/saved");
    system("clear");

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
                Quit(&tree);

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