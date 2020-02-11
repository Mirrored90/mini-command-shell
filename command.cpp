#include "command.hpp"

/**
 * This function split input lines into cmd and argv.
 * The main purpose is to skip leading space.
 */
void Command::splitInTwo() {
  int len = lines.size();
  int i = 0;
  // skip the leading space
  for (; i < len; i++) {
    if (lines[i] != ' ') {
      break;
    }
  }
  // parse into two: cmd and argv
  int j = i;
  for (; j < len; j++) {
    if (lines[j] == ' ') {
      break;
    }
  }
  // push back i-j: cmd
  for (int k = i; k < j; k++) {
    cmd.push_back(lines[k]);
  }
  // push back rest: argv
  for (int k = j + 1; k < len; k++) {
    argv.push_back(lines[k]);
  }
}

/**
 * This function looks up the map.
 * Any time if the function find a "$", it will replace the 
 * upcoming key with its value. If the key dose not exit,
 * it will do nothing.
 */
void Command::replaceEnvVar(std::string & input,
                            const std::map<std::string, std::string> & mp) {
  for (size_t i = 0; i < input.size(); i++) {
    // check if there is any charactor behind "$"
    if (input[i] == '$' && i != input.size() - 1) {
      // save each charactor in ss
      std::stringstream ss;
      std::string value;
      size_t pos;
      // loop and check if ss exist in map
      for (size_t j = i + 1; j < input.size(); j++) {
        ss << input[j];
        // if find ss exist, get the value
        if (mp.find(ss.str()) != mp.end()) {
          value = mp.find(ss.str())->second;
          pos = j;
        }
      }
      // exchange the value with the key
      if (value != "\0") {
        input.erase(i, pos - i + 1);
        input.insert(i, value);
      }

    }  // end big if
  }    // end for loop
}

/**
 * This function extend the cmd to a path:
 * It can be either absolute path, or relative path.
 * The accessablility is checked by function access. 
 */
bool Command::setPath() {
  std::vector<std::string> tokens;
  // get ECE551PATH
  std::string ECE551PATH = var.getEnvVal("ECE551PATH");
  // parse it into different tokens by ":"
  parseStr(ECE551PATH, tokens, ":");

  if (cmd == "." || cmd == "/" || cmd == "..") {
    std::cout << "Command " << cmd << " not found\n";
    return false;
  }

  // loop every possible path
  for (size_t i = 0; i < tokens.size(); i++) {
    // relative path
    tokens[i] = tokens[i] + "/" + cmd;
    if ((access(tokens[i].c_str(), F_OK)) == 0) {
      path = tokens[i];
      return true;
    }
    // absolute path
    else if ((access(cmd.c_str(), F_OK)) == 0) {
      path = cmd;
      return true;
    }
  }
  std::cout << "Command " << cmd << " not found\n";
  return false;
}

/**
 * This function parse the spcial charactor \\ \" and space
 */
bool Command::parse() {
  std::string buf(path + " " + argv);
  if (!parseArgv(buf, tokens)) {
    std::cout << "Unclosed quotation mark!\n";
    return false;
  }
  return true;
}

/**-----------
 *   PUBLIC
 ------------*/
bool Command::getLines() {
  if (!std::getline(std::cin, lines)) {
    std::cout << "\n";
    return false;
  }
  return true;
}

int Command::wrapUp() {
  splitInTwo();
  parseArgv(argv, kv);
  replaceEnvVar(cmd, var.getMp());
  replaceEnvVar(argv, var.getMp());

  mode = var.chooseMode(cmd, argv, kv);
  if (mode == CONTINUE) {
    return CONTINUE;
  }
  else if (mode == BREAK) {
    return BREAK;
  }

  mode = setPath() ? NORMAL : CONTINUE;
  if (mode == CONTINUE) {
    return CONTINUE;
  }

  mode = parse() ? NORMAL : CONTINUE;
  if (mode == CONTINUE) {
    return CONTINUE;
  }

  env = var.getMyEnv();

  return NORMAL;
}

/**
 * This function performs redirection for stdin, stdout, stderr;
 * It is achieved by calling open dup2 and close. 
 */
bool Command::redir(size_t pos, int ioType) {
  const char * filename = tokens[pos + 1].c_str();
  int fd, dest;

  // input redirection
  if (ioType == 0) {
    fd = open(filename, O_RDONLY);
    dest = STDIN_FILENO;
  }
  // output redirection
  else {
    fd = open(filename,
              O_WRONLY | O_CREAT | O_TRUNC,
              S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (ioType == 1) {
      dest = STDOUT_FILENO;
    }
    else {
      dest = STDERR_FILENO;
    }
  }

  if (fd == -1) {
    std::cerr << filename << " does not exist\n";
    return false;
  }
  if (dup2(fd, dest) == -1) {
    perror("dup2");
    return false;
  }
  if (close(fd) == -1) {
    perror("close");
    return false;
  }

  tokens.erase(tokens.begin() + pos, tokens.begin() + pos + 2);

  return true;
}

/**
 * This function scans tokens to check if it contains:
 * "<", ">" and "2>"; and it performs input redirction,
 * output redirction and error redirction accordingly.
 */
bool Command::ioRedir() {
  for (size_t i = 0; i < tokens.size() - 1; i++) {
    if (tokens[i] == "<") {
      if (!redir(i, 0)) {
        return false;
      }
      i--;
    }
    else if (tokens[i] == ">") {
      if (!redir(i, 1)) {
        return false;
      }
      i--;
    }
    else if (tokens[i] == "2>") {
      if (!redir(i, 2)) {
        return false;
      }
      i--;
    }
  }
  return true;
}

/**
 * Clear everything stored in container.
 */
void Command::clearAll() {
  lines.clear();
  cmd.clear();
  argv.clear();
  path.clear();
  kv.clear();
  tokens.clear();
}
