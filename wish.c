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
int run = 0;

char* concat(const char *s1, const char *s2)


{
  // printf("Im in concat\n");
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
// printf("Im in time white space\n");
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
  // printf("Im in chckeck %s\n", path);
  path[strcspn(path, "\n")] = 0;

  char* input;

  input = (char*)malloc(1000*sizeof(char));
  input = strcpy(input, path);
  // printf("the path is %s\n", path);

  while(1){
    result = strsep(&input, " ");
    // printf("Im in chckeck %s\n", path);
    if (result == NULL){
      break;
    }
    if (strlen(result) == 0){
      continue;
    }
    // printf("the path is %s\n", path);
    
    if(access(concat(result, cmd), X_OK) == 0){
      return concat(result, cmd);
    } else{
      continue;
    }

    result = "\0";
  } 
  // path = strcpy(path, input);
  // printf("the path is %s\n", path);
  return (cmd); 
}

int parse_line(char* str, char** parsed){
  int count;
  // printf("Im in parse line\n");
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

  // printf("Im in execut arg\n");
  // printf("the working path is now %s\n", path);

  char* working_path = check_path(path, parsed[0]);
  // printf("the working path is now %s\n", working_path);

  if (pid < 0){
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(0);
    // printf("its in the pid\n");
  } else if (pid == 0){
    flag = execv(working_path, parsed);
    // printf("the working path: %s \n", working_path);
    if (flag < 0){
      write(STDERR_FILENO, error_message, strlen(error_message));
      
      // printf("its in the flag\n");
      exit(0);
    } else if(flag == 0){
      wait(NULL);
      return;
    }
  } 
  return;
}

void redi_execute_arg(char** parsed, char* path){
  int flag;
  pid_t pid = fork();
  // printf("the pid is %d\n", pid);

  // printf("Im in execut arg\n");
  // printf("the working path is now %s\n", path);

  char* working_path = check_path(path, parsed[0]);
  // printf("the working path is now %s\n", working_path);

  if (pid < 0){
    write(STDERR_FILENO, error_message, strlen(error_message));
    exit(0);
    // printf("its in the pid\n");
  } else if (pid == 0){
    flag = execv(working_path, parsed);
    // printf("the working path: %s \n", working_path);
    if (flag < 0){
      write(STDERR_FILENO, error_message, strlen(error_message));
      
      // printf("its in the flag\n");
      exit(0);
    } else if(flag == 0){
      wait(NULL);
      return;
    }
  } 
  return;
}


void parallel(char* str, char* path){
  int count, i, flag;
  pid_t pid, wpid;
  char* parsed[1000];
  char* redirect_parsed[1000];
  int status, redi_flag = 0, saved_stdout, fd, redi_count;
  char* redi_parsed[1000];

  str[strcspn(str, "\n")] = 0;

  for (count = 0; count < 1000; count++){
    redirect_parsed[count] = strsep(&str, "&");
    
    if (redirect_parsed[count] == NULL)
      break;
  }

  for (i = 0; i < count; i++){
    pid = fork();

    if (pid == 0){
      if(strstr(redirect_parsed[i], ">")){
        // parse on redirect
        for (redi_count = 0; redi_count < 2; redi_count++){
          redi_parsed[count] = strsep(&str, ">");

          if (strlen(redirect_parsed[count]) == 0){
            count--;
          }
        }
        redirect_parsed[i] = redi_parsed[0];

        //opening the file
        fd = open(trimwhitespace(redi_parsed[1]), O_WRONLY | O_CREAT | O_TRUNC, 0666); // opening file

        // duplicating file descrptor for later
        saved_stdout = dup(1);
        dup2(fd, 1);
        close(fd);

        if(fd < 0){ // checking for open file error
          write(STDERR_FILENO, error_message, strlen(error_message));
          return;
        }

        // set a flag for later
        redi_flag = 1;
      }

      parse_line(redirect_parsed[i], parsed);
      //what if redirected
      char* working_path = check_path(path, parsed[0]); 

      flag = execv(working_path, parsed);

      if (flag < 0){
        write(STDERR_FILENO, error_message, strlen(error_message));
      }

      if(redi_flag == 1){
        dup2(saved_stdout, 1); //switch back out put
        close(saved_stdout);
      }

      exit(0);
    }
    else if(pid < 0){
      write(STDERR_FILENO, error_message, strlen(error_message));
      return;
    }
  }

  while((wpid = wait(&status)) > 0);
  return;
}

int redirect(char* str, char** redirect_parsed, char ** parsed_args, char* path){
  int count, fd, saved_stdout;

  str[strcspn(str, "\n")] = 0;

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

  fd = open(trimwhitespace(redirect_parsed[1]), O_WRONLY | O_CREAT | O_TRUNC, 0666); // opening file
    
  if(fd < 0){ // checking for open file error
    write(STDERR_FILENO, error_message, strlen(error_message));
    return(-1);
  }

  saved_stdout = dup(1);
  dup2(fd, 1);
  close(fd);
  
  parse_line(redirect_parsed[0], parsed_args);

  redi_execute_arg(parsed_args, path);
  wait(NULL);
    
  dup2(saved_stdout, 1); //switch back out put
  close(saved_stdout);

  // exit(0);
  return(0);
}

int run_command(char * str, int size, char* path){
  char* parsed_args[size];
  char* redi_parsed_args[size];
  int arg_num, parallel_flag = 0, redirect_flag = 0;

  char* input;
  // printf("Im in run command\n");

  str[strcspn(str, "\n")] = 0;

  input = (char*)malloc(1000*sizeof(char));
  input = strcpy(input, str);

  if(strstr(str, "&")){ //check for parallel
    parallel(str, path);
    parallel_flag = 1;
    return(0);
  } 
  else if(strstr(str, ">")){ // check for redirection
    redirect(str, redi_parsed_args, parsed_args, path);
    redirect_flag = 1;
    return(0);
  } 
  else{ // no redirect or parallel flags
    arg_num = parse_line(str, parsed_args);

    // checking to see if it is a built in command
    if (strcmp(parsed_args[0], "exit") == 0){
      if (arg_num != 1){
        write(STDERR_FILENO, error_message, strlen(error_message));
        // printf("its in exit 1 \n");
        return(0);
      }
      exit(0);
      return(1);
    } 
    else if (strcmp(parsed_args[0], "cd") == 0){
      if (arg_num != 2){
        write(STDERR_FILENO, error_message, strlen(error_message));
        // printf("its in cd1 \n");
        return(0);
      } else if (chdir(parsed_args[1]) == -1){
        write(STDERR_FILENO, error_message, strlen(error_message));
        return(0);
      }
      return(0);
    } 
    else if (strcmp(parsed_args[0], "path") == 0){
      strcpy(path, input);
      return(0);
    } 
  }
    

  if (parallel_flag == 0 && redirect_flag == 0){
    execute_arg(parsed_args, path);
  }
  return(0);
}

int batch_wish(FILE *fp, char* path){
  char *line_buffer = NULL;
  size_t line_buffer_size = 0;
  int input_size = 0, run = 0;
  // char* path;

  // // printf("Im in batch\n");
  // path = (char*)malloc(1000*sizeof(char));
  // path = strcpy(path, "/bin");
  // printf("the path is %s\n", path);

  while (run == 0){

    input_size = getline(&line_buffer, &line_buffer_size, fp);
    // printf(" the in put is size %d and the line len is %ld\n ", input_size, strlen(line_buffer));
    line_buffer = trimwhitespace(line_buffer);
    // printf(" the in put is size %d and the line len is %ld\n ", input_size, strlen(line_buffer));
    if(strlen(line_buffer) == 0){
      continue;
    }
    // printf("getting an dline \n");
    if (input_size < 0){
      return(run);
    }
  
    run = run_command(line_buffer, input_size, path);
    wait(NULL);
    // printf("success!\n");
  }
  return(run);
}

int interactive_wish(char* path){
  char *line_buffer = NULL;
  size_t line_buffer_size =0;
  int input_size, run = 0;
  // char* path;
  // printf("the path is %s\n", path);

  // path = (char*)malloc(1000*sizeof(char));
  // path = strcpy(path, "/bin");

  while (run == 0){
    printf("wish> ");
    input_size = getline(&line_buffer, &line_buffer_size, stdin);
    if(input_size == 1){
      continue;
    }
    run = run_command(line_buffer, input_size, path);
    wait(NULL);
  }
  return(run);
}

int main(int argc, char **argv)
{
  // printf("I'm in main agin!");
  int run;
  char* path;

  // printf("Im in batch\n");
  path = (char*)malloc(1000*sizeof(char));
  path = strcpy(path, "/bin");

  if(argc > 1){
    FILE *fpin;

    fpin = fopen(argv[1], "r");
    if(fpin == NULL){
      write(STDERR_FILENO, error_message, strlen(error_message));
      // printf("its in batch 1 \n");
      exit(1);
    }
    if(argc > 2){
      write(STDERR_FILENO, error_message, strlen(error_message));
      // printf("its in batch 2\n");
      exit(1);
    }

    run = batch_wish(fpin, path);
    if(run == 1){
    exit(0);
  }
    exit(0);

  } else { 
  run = interactive_wish(path);
  if(run == 1){
    printf("shouldn't I exit\n");
    exit(0);
    printf("i really should \n");
    exit(0);
  }
  exit(0);
  }

}