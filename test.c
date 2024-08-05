#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char * argv []) {
    char * test = malloc (30);
    strcpy (test, " ");

test [0] = '1';
    printf ("%d\n", test [0]);
}
