#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_TOKENS 128
#define MAX_ARGS 64

typedef struct {
  int append_mode;
  char* args[MAX_ARGS];
  char* input_file;
  char* output_file;
} Command;

void prompt() {
  // printf("[interpy] ");
  char cwd[MAX_INPUT];

  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("[interpy] [%s] ", cwd);
  } else {
    perror("cwd");
    printf("[interpy] ");
  }

  fflush(stdout);
}

void parse(char* line, Command* cmd) {
  char* tokens[MAX_TOKENS];
  int token_count = 0;
  int arg_count = 0;
  memset(cmd->args, 0, sizeof(cmd->args));

  cmd->input_file = NULL;
  cmd->output_file = NULL;
  cmd->append_mode = 0;
  
  char* token = strtok(line, " \t\n");
  while (token != NULL && token_count < MAX_TOKENS) {
    tokens[token_count++] = token;
    token = strtok(NULL, " \t\n");
  }
  
  for (int i = 0; i < token_count; i++) {
    // I: INPUT REDIRECTION
    if (strcmp(tokens[i], "<") == 0) {
      if (i + 1 < token_count) {
        cmd->input_file = tokens[++i];
      }
    } 

    // II: OUTPUT REDIRECTION
    else if (strcmp(tokens[i], ">") == 0) {
      if (i + 1 < token_count) {
        cmd->output_file = tokens[++i];
        cmd->append_mode = 0;
      }
    }

    // III: APPEND REDIRECTION
    else if (strcmp(tokens[i], ">>") == 0) {
      if (i + 1 < token_count) {
        cmd->output_file = tokens[++i];
        cmd->append_mode = 1;
      }
    }

    // ADD IT TO ARGS
    else {
      if (arg_count < MAX_ARGS - 1) {
        cmd->args[arg_count++] = tokens[i];
      }
    }
  }
  cmd->args[arg_count] = NULL;
}

int execute(Command* cmd) {
  if (cmd->args[0] == NULL) return 1;
  if (strcmp(cmd->args[0], "exit") == 0) {
    exit(0);
  }

  if (strcmp(cmd->args[0], "cd") == 0) {
    if (cmd->args[1] == NULL) {
      fprintf(stderr, "[cd] needs an argument\n");
    } else {
      if (chdir(cmd->args[1]) != 0) {
        perror("cd");
      }
    }
    return 1;
  }

  pid_t pid = fork();

  if (pid < 0) {
    perror("fork"); 
    return 1;
  }

  if (pid == 0) {
    if (cmd->input_file != NULL) {
      int fd_in = open(cmd->input_file, O_RDONLY);
      
      if (fd_in < 0) {
        perror("input redirection");
        return 1;
      }

      dup2(fd_in, STDIN_FILENO);
      close(fd_in);
    }

    if (cmd->output_file != NULL) {
      int fd_out;

      if (cmd->append_mode) {
        fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
      } else {
        fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      }

      if (fd_out < 0) {
        perror("output redirection");
        return 1;
      }

      dup2(fd_out, STDOUT_FILENO);
      close(fd_out);
    }

    execvp(cmd->args[0], cmd->args);
    perror("execvp");
    exit(1);
  } else {
    int status;
    waitpid(pid, &status, 0);
  }

  return 1;
}

int main() {
  char input[MAX_INPUT]; 
  Command cmd;

  while (1) {
    prompt();

    if (fgets(input, MAX_INPUT, stdin) == NULL) {
      printf("\n");
      break;
    }

    input[strcspn(input, "\n")] = 0;

    if (strlen(input) == 0) {
      continue;
    }

    parse(input, &cmd);
    execute(&cmd); 
  }

  return 0;
}
