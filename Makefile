CC = g++
CFLAGS = -Wall -Wextra -Werror 
CFILES = router.cpp path.cpp request.cpp response.cpp tree.cpp
OFILES = router.o path.o request.o response.o tree.o
HFILES = router.hpp path.hpp request.hpp response.hpp http.hpp tree.hpp

NAME = libhttpablu.a

all: $(NAME) clean

$(NAME): ofiles 
	ar rcs $(NAME) $(OFILES)

ofiles:
	$(CC) $(CFLAGS) -std=c++20 -g -O0 -c $(CFILES)

clean:
	rm -f $(OFILES)

fclean:
	rm -f $(NAME)

re: fclean $(NAME)

build: all
	$(CC) -std=c++20 -o server main.cpp -L. -lhttpablu

run: build
	./server

install: all
	mv $(NAME) /lib
	cp $(HFILES) /usr/include

remove: 
	rm /lib/$(NAME)
	rm -d /usr/include/ -f $(HFILES)

.PHONY: all clean fclean re run ofiles remove install
