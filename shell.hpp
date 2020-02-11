#ifndef SHELL_H
#define SHELL_H
#include <fcntl.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define KYEL "\x1B[33m"  //make everything yellow
#include "command.hpp"

class Shell {
 private:
  char ** myArgv;
  char ** myEnv;
  pid_t pid;
  Command command;

  void printPrompt();
  void checkReturn();
  void exec();
  char ** convertCharSS(const std::vector<std::string> & strings,
                        std::vector<char *> & myvector);

 public:
  void run();
};

#endif
