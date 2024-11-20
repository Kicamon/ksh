CC = cc
TARGET = ksh
SRC = $(wildcard src/*.c)
OBJ = ${SRC:.c=.o}

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ -lreadline
 
%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f lib/*.o $(TARGET)

install: $(TARGET)
	mv $(OBJ) ./lib
	cp -f ${TARGET} /usr/bin

test: $(TARGET)
	mv $(OBJ) ./lib
	./$(TARGET)
 
uninstall:
	rm -r /usr/bin/${TARGET}

.PHONY: test clean install uninstall
