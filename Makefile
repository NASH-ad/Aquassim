NAME	=	aquassim

CFLAGS	=	-Wall -Wextra -g

SRCS	=	src/main.c \
			src/init.c \
			src/clean.c \
			src/engine/vector.c \
			src/engine/rigidbody.c \
			src/engine/collision_detection.c \
			src/engine/physics_engine.c \
			src/engine/ecs.c \
			src/engine/systems.c \
			src/entities/ball.c \

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