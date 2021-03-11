//Add your code here
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int piped(void){
  exit(0);
}

int parse_piped(){
  exit(0);
}

void parse_line(){

}

int execute_arg(void){
  return 0;
}

int batch_wish(void){
  exit(0);
}

void interactive_wish(void){
  char *line_buffer = NULL;
  size_t line_buffer_size =0;
  // ssize_t line_size = 0;
  char *target = "exit";
  char *found;
  

  // geting the user input
  printf("wish> ");
  getline(&line_buffer, &line_buffer_size, stdin);
  while (strcmp(line_buffer, target) != 10){
    printf("%d", strcmp(line_buffer, target));

    // parsing the line
    while( (found = strsep(&line_buffer," ")) != NULL ){
    printf("%s\n",found);
        }

    // parse_line(line_buffer);
    // execute_arg();
    printf("wish> ");
    getline(&line_buffer, &line_buffer_size, stdin);
  }

    
  // if(piped()){
  //   parse_piped();
  // }
}

int main(int argc, char **argv)
{
  if(argc > 1){
    batch_wish();
  }
  
  interactive_wish();

}