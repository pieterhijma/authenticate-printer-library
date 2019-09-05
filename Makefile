CC		= gcc
CFLAGS		= -g -Wall `cups-config --cflags` -fpic
LFLAGS		= `cups-config --libs`
SRC		= authenticate_printer.c
OBJ		= $(SRC:.c=.o)
LIBNAME		= libauthenticateprinter.so

DEPFILE		= .depends
DEPFLAGS	= -Y -f$(DEPFILE)


$(LIBNAME): $(OBJ)
	$(CC) $(LFLAGS) -shared -o $@ $^


%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean: 
	rm -f $(LIBNAME) *.o

