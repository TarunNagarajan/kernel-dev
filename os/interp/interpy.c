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
#define MAX_COMMANDS 16

typedef struct {
  int append_mode;
  int merge_stderr;
  char* args[MAX_ARGS];
  char* input_file;
  char* output_file;
  char* error_file;
  char* heredoc_delim;
} Command;

typedef struct {
  Command commands[MAX_COMMANDS];
  int command_count;
} Pipeline;

void prompt() {
  // printf("[interpy] ");
  char cwd[MAX_INPUT];

  if ((getcwd(cwd, sizeof(cwd)) != NULL)) {
    printf("[interpy] [%s] ", cwd);
  } else {
    perror("cwd");
    printf("[interpy] ");
  }

  fflush(stdout);
}

void parse_command(char* line, Command* cmd) {
  char* tokens[MAX_TOKENS];
  int token_count = 0;
  int arg_count = 0;
  memset(cmd->args, 0, sizeof(cmd->args));

  cmd->input_file = NULL;
  cmd->output_file = NULL;
  cmd->error_file = NULL;
  cmd->heredoc_delim = NULL;
  cmd->append_mode = 0;
  cmd->merge_stderr = 0;
  
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

    // HERE-DOCUMENT
    else if (strcmp(tokens[i], "<<") == 0) {
      if (i + 1 < token_count) {
        cmd->heredoc_delim = tokens[++i];
      }
    }

    // ERROR REDIRECTION
    else if (strcmp(tokens[i], "2>") == 0) {
      if (i + 1 < token_count) {
        cmd->error_file = tokens[++i];
      }
    }

    // MERGE ERROR + OUTPUT 
    else if (strcmp(tokens[i], "2>&1") == 0) {
      cmd->merge_stderr = 1;
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

void parse_pipeline(char* line, Pipeline* pipe) {
  char* saveptr;
  char* segment;
  int count = 0;

  pipe->command_count = 0;
  segment = strtok_r(line, "|", &saveptr);

  while (segment != NULL && count < MAX_COMMANDS) {
    parse_command(segment, &pipe->commands[count]);
    count++;
    segment = strtok_r(NULL, "|", &saveptr);
  }

  pipe->command_count = count;
}

void handle_heredoc(Command* cmd) {
  if (cmd->heredoc_delim == NULL) return;

  FILE* fp = fopen(".tempheredoc", "w");
  
  if (!fp) {
    perror("heredoc");
    return;
  }

  char line[MAX_INPUT];

  while (1) {
    printf("> ");
    if (fgets(line, sizeof(line), stdin) == NULL) {
      break;
    }

    line[strcspn(line, "\n")] = 0;

    if (strcmp(line, cmd->heredoc_delim) == 0) {
      break;
    }

    // FILE NEWLINE
    fprintf(fp, "%s\n", line);
  } 

  fclose(fp);
  cmd->input_file = ".tempheredoc";
}

void launch_process(Command* cmd) {
  handle_heredoc(cmd); // files override pipes
  
  if (cmd->input_file != NULL) {
    int fd_in = open(cmd->input_file, O_RDONLY);
    if (fd_in < 0) {
      perror("input redirection"); 
      exit(1);
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
      exit(1);
    }

    dup2(fd_out, STDOUT_FILENO);
    close(fd_out); 
  }

  if (cmd->error_file != NULL) {
    int fd_err = open(cmd->error_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd_err < 0) {
      perror("error redirection");
      exit(1);
    }

    dup2(fd_err, STDERR_FILENO);
    close(fd_err);
  }

  if (cmd->merge_stderr) {
    dup2(STDOUT_FILENO, STDERR_FILENO);
  }

  execvp(cmd->args[0], cmd->args);
  perror("execvp");
  exit(1);
}

int execute_command(Command* cmd) {
  handle_heredoc(cmd);
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

    if (cmd->error_file != NULL) {
      int fd_err = open(cmd->error_file, O_WRONLY | O_CREAT | O_TRUNC, 0644); 
      
      if (fd_err < 0) {
        perror("error redirection");
        return 1;
      }

      /*
      
      if (cmd->merge_stderr) {
        dup2(STDOUT_FILENO, STDERR_FILENO);
      } else {
        dup2(fd_err, STDERR_FILENO);
      }

      close(fd_err);

      */ 

      dup2(fd_err, STDERR_FILENO);
      close(fd_err);    
    }

  if (cmd->merge_stderr) {
      dup2(STDOUT_FILENO, STDERR_FILENO);
    }

    execvp(cmd->args[0], cmd->args);
    perror("execvp");
    exit(1);
  } else {
    int status;
    waitpid(pid, &status, 0);

    if (cmd->heredoc_delim != NULL) {
      unlink(".tempheredoc"); 
    }
  }

  return 1;
}

int execute_pipeline(Pipeline* pipeline) {
  int i;
  int prev_fd = -1;
  int fd[2]; 
  pid_t pid;

  if (pipeline->command_count == 1) {
    Command* cmd = &pipeline->commands[0];
    if (strcmp(cmd->args[0], "exit") == 0) {
      exit(0);
    } 

    if (strcmp(cmd->args[0], "cd") == 0) {
      if (cmd->args[1] == NULL) {
        fprintf(stderr, "[cd] missing argument\n");
      } else if (chdir(cmd->args[1]) != 0) {
        perror("cd"); 
        return 1;
      }
      return 1;
    }
  }

  for (i = 0; i < pipeline->command_count; i++) {
    // DON'T CREATE A PIPE FOR THE LAST COMMAND
    if (i < pipeline->command_count - 1) {
      if (pipe(fd) < 0) {
        perror("pipe");
        return 1;
      }
    }

    pid = fork();

    if (pid < 0) {
      perror("fork");
      return 1;
    }

    if (pid == 0) {
      if (prev_fd != -1) {
        dup2(prev_fd, STDIN_FILENO); 
        close(prev_fd);
      }

      if (i < pipeline->command_count - 1) {
        dup2(fd[1], STDOUT_FILENO); 
        close(fd[1]);
        close(fd[0]);
      }

      launch_process(&pipeline->commands[i]);
    }

    else {
      if (prev_fd != -1) {
        close(prev_fd);
      }

      if (i < pipeline->command_count - 1) {
        close(fd[1]);
        prev_fd = fd[0];
      }
    }
  }

  for (int i = 0; i < pipeline->command_count; i++) {
    wait(NULL);
  }

  return 1;
}

int main() {
  char input[MAX_INPUT];
  Pipeline pipe;
  // Command cmd;

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

    parse_pipeline(input, &pipe);
    execute_pipeline(&pipe); 
  }

  return 0;
}
