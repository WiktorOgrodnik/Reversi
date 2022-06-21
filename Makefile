CC = g++
SRC_DIR = reversi-src
INC_DIR = .
OBJ_DIR = reversi-obj
CFLAGS = -std=c++17 -g -pthread -Wall -Wextra -Wpedantic -O3
NAME = reversi
NAME_TEST = reversi_test
NAME_GEN = reversi_gen
NAME_ARENA = reversi_arena
NAME_TURNIEJ = reversi_turniej
OBJS = $(addprefix $(OBJ_DIR)/, types.o field.o board.o bots.o reversi.o)
OBJS_TEST = $(addprefix $(OBJ_DIR)/, types.o field.o board.o bots.o reversi_test.o)
OBJS_GEN = $(addprefix $(OBJ_DIR)/, types.o field.o board.o bots.o reversi_gen.o)
OBJS_ARENA = $(addprefix $(OBJ_DIR)/, types.o field.o board.o bots.o reversi_arena.o)
OBJS_TURNIEJ = $(addprefix $(OBJ_DIR)/, types.o field.o board.o bots.o reversi_turniej.o)
SFML = -lsfml-graphics -lsfml-window -lsfml-system

all: game test gen arena turniej

game: pre $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(SFML)

test: pre $(OBJS_TEST)
	@$(CC) $(CFLAGS) $(OBJS_TEST) -o $(NAME_TEST)

gen: pre $(OBJS_GEN)
	@$(CC) $(CFLAGS) $(OBJS_GEN) -o $(NAME_GEN)

arena: pre $(OBJS_ARENA)
	@$(CC) $(CFLAGS) $(OBJS_ARENA) -o $(NAME_ARENA)

turniej: pre $(OBJS_TURNIEJ)
	@$(CC) $(CFLAGS) $(OBJS_TURNIEJ) -o $(NAME_TURNIEJ)

pre:
	@if [ ! -d "$(OBJ_DIR)" ]; then mkdir $(OBJ_DIR); fi;

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(CC) $(CFLAGS) -c -I$(INC_DIR) $< -o $@

clean:
	@if [ -d "$(OBJ_DIR)" ]; then rm -r -f $(OBJ_DIR); fi;

distclean: clean
	@rm -f $(NAME) $(NAME_TEST) $(NAME_GEN) $(NAME_ARENA) $(NAME_TURNIEJ)