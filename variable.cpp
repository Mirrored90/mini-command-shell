#include "variable.hpp"

/**
 * This function initialize environmental viaribale 
 * accoring to the map. The map was initialize by function
 * initialMp.
 */
void Variable::initialMyEnv() {
  initialMp();
  int i = 0;
  // while not enconter NULL
  while (environ[i]) {
    myEnv.push_back(environ[i++]);
  }
}

/**
 * This function step up a map to store any user set variables.
 * First it is initialized as same as environ. Then one special 
 * value ECE551PATH is pushed into the map.
 */
void Variable::initialMp() {
  int i = 0;
  while (environ[i]) {
    std::string temp(environ[i++]);
    // parse every environ by "=" in to key and value
    size_t pos = temp.find_first_of('=', 0);

    std::string value(temp.substr(pos + 1));
    std::string key(temp.substr(0, pos));
    mp[key] = value;
  }
  // ECE551PATH = PATH
  std::string PATH(getenv("PATH"));
  setMap("ECE551PATH", PATH);
}

/**
 * This function add new key value paris into the map.
 * If the key already exist, the value will be updated.
 * Only letters, numbers and underscore can show up in key.
 */
void Variable::setMap(const std::string key, const std::string value) {
  bool isValid = true;
  for (size_t i = 0; i < key.size(); i++) {
    // check if current letter is valid
    isValid &= isalnum(key[i]) || key[i] == '_';
  }
  // if it is valid, put into map
  if (isValid) {
    setenv(key.c_str(), value.c_str(), 1);
    mp[key] = value;
  }
  else {
    std::cout << "Key: " << key << " is invalid!\n";
  }
}

/**
 * This function add envrionmental variable by pushback new
 * string into vector myEnv.
 */
void Variable::addMyEnv(const std::vector<std::string> & key) {
  for (size_t i = 0; i < key.size(); i++) {
    // use key to find value
    std::string value = getEnvVal(key[i]);
    // if not empty
    if (value != "\0") {
      std::string temp = key[i] + "=" + value;
      myEnv.push_back(temp);
    }
  }
}

/**
 * This function reverse variables as user input rev in shell.
 * First it will find if the key has corresponding values,
 * then the value is reversed and added to map.
 */
void Variable::revEnvVar(const std::vector<std::string> & key) {
  for (size_t i = 0; i < key.size(); i++) {
    std::string value = getEnvVal(key[i]);
    if (!value.empty()) {
      // reverse by using iterator
      std::reverse(value.begin(), value.end());
      setMap(key[i], value);
    }
  }
}

/**-------------
 *     PUBLIC
 ---------------*/
/**
 * This function performs cd.
 * It is achieved by calling chdir.
 */
void Variable::changeDirectory(const std::string & argv) {
  std::vector<std::string> temp;
  parseArgv(argv, temp);
  if (temp.size() != 1) {
    std::cerr << "Arguments number is wrong!\n";
  }
  else {
    const char * path = temp[0].c_str();
    if (chdir(path) == -1) {
      perror("chdir failed");
    }
  }
}

/**
 * This function use key to find value.
 * If the value dose not exit, it will return emtpy string.
 */
std::string Variable::getEnvVal(const std::string & key) {
  if (mp.find(key) != mp.end()) {
    std::string value = mp.find(key)->second;

    return value;
  }
  else {
    std::cout << "Key [" << key << "] does not exist!\n";
  }
  return "";
}

/**
 * This function perform cd, set, export and exit
 */
int Variable::chooseMode(const std::string & _cmd,
                         const std::string & _argv,
                         std::vector<std::string> & kv) {
  int mode = NORMAL;
  // no input
  if (_cmd == "\0") {
    std::cout << "No input cmd!\n";
    mode = CONTINUE;
  }

  // cd
  else if (_cmd == "cd") {
    changeDirectory(_argv);
    mode = CONTINUE;
  }

  // set var
  else if (_cmd == "set") {
    if (kv.size() != 2) {
      std::cout << "Wrong number of input for kv pairs!\n";
    }
    else {
      setMap(kv[0], kv[1]);
    }
    mode = CONTINUE;
  }
  // rev var
  else if (_cmd == "rev") {
    if (kv.size() == 0) {
      std::cout << "Not enough argument!\n";
    }
    else {
      revEnvVar(kv);
    }
    mode = CONTINUE;
  }

  // export
  else if (_cmd == "export") {
    if (kv.size() == 0) {
      std::cout << "Not enough argument!\n";
    }
    else {
      addMyEnv(kv);
    }
    mode = CONTINUE;
  }

  // exit
  else if (_cmd == "exit") {
    mode = BREAK;
  }
  return mode;
}

/**                                                                                    
 * This functions parse at every delim and store substrings in tokens                 
 * It is only used to parse ":" to search for PATH                                     
 */
void parseStr(const std::string & buf,
              std::vector<std::string> & tokens,
              const std::string & delim) {
  std::string temp;
  std::string dup(buf);
  size_t pos = 0;

  while ((pos = dup.find(delim)) != std::string::npos) {
    if (pos != 0) {
      temp = dup.substr(0, pos);
      tokens.push_back(temp);
    }
    dup.erase(0, pos + delim.size());
  }

  if (dup != "\0") {
    tokens.push_back(dup);
  }
}

/**
 * This function do most of the job of parsing.
 * It deal with special charactors such as \\, \*, " and space.
 * I used a stringstream ss to hold temperay substring.
 * If ss meets some reqirements, eg. enclosed by two "",
 * it will be pushed into a vector string and it selfwill be updated to empty.
 */
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
        return false;
      }
      for (size_t j = i + 1; j < dup.size(); j++) {
        // quatation mark pair not found
        if (j == dup.size() - 1 && dup[j] != '"') {
          ss.str("");
          return false;
        }

        // skip backslash
        if (dup[j] == '\\') {
          if (j == dup.size() - 2) {
            ss.str("");
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
