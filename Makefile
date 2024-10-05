CC = cc
TARGET = ksh
SRC = ksh.c lib/utools.c
OBJ = ${SRC:.c=.o}
 
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ -lreadline
 
%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o lib/*.o $(TARGET)

install:
	cp -f ${TARGET} /usr/bin
 
uninstall:
	rm -r /usr/bin/${TARGET}

.PHONY: clean install uninstall
