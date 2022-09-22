#include <stdio.h>
#include <stdlib.h>
#include "./solver/solver.h"

int main(int argc,char ** argv) {
    if(argc == 2)
        solve(argv[1]);
    return 0;
}
