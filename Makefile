CC = cc
TARGET = Main
SRC = $(wildcard *.c)
SRC += $(wildcard lib/*.c)
OBJ = $(patsubst %ac, %.o,$(SRC))
 
$(TARGET): $(OBJ)
	$(CC) -o $@ $^
 
%.o: %.c
	$(CC) -c $< -o $@
 
.PHONY: clean
clean:
	rm -f *.o $(TARGET)
