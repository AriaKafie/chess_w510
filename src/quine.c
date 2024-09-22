#include <stdlib.h>
#include <stdio.h>
 
char c[] = "#include <stdlib.h>%c#include <stdio.h>%cchar c[] = %c%s%c;%c%cint main(int argc, char** argv) { char command_string[50] = %cgcc %c; char ver_string[12]; int ver_num = atoi(argv[1])+1; sprintf(ver_string, %c%cd%c, ver_num); char child_name[18] = %cchild%c; char dot_c_tack_o[] = %c.c -o %c; char and_dot_slash[] = %c && ./%c; strcat(child_name, ver_string); strcat(command_string, child_name); strcat(command_string, dot_c_tack_o); strcat(command_string, child_name); strcat(command_string, and_dot_slash); strcat(command_string, child_name); char space[] = %c %c; strcat(command_string, space); strcat(command_string, ver_string); char dot_c[] = %c.c%c; strcat(child_name, dot_c); FILE* child = fopen(child_name, %cw%c); fprintf(child, c, 10, 10, 34, c, 34, 10, 10, 34, 34, 34, 37, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34); system(command_string);}";
 
int main(int argc, char** argv)
{
  char command_string[50] = "gcc ";
  char ver_string[12];
  int ver_num = atoi(argv[1]) + 1;
  sprintf(ver_string, "%d", ver_num);
 
   char child_name[18] = "child";
   char dot_c_tack_o[] = ".c -o ";
   char and_dot_slash[] = " && ./"; 
   strcat(child_name, ver_string); 
 
   strcat(command_string, child_name); 
   strcat(command_string, dot_c_tack_o); 
   strcat(command_string, child_name); 
   strcat(command_string, and_dot_slash); 
   strcat(command_string, child_name); 
 
   char space[] = " "; 
 
   strcat(command_string, space);
   strcat(command_string, ver_string); 
 
   char dot_c[] = ".c";
 
   strcat(child_name, dot_c);
 
   FILE* child = fopen(child_name, "w");
 
   fprintf(child, c, 10, 10, 34, c, 34, 10, 10, 34, 34, 34, 37, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34);
 
   system(command_string);
}
