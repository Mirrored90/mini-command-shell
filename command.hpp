#ifndef COMMAND_H
#define COMMAND_H

#include <fcntl.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "variable.hpp"
class Command {
 private:
  std::string lines;
  std::string cmd;
  std::string argv;
  std::string path;
  std::vector<std::string> kv;
  int mode;
  std::vector<std::string> tokens;
  std::vector<std::string> env;
  Variable var;

  void splitInTwo();
  void replaceEnvVar(std::string & input, const std::map<std::string, std::string> & mp);
  bool setPath();
  bool parse();

 public:
  bool getLines();
  int wrapUp();
  bool redir(size_t pos, int ioType);
  bool ioRedir();
  void clearAll();
  std::vector<std::string> getTokens() { return tokens; }
  std::vector<std::string> getEnv() { return env; }
  std::string getPath() { return path; };
};

#endif
