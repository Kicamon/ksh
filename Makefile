CC = cc
TARGET = ksh
SRC = $(wildcard src/*.c)
OBJ = ${SRC:.c=.o}

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ -lreadline
 
%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o src/*.o $(TARGET)

install: $(TARGET)
	cp -f ${TARGET} /usr/bin

test: $(TARGET)
	./$(TARGET)
 
uninstall:
	rm -r /usr/bin/${TARGET}

.PHONY: test clean install uninstall
