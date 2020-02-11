#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
bool parseArgv(const std::string & dup, std::vector<std::string> & tokens) {
  std::stringstream ss;
  for (size_t i = 0; i < dup.size(); i++) {
    // encounter a escape sequence
    if (dup[i] == '\\' && i != dup.size() - 1) {
      if (dup[i + 1] == '\\' || dup[i + 1] == '"') {
        ss << dup[i + 1];
        i++;
      }
      else {
        ss << dup[i];
      }
    }

    // encounter a quatation mark
    else if (dup[i] == '"') {
      if (ss.str() != "\0") {
        tokens.push_back(ss.str());
        ss.str("");
      }

      if (i == dup.size() - 1) {
        ss.str("");
        std::cout << "Unpaired\n";
        return false;
      }
      for (size_t j = i + 1; j < dup.size(); j++) {
        // quatation mark pair not found
        if (j == dup.size() - 1 && dup[j] != '"') {
          ss.str("");
          std::cout << "Unpaired!\n";
          return false;
        }

        // skip backslash
        if (dup[j] == '\\') {
          if (j == dup.size() - 2) {
            ss.str("");
            std::cout << "Unpaired!\n";
            return false;
          }
          if ((dup[j + 1] == '\\' || dup[j + 1] == '"')) {
            ss << dup[j + 1];
            j++;
            i = j;
          }
          else {
            ss << dup[j];
          }
        }
        // find the marched quatiation mark
        else if (dup[j] == '"') {
          tokens.push_back(ss.str());
          ss.str("");
          i = j;
          break;
        }

        // push all other char into ss
        else {
          ss << dup[j];
        }
      }
    }  // end quatioan mark

    else if (dup[i] == ' ') {
      if (ss.str() != "\0") {
        tokens.push_back(ss.str());
        ss.str("");
      }
    }

    else {
      ss << dup[i];
    }
  }  // end big for loop

  if (ss.str() != "\0") {
    tokens.push_back(ss.str());
    ss.str("");
  }

  return true;
}
/*
int main() {
  //std ::string input("g++ -o \"\\\"hello world\"\\\" hello\\\\word.cpp ");
  std ::string input;
  std::getline(std::cin, input);

  std::vector<std::string> tokens;

  parseArgv(input, tokens);

  std::cout << "display my token:\n";
  for (unsigned long i = 0; i < tokens.size(); i++) {
    std::cout << tokens[i] << '\n';
  }

  return EXIT_SUCCESS;
}
*/
