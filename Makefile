CC = cc
TARGET = ksh
SRC = $(wildcard *.c)
SRC += $(wildcard lib/*.c)
OBJ = ${SRC:.c=.o}

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ -lreadline
 
%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o lib/*.o $(TARGET)

install: $(TARGET)
	cp -f ${TARGET} /usr/bin
	rm -f *.o lib/*.o $(TARGET)
 
uninstall:
	rm -r /usr/bin/${TARGET}

.PHONY: clean install uninstall
