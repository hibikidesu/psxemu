PREFIX	:= 
CC		:= $(PREFIX)gcc
STRIP	:= $(PREFIX)strip

TARGET	:= psxemu
DEBUG	:= 1
CFLAGS	:= -Wall -std=c99 -DDEBUG=$(DEBUG)

# Extra debug flags
ifeq ($(DEBUG), 1)
CFLAGS	+= -ggdb3
else
CFLAGS	+= -O2
endif

LIBS	:= $(shell pkg-config --static --libs sdl2 glew)
SRC		:= $(shell find src -name '*.c')
OBJS	:= $(patsubst %.c,%.o,$(SRC))

.PHONY:	\
	all	\
	clean	\

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@
ifeq ($(DEBUG), 0)
	$(STRIP) $@
endif

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET)
