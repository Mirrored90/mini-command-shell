CFLAGS =-Wall -std=gnu++98 -Werror -pedantic 
ffosh: ffosh.cpp variable.hpp variable.cpp command.hpp command.cpp shell.hpp shell.cpp 
	g++ -o ffosh $(CFLAGS) ffosh.cpp variable.hpp variable.cpp command.hpp command.cpp shell.hpp shell.cpp 
.PHONY: clean
clean:
	rm -f mian *.o *~
