/**
 * Author: Qingyu Wu 
 * NetID: qw90
 *
 * This program performs a simply shell.
 * It supports following build-in functions:
 * (1): cd: change working directory;
 * (2): set: set variable;
 * (3): rev: reverse the value of variable;
 * (4): export: export variable to into the envrionment;
 * Besides, it also supports stdin, stdout, stderr redirction.
 * My shell will take in user's input and parse it into commands and
 * arguments. The command will be excecuted arccoring to different
 * arguments.
 *
 **/

#include "shell.hpp"

int main(void) {
  Shell shell;
  shell.run();

  return EXIT_SUCCESS;
}
