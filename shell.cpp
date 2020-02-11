#include "shell.hpp"

/**
 * This function prompring message as:
 * "ffosh CURRENT WORKING DIRECTORY $:"
 * current working directory is obtained by getcwd function
 */
void Shell::printPrompt() {
  char temp[PATH_MAX];
  std::string cwd;
  if (getcwd(temp, PATH_MAX)) {
    cwd = temp;
  }
  else {
    perror("getcwd Failed!\n");
    exit(EXIT_FAILURE);
  }
  std::cout << KYEL << "ffosh:" << cwd << " $";
}

/**
 * This function check the return status of the child process
 */
void Shell::checkReturn() {
  int status;
  pid_t ret = waitpid(pid, &status, 0);
  if (ret < 0) {
    std::cerr << "Waitpid failed!\n";
    exit(EXIT_FAILURE);
  }
  //normally terminated
  if (WIFEXITED(status)) {
    //successful
    if (WEXITSTATUS(status) == 0) {
      std::cout << "Program was successful\n";
    }
    //faild, print the exit status
    else {
      std::cout << "Program failed with code " << WEXITSTATUS(status) << "\n";
    }
  }
  //terminated by signal
  if (WIFSIGNALED(status)) {
    std::cout << "Terminated by signal " << WTERMSIG(status) << "\n";
  }
}

void Shell::exec() {
  // fork
  pid = fork();
  if (pid < 0) {
    std::cerr << "Fork failed!\n";
    exit(EXIT_FAILURE);
  }

  //child process
  else if (pid == 0) {
    // redir
    if (!command.ioRedir()) {
      exit(EXIT_FAILURE);
    }

    // convert to char**
    std::vector<std::string> tokens = command.getTokens();
    std::vector<char *> myargv(tokens.size() + 1);
    myArgv = convertCharSS(tokens, myargv);

    //int execve(const char *filename, char *const argv[],char *const envp[]);
    std::vector<std::string> env = command.getEnv();
    std::vector<char *> myenv(env.size() + 1);
    myEnv = convertCharSS(env, myenv);

    const char * myPath = command.getPath().c_str();
    execve(myPath, myArgv, myEnv);
    perror("Execve Failed");
    exit(EXIT_FAILURE);
  }
  //parent process
  else {
    checkReturn();
  }  //end else for parent process
}

/**
 * This function generate char **
 */
char ** Shell::convertCharSS(const std::vector<std::string> & strings,
                             std::vector<char *> & myvector) {
  for (size_t i = 0; i < strings.size(); ++i) {
    myvector[i] = (char *)strings[i].data();
  }
  myvector.push_back(NULL);

  return myvector.data();
}

/**----------
 *  PUBLIC
 ------------*/

void Shell::run() {
  while (true) {
    printPrompt();
    if (!command.getLines()) {
      break;
    }

    int status = command.wrapUp();
    if (status == BREAK) {
      break;
    }
    else if (status == CONTINUE) {
      command.clearAll();
      continue;
    }

    exec();
    command.clearAll();
  }  //end while
}
