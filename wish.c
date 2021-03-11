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

void parse_line(char* str, char** parsed){
  int count;

  for (count = 0; count < sizeof( *parsed); count++){
    parsed[count] = strsep(&str, " ");

    if (parsed[count] == NULL)
      break;
  

    // if (strlen(parsed[count]) == 0){
    //   count--;
    // }
  }
}

void execute_arg(char** parsed){
  int flag;
  pid_t pid = fork();

  if (pid < 0){
    printf("\n failed to fork, maybe try a spoon \n");
  } else if (pid == 0){
    flag = execvp(parsed[0], parsed);
    if (flag < 0){
      printf("\n failed to execute, maybe try prison \n");
    }
    exit(0);
  } else{
    wait(NULL);
    return;
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
  int input_size;
  

  // geting the user input
  printf("wish> ");
  input_size = getline(&line_buffer, &line_buffer_size, stdin);

  char* parsed_args[input_size];

  while (strcmp(line_buffer, target) != 10){
    // parsing the line
    line_buffer[strcspn(line_buffer, "\n")] = 0;
    parse_line(line_buffer, parsed_args);
    execute_arg(parsed_args);
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