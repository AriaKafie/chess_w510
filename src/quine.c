 
#include <stdlib.h>
#include <stdio.h>
 
char c[] = "#include <stdlib.h>%c#include <stdio.h>%cchar c[] = %c%s%c;int main(int argc, char** argv) { int version = atoi(argv[1]) + 1; char path[20]; snprintf(path, sizeof(path), %cchild%cd.c%c, version); char command_string[100]; snprintf(command_string, sizeof(command_string), %cgcc %cs -ochild%cd; ./child%cd %cd%c, path, version, version, version); FILE* child = fopen(path, %cw%c); fprintf(child, c, 10, 10, 34, c, 34, 34, 37, 34, 34, 37, 37, 37, 37, 34, 34, 34); fclose(child); system(command_string); }";
 
int main(int argc, char** argv)
{
   int version = atoi(argv[1]) + 1;
 
   char path[20];
   snprintf(path, sizeof(path), "child%d.c", version);
 
   char command_string[100];
   snprintf(command_string, sizeof(command_string), "gcc %s -ochild%d; ./child%d %d", path, version, version, version);
 
   FILE* child = fopen(path, "w");
 
   fprintf(child, c, 10, 10, 34, c, 34, 34, 37, 34, 34, 37, 37, 37, 37, 34, 34, 34);
   fclose(child);
 
   system(command_string);
}
