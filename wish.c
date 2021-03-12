//Add your code here
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>


char error_message[30] = "An error has occurred\n";

int parse_line(char* str, char** parsed){
  int count;

  str[strcspn(str, "\n")] = 0;

  for (count = 0; count < sizeof( *parsed); count++){
    parsed[count] = strsep(&str, " ");

    if (parsed[count] == NULL)
      break;
  

    // if (strlen(parsed[count]) == 0){
    //   count--;
    // }
  }
  return(count);
}

void execute_arg(char** parsed){
  int flag;
  pid_t pid = fork();

  if (pid < 0){
    write(STDERR_FILENO, error_message, strlen(error_message));
    printf("there err is in pid < 0");
  } else if (pid == 0){
    flag = execvp(parsed[0], parsed);
    if (flag < 0){
      write(STDERR_FILENO, error_message, strlen(error_message));
      // return;
      // printf("error in flag execvp");
      // exit;
    }
    exit(0);
  } else{
    wait(NULL);
    return;
  }
}

// int built_in(char** parsed){
//   if (strcmp(parsed_args[0], "exit") == 0){
//       exit(0);
//     } else if (strcmp(parsed_args[0], "cd") == 0){
//       if (arg_num != 2){
//         write(STDERR_FILENO, error_message, strlen(error_message));
//         printf("wrong args num\n");
//       } else if (chdir(parsed_args[1]) == -1){
//         write(STDERR_FILENO, error_message, strlen(error_message));
//         printf("invalid cd");
//         // chdir(parsed_args[1]);
//       }
//     } else if (strcmp(parsed_args[0], "path") == 0){
//       return;
//     } 
//     }

//   }

// }

int batch_wish(FILE *fp){
  char *line_buffer = NULL;
  size_t line_buffer_size = 0;
  int input_size = 0;
  int arg_num;

  while (input_size >= 0){

    input_size = getline(&line_buffer, &line_buffer_size, fp);
    if (input_size < 0){
      exit(0);
    }
  
    char* parsed_args[input_size];
    // printf("the line begins with %c", line_buffer[0]);
    // printf("the input size is %d", input_size);

    if(line_buffer[0] != '#'){
      arg_num = parse_line(line_buffer, parsed_args);

      if (strcmp(parsed_args[0], "exit") == 0){
        if (arg_num != 1){
          write(STDERR_FILENO, error_message, strlen(error_message));
          continue;
        } else{
          exit(0);
        }
      } else if (strcmp(parsed_args[0], "cd") == 0){
        if (arg_num != 2){
          write(STDERR_FILENO, error_message, strlen(error_message));
          continue;
        } else if (chdir(parsed_args[1]) == -1){
          write(STDERR_FILENO, error_message, strlen(error_message));
          continue;
        }
      } else {
        execute_arg(parsed_args);
      }
    }

    line_buffer = '\0';
  }

  exit(0);
}

void interactive_wish(void){
  char *line_buffer = NULL;
  size_t line_buffer_size =0;
  int input_size;
  int arg_num;

  while (1){
    printf("wish> ");
    input_size = getline(&line_buffer, &line_buffer_size, stdin);

    char* parsed_args[input_size];

    // parsing the line
    arg_num = parse_line(line_buffer, parsed_args);

    // checking to see if it is a built in command
    if (strcmp(parsed_args[0], "exit") == 0){
      if (arg_num != 1){
        write(STDERR_FILENO, error_message, strlen(error_message));
        continue;
      } else{
        exit(0);
      }
    } else if (strcmp(parsed_args[0], "cd") == 0){
      if (arg_num != 2){
        write(STDERR_FILENO, error_message, strlen(error_message));
      } else if (chdir(parsed_args[1]) == -1){
        write(STDERR_FILENO, error_message, strlen(error_message));
        // chdir(parsed_args[1]);
      }
    } else if (strcmp(parsed_args[0], "path") == 0){
      exit(0);
    } else {
      // excuting the command 
      execute_arg(parsed_args);
    }

  }

    
  // if(piped()){
  //   parse_piped();
  // }
}

int main(int argc, char **argv)
{
  if(argc > 1){
    FILE *fpin;

    fpin = fopen(argv[1], "r");
    if(fpin == NULL){
      write(STDERR_FILENO, error_message, strlen(error_message));
      exit(1);
    }
    if(argc > 2){
      write(STDERR_FILENO, error_message, strlen(error_message));
      exit(1);
    }

    batch_wish(fpin);
  }
  
  interactive_wish();

}