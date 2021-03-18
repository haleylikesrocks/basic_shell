//Add your code here
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

char error_message[30] = "An error has occurred\n";

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 2);
    strcpy(result, s1);
    strcat(result, "/");
    result[strcspn(result, "\0")] = 0;
    strcat(result, s2);
    return result;
}

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

char* check_path(char* path, char* cmd){
  char* result;

  path[strcspn(path, "\n")] = 0;

  while(1){
    result = strsep(&path, " ");

    if (result == NULL){
      break;
    }
    if (strlen(result) == 0){
      continue;
    }

    if(access(concat(result, cmd), X_OK) == 0){
      return concat(result, cmd);
    } else{
      continue;
    }

    result = "\0";
  } 

  return (cmd); 
}

int parse_line(char* str, char** parsed){
  int count;

  for (count = 0; count < 1000; count++){
    parsed[count] = strsep(&str, " ");

    if (parsed[count] == NULL)
      break;

    if (strlen(parsed[count]) == 0){
      count--;
    }
  }
  return(count);
}

void execute_arg(char** parsed, char* path){
  int flag;
  pid_t pid = fork();

  char* working_path = check_path(path, parsed[0]);
  printf("the working path is now %s\n", working_path);

  if (pid < 0){
    write(STDERR_FILENO, error_message, strlen(error_message));
    // printf("its in the pid\n");
  } else if (pid == 0){
    flag = execv(working_path, parsed);
    if (flag < 0){
      write(STDERR_FILENO, error_message, strlen(error_message));
      printf("its in the flag\n");
    } else if(flag == 0){
      wait(NULL);
      return;
      exit(0);
    }
  } 
  return;
}

void parallel(char* str){
  int count, i;
  pid_t pid;
  char* parsed[1000];
  char* redirect_parsed[1000];
  int status;

  str[strcspn(str, "\n")] = 0;

  for (count = 0; count < 1000; count++){
    redirect_parsed[count] = strsep(&str, "&");
    
    if (redirect_parsed[count] == NULL)
      break;
  }

  for (i = 0; i < count; i++){
    pid = fork();

    if (pid == 0){
      parse_line(redirect_parsed[i], parsed);
      execvp(parsed[0], parsed);
    }
  }
  while (count > 0){
    waitpid(-1, &status, 0);

    count--;
  }
  return;
}

int redirect(char* str, char** redirect_parsed, char ** parsed_args, char* path){
  int count, fd, saved_stdout;

  str[strcspn(str, "\n")] = 0;
  // printf("currently working on %s\n", str);

  for (count = 0; count < 1000; count++){
    redirect_parsed[count] = strsep(&str, ">");

    if (redirect_parsed[count] == NULL)
      break;

    if (strlen(redirect_parsed[count]) == 0){
      count--;
    }
  }
  if(count != 2){
    write(STDERR_FILENO, error_message, strlen(error_message));
    return (-1);
  }
  if(strstr(trimwhitespace(redirect_parsed[1]), " ")){
    write(STDERR_FILENO, error_message, strlen(error_message));
    return (-1);
  }

  if (fork() == 0){

    fd = open(trimwhitespace(redirect_parsed[1]), O_WRONLY | O_CREAT | O_TRUNC, 0666); // opening file
    
    if(fd < 0){ // checking for open file error
     write(STDERR_FILENO, error_message, strlen(error_message));
     return(-1);
    }
    saved_stdout = dup(1);
    dup2(fd, 1);
    close(fd);
      
  parse_line(redirect_parsed[0], parsed_args);

  execute_arg(parsed_args, path);

  dup2(saved_stdout, 1);
  close(saved_stdout);

  return (0);
}

return (-1);
}

void run_command(char * str, int size, char* path){
  char* parsed_args[size];
  char* redi_parsed_args[size];
  int arg_num, parallel_flag = 0, redirect_flag = 0;

  char* input;

  str[strcspn(str, "\n")] = 0;

  input = (char*)malloc(1000*sizeof(char));
  input = strcpy(input, str);

  if(strstr(str, "&")){ //check for parallel
    parallel(str);
    parallel_flag = 1;
    return;
  } 
  else if(strstr(str, ">")){ // check for redirection
    redirect(str, redi_parsed_args, parsed_args, path);
    redirect_flag = 1;
    return;
  } 
  else{ // no redirect or parallel flags
    arg_num = parse_line(str, parsed_args);

    // checking to see if it is a built in command
    if (strcmp(parsed_args[0], "exit") == 0){
      if (arg_num != 1){
        write(STDERR_FILENO, error_message, strlen(error_message));
      }
      exit(0);
    } 
    else if (strcmp(parsed_args[0], "cd") == 0){
      if (arg_num != 2){
        write(STDERR_FILENO, error_message, strlen(error_message));
      } else if (chdir(parsed_args[1]) == -1){
        write(STDERR_FILENO, error_message, strlen(error_message));
      }
      return;
    } 
    else if (strcmp(parsed_args[0], "path") == 0){
      strcpy(path, input);
      return;
    } 
  }
    

  if (parallel_flag == 0 && redirect_flag == 0){
    execute_arg(parsed_args, path);
  }
}

int batch_wish(FILE *fp){
  char *line_buffer = NULL;
  size_t line_buffer_size = 0;
  int input_size = 0;
  char* path;


  path = (char*)malloc(1000*sizeof(char));
  path = strcpy(path, "/bin");

  while (1){

    input_size = getline(&line_buffer, &line_buffer_size, fp);
    if (input_size < 0){
      exit(0);
    }
    
    run_command(line_buffer, input_size, path);
    wait(NULL);
  }

  exit(0);
}

void interactive_wish(void){
  char *line_buffer = NULL;
  size_t line_buffer_size =0;
  int input_size;
  char* path;

  path = (char*)malloc(1000*sizeof(char));
  path = strcpy(path, "/bin");

  while (1){
    printf("wish> ");
    input_size = getline(&line_buffer, &line_buffer_size, stdin);
    if(input_size == 1){
      continue;
    }
    run_command(line_buffer, input_size, path);
    wait(NULL);
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
    
    exit(0);

  }
  interactive_wish();
  
  exit(0);

}