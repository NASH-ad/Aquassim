NAME	=	aquassim

CFLAGS	=	-Wall -Wextra -Werror -g

SRCS	=	src/main.c \

LIBS	=	-Iinclude -lm -lcsfml-system -lcsfml-window -lcsfml-graphics

OBJ	=	$(SRCS:.c=.o)

%.o: %.c
	@gcc $(CFLAGS) -c $< -o $@ -Iinclude

all: $(NAME)

.PHONY: all clean fclean re

$(NAME): $(OBJ)
	@gcc $(CFLAGS) -o $(NAME) $(OBJ) $(LIBS)

clean:
	@rm -f $(OBJ)

fclean: clean
	@rm -f $(NAME)

re: fclean all