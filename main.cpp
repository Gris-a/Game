#include <stdlib.h>

#include "include/akinator.h"

int main(int argc, char *argv[])
{
    if(argc != 2) return EXIT_FAILURE;

    Akinator(argv[1]);

    return EXIT_SUCCESS;
}