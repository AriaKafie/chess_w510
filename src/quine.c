#include <stdlib.h>
#include <stdio.h>
#define path "child.c"
#define write "w"
#define compile_and_run "gcc child.c && ./a.out"
 
const char* c = "#include <stdlib.h>%c#include <stdio.h>%c#define path %cchild.c%c%c#define write %cw%c%c#define compile_and_run %cgcc child.c && ./a.out%c%cconst char* c = %c%s%c;int main() { FILE* child = fopen(path, write);fprintf(child, c, 10, 10, 34, 34, 10, 34, 34, 10, 34, 34, 10, 34, c, 34);}";
 
int main() {
 
    FILE* child = fopen(path, write);
 
    fprintf(child, c, 10, 10, 34, 34, 10, 34, 34, 10, 34, 34, 10, 34, c, 34);
 
    system(compile_and_run);
}
