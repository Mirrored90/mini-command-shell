# mini-command-shell
Mini-Project Option: Command Shell
==================================

For this option, you will write a simplified version of a command shell
(ffosh--for Five Fifty One SHell).
You should use C++ for this program, so that you can demonstrate your
command of object orientation.

Hint: think about the NOUNS (classes) involved and what VERBS they can
do (methods). This will guide you in determining how you can make good use of
classes in this assignment.


Step 1: Read a command name and run it
--------------------------------------

  You should write a program called "ffosh." Whenever your command shell
  expects input, it should print a prompt ("ffosh$").
  It should read one line of input (from stdin), which is the name of a
  command. For this step, you may assume that the user will type the full
  path to the command they want to run.

  Your shell should then run the specified command--see the man pages for
  the system calls fork, execve, and waitpid. Note that there is a library
  function "system" which combines all three of these, which may be easier
  for now, but will NOT work for later steps.

  After the specified program exits, your shell should print:
    Program was successful
    [if the exit status was 0]
  OR
    Program failed with code 1
    [if the program failed: replace 1 with actual exit status]
  OR
    Terminated by signal 11
    [replace 11 with the actual signal]
  See man waitpid and the WEXITSTATUS(status) and WIFSIGNALED (status) macros.

  Then your shell should print the "ffosh$" prompt again and repeat
  the process.

  If the user types the command "exit" or EOF is encountered reading
  from stdin, then the shell should exit.

Step 2: Improving commands
--------------------------

  For this step, you will improve your handling of commands in two ways:
  
   1. The program searches the ECE551PATH environment variable for commands
   2. Commands can take arguments (separated by white spaces)
   
  For the first, you will want to initialize the ECE551PATH
  environment variable withe the value of PATH that exists when your
  shell starts (see man getenv), which will be a
  colon-delimited list of paths. When the user types a command name that
  does not have a forward slash (/) in it, you should search each
  directory specified in the ECE551PATH (in order) for the specified program.
  If the program is found, your shell should execute it. If not, your
  shell should print
    Command commandName not found
    [replace commandName with the actual command name]
  If the path name does contain a / in it, you should only look in the
  specified directory (which may not be on the PATH at all). Note that a
  path with a / in it could be relative (.e.g, ./myProgram) or absolute
  (e.g. /bin/ls).

  Note that ECE551PATH should look like the PATH variable that bash uses,
  and behave like it (you should not use PATH except to initialize ECE551PATH
  when your shell starts). Specifically, ECE551PATH is a colon (:) delimited
  list of directory names which is searched in order.   So if ECE551PATH is
    /bin:/usr/bin:/usr/local/bin
  and a command of xyz is run, then you would search for
    /bin/xyz
    /usr/bin/xyz
    /usr/local/bin/xyz
  in that order.
  You do NOT search recursively within the directories listed.
  Note that any change to ECE551PATH (in Step 3) should take effect immediately
  for the next command that the user tries to execute.

  For the second, you should also make commands such that they can take
  arguments separated by white space. For example
    ./myProgram a b 23
  runs myProgram with arguments "a," "b," and "23." There may be an
  arbitrary amount of whitespace in between arguments, so 
    ./myProgram         a               b     23
  has the same behavior as the previous example. However,
  the user may also use "s to include literal whitepsace:
     ./myProgram  "a   b"   c       "def   ghi"
  runs myProgram with 3 arguments:
   1. a   b
   2. c
   3. def   ghi
  Your shell should support escaped quotation marks (\") and backslashes (\\)
  to allow the user to write a literal quotation mark and backslashes:
     ./myProgram  "\"hello world\""   "a\\b\\c d\\"
   runs myProgram with 2 arguments
    1. "hello world"
    2. a\b\c d\
    
  It is an error to have an unclosed quotation mark at the end of the command line.
  
Step 3: Directories and variables
---------------------------------

   1. Add the "cd" command to change the current directory. See the chdir
      function.
   2. The prompt should show the current directory before the $.
      That is, if your current directory is /home/drew, the prompt should
      be
        ffosh:/home/drew $
   3. Your shell should provide access to variables. A variable name
      must be a combination of letters (case sensitive), underscores,
      and numbers, (e.g., PATH, xyz, abc42, my_var).  Note that
      $var1$var2 is legal: if var1=a and var2=b, then $var1$var2
      expands to ab.  Likewise, $var1- is legal: it is $var1 followed
      immediately by - (so again if var1=a, it would be a-).
   4. If the user writes $varname on the command line, your shell should
      replace it with the current value of that variable.
   5. You should provide three built-in commands:
      - set var value
        This should set the variable var to the string on the rest of the
	command line (even if it contains spaces, etc.) Your shell should
	remember this value, and make use of it in future $ evaluations;
	however, it should not be placed in the environment for other
	programs.
      - export var
        This should put the current value of var into the environment for
	other programs. Note that you may find the "env" command useful for
	testing this.
      - rev var
        Reverses the string held in var.  For example
	 set x hello
	 //now x = hello
	 rev hello
	 //now x = olleh
   6. If you "set" a variable (but dont "export" it) the new value should
      not show up in "env." If you "export" it, the new value should show
      up in "env".

  Note also that if the user changes PATH, it should affect where your shell
  searches for programs.

Step 4: Pipes and redirection
-----------------------------

  - Implement input redirection (<) and output redirection (>)
      < filename   redirects standard input for the command
      > filename   redirects standard ouput
      2> filename  redirects standard error
    Note that you will need to implement these between the fork()
    and execve() calls.  You will need to make use of close()
    on the relevant file descriptors (0 = stdin, 1 = stdout, 2 = stderr)
    use open() to open the appropriate file.  You may also need to make
    use of dup2().

  - Implement pipes (|)
    You should be able to run one command and pipe its output to another's
    input:
      ./myProgram | ./anotherProgram
    See the pipe() system call.

  Note that you need to be able to mix and match these in ways that make
  sense, along with having command line arguments:
    ./myProgram a b c < anInputFile | ./anotherProgram 23 45 > someOutputFile
  NOTE: It is very easy to write a broken implementation of pipes which
  LOOKS like it works on short input (< 65536 bytes) but deadlocks on longer
  input. Your implemenation must work correctly when sending large
  amounts of data through the pipe for full credit.

  Using fcntl with F_SETPIPE_SZ to adjust the kernel buffer size to
  "larger than you think we will test with" is NOT a valid approach.
  If we see you trying to do this, you will get no credit for part 4.
  
 For Part 4, you may refer to parts 11 through 15 of the gnu libc reference:
 https://www.gnu.org/software/libc/manual/html_node/index.html#toc-Introduction-1
 for additional background on file descriptors and pipes.

 When you run a pipeline of commands, you should report the exit status
 of the last (rightmost one) only.
