export LANG = en

CC = gcc
RM = rm -f
CFLAGS = 
LIBS = 
OUTPUT_DIR = ./
OBJ_DIR = ./debug/obj
TARGETS = $(OUTPUT_DIR)/sundown_pygmentize_glue

OBJS = \
    $(OBJ_DIR)/sundown_pygmentize_glue.o
    
all: $(TARGETS)

$(OUTPUT_DIR)/sundown_pygmentize_glue: $(OBJS)
    @$(CC) -o $(OUTPUT_DIR)/sundown_pygmentize_glue $(OBJS) $(LIBS)

clean:
	$(RM) $(TARGETS) $(OBJS)

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@
    
    
