//Add your code here
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


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

void parallel(char* str){
  int count, i;
  pid_t pid;
  char* parsed[1000];
  char* redirect_parsed[1000];

  str[strcspn(str, "\n")] = 0;

  for (count = 0; count < sizeof( *redirect_parsed); count++){
    redirect_parsed[count] = strsep(&str, "&");

    if (redirect_parsed[count] == NULL)
      break;
  }

  for (i = 0; i < count; i++){
    pid = fork();
    printf("%s \n", redirect_parsed[i]);
    if (pid == 0){
      printf("ya forked!"); 
      parse_line(redirect_parsed[i], parsed);
      printf("%s \n", parsed[0]);
      execvp(parsed[0], parsed);
    }
    wait(NULL);
  }
}

int redirect(char* str, char** redirect_parsed){
  int count;

  str[strcspn(str, "\n")] = 0;

  for (count = 0; count < sizeof( *redirect_parsed); count++){
    redirect_parsed[count] = strsep(&str, ">");
    printf("this is the curent %s \n", redirect_parsed[count]);

    if (redirect_parsed[count] == NULL)
      break;
  }
  if(count > 3){
    write(STDERR_FILENO, error_message, strlen(error_message));
  }
  return(count);
}

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

    if(line_buffer[0] != '#'){
      if(strstr(line_buffer, ">")){
        printf("redirect");
      }
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
  int fd;

  while (1){
    printf("wish> ");
    input_size = getline(&line_buffer, &line_buffer_size, stdin);

    char* parsed_args[input_size];
    char* redi_parsed_args[input_size];

    if(strstr(line_buffer, "&")){
      parallel(line_buffer);
    }

    if(strstr(line_buffer, ">")){
      redirect(line_buffer, redi_parsed_args);
      
      printf("%s \n", redi_parsed_args[1]);
      

      if (fork() == 0){

      fd = open(redi_parsed_args[1], O_RDWR | O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
      dup2(fd, 1);
      close(fd);
      
      arg_num = parse_line(redi_parsed_args[0], parsed_args);

      execvp(parsed_args[0], parsed_args);

      continue;
      }

    }


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
      }
    } else if (strcmp(parsed_args[0], "path") == 0){
      exit(0);
    } else {
      // excuting the command 
      execute_arg(parsed_args);
    }
  }
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