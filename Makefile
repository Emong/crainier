OBJ = readdata.o rainier.o safe_func.o calfunc.o ck_model.o nrand.o cell_loop.o next_cell.o
LIBS = -lm
CFLAGS += 

world: compile

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

compile: $(OBJ)
	$(CC) -o rainier $(OBJ) $(LIBS) 

clean:
	rm -f *.o *.so rainier 
