#ifndef VARIABLE_H
#define VARIABLE_H

#include <ctype.h>
#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#define NORMAL 0
#define CONTINUE 1
#define BREAK 2

class Variable {
 private:
  std::map<std::string, std::string> mp;
  std::vector<std::string> myEnv;

  void initialMyEnv();
  void initialMp();

  void setMap(const std::string key, const std::string value);
  void addMyEnv(const std::vector<std::string> & key);
  void revEnvVar(const std::vector<std::string> & key);

 public:
  Variable() { initialMyEnv(); }
  std::map<std::string, std::string> getMp() { return mp; }
  std::vector<std::string> getMyEnv() { return myEnv; }
  std::string getEnvVal(const std::string & key);
  void changeDirectory(const std::string & argv);
  int chooseMode(const std::string & _cmd,
                 const std::string & _argv,
                 std::vector<std::string> & kv);
};

bool parseArgv(const std::string & dup, std::vector<std::string> & tokens);
void parseStr(const std::string & buf,
              std::vector<std::string> & tokens,
              const std::string & delim);

#endif
