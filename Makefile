OBJ = readdata.o rainier.o safe_func.o

world: compile

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

compile: $(OBJ)
	$(CC) -o rainier $(OBJ)

clean:
	rm -f *.o *.so rainier 