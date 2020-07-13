#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./src/tables.h"
#include "./src/translate.h"
#include <stdarg.h>

char **makeArgv(int count, ...)
{
    va_list args;
    int i;
    char **argv = malloc((count+1) * sizeof(char*));
    char *temp;
    va_start(args, count);
    for (i = 0; i < count; i++) {
        temp = va_arg(args, char*);
        argv[i] = malloc(sizeof(temp+1));
        strcpy(argv[i],temp);
    }
    argv[i] = NULL;
    va_end(args);
    return argv;
}

char *strip_comments(char *line) {
    int i = 0;

    while(line[i]) {
        if (line[i] == '#') {
          line[i] = '\0';
          return line;
        }
        i++;
    }    
    return line;
}


int main() {
  SymbolTable *table;
  int a, b, c;

  a = 10;
  b = 12;
  c = 133;


  table = create_table(0);
  add_to_table(table, "a", &a);
  add_to_table(table, "b", &b);
  add_to_table(table, "c", &c);


  char **args = makeArgv(3, "8", "$a2", "1");
  
  char s[] = "sltu $s1, $s2, $s3	# some more comments here";
  char *w = strip_comments(s);
  printf("%s\n", w);

  // write_shift(0, stdout, args, 3);
//   write_itype(4, stdout, args, 3);
  // write_jtype(2, stdout, args, 1);

  // printf("%s\n", args[0]);
  
  // write_rtype(0x21, stdout, args, 3);

  // printf("%ld\n", sizeof(uint32_t));
  // write_table(table, stdout);
  // printf("%s %ld\n", "b", get_addr_for_symbol(table, "b"));
  
}