 
#include <stdlib.h>
#include <stdio.h>
 
char c[] = "#include <stdlib.h>%c#include <stdio.h>%cchar c[] = %c%s%c;%c%cint main(int argc, char** argv) { char command_string[50]; char ver_string[12]; int ver_num = atoi(argv[1]) + 1; sprintf(ver_string, %c%cd%c, ver_num); char child_name[18] = %cchild%c; strcat(child_name, ver_string); snprintf(command_string, sizeof(command_string), %cgcc %cs.c -o%cs; ./%cs %cs%c, child_name, child_name, child_name, ver_string); char path[20]; snprintf(path, sizeof(path), %c%cs.c%c, child_name); FILE* child = fopen(path, %cw%c); fprintf(child, c, 10, 10, 34, c, 34, 10, 10, 34, 37, 34, 34, 34, 34, 37, 37, 37, 37, 34, 34, 37, 34, 34, 34); fclose(child); system(command_string); }";
 
int main(int argc, char** argv)
{
   char command_string[50];
   char ver_string[12];
   int ver_num = atoi(argv[1]) + 1;
   sprintf(ver_string, "%d", ver_num);
 
   char child_name[18] = "child";
   strcat(child_name, ver_string);
 
   snprintf(command_string, sizeof(command_string), "gcc %s.c -o%s; ./%s %s", child_name, child_name, child_name, ver_string);
   char path[20];
   snprintf(path, sizeof(path), "%s.c", child_name);
 
   FILE* child = fopen(path, "w");
 
   fprintf(child, c, 10, 10, 34, c, 34, 10, 10, 34, 37, 34, 34, 34, 34, 37, 37, 37, 37, 34, 34, 37, 34, 34, 34);
   fclose(child);
 
   system(command_string);
}
