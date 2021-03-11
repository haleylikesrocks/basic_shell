//Add your code here
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>

int piped(void){
  exit(0);
}

int parse_piped(){
  exit(0);
}

void parse_line(){

}

void execute_arg(char** parsed){
  int flag;
  pid_t pid = fork();

  if (pid < 0){
    printf("\n failed to fork, maybe try a spoon \n")
  } else if (pid == 0){
    flag = execvp(parsed[0], parsed)
    if (flag < 0){
      printf("\n failed to execute, maybe try prison \n")
    }
    exit(0);
  } else{
    wait(NULL);
    return(0);
  }
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