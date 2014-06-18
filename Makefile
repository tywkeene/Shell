SHELL	     = /bin/sh
CC	     = gcc
FLAGS	     = -std=c11
CFLAGS	     = -Werror -I./include -D_POSIX_C_SOURCE=200112L
DEBUGFLAGS   = -O0 -g -DDEBUG -D_POSIX_C_SOURCE=200112L
RELEASEFLAGS = -O2
TARGET	     = shell
SOURCES	     = $(shell echo src/*.c)
HEADERS	     = $(shell echo include/*.h)
OBJECTS	     = $(SOURCES:.c=.o)
LIBCFLAGS    = 
LIBS	     = -ltecla

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

release: $(SOURCES) $(HEADERS)
	$(CC) $(FLAGS) $(CFLAGS) $(RELEASEFLAGS) $(LIBCFLAGS) $(LIBS) -o $(TARGET) $(SOURCES)

clean:
	-rm -f $(OBJECTS) $(TARGET)

distclean: clean
	-rm -f $(OBJECTS) $(TARGET)

.SECONDEXPANSION:

$(foreach OBJ,$(OBJECTS),$(eval $(OBJ)_DEPS = $(shell gcc -MM $(OBJ:.o=.c) | sed s/.*://)))
%.o: %.c $$($$@_DEPS)
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) $(LIBCFLAGS) -c -o $@ $<

.PHONY : all release \
  clean distclean
