SRC_C = rx.c

SRCDIR = ../src/rxemu
OBJDIR = ../../obj
BINDIR = ../../bin
OUTFILE = $(BINDIR)/rx

CCX = gcc
CFLAGS = -Wall -g
LFLAGS = -Wall
COMPILE = $(CCX) $(CFLAGS)
LINK = $(CCX) $(LFLAGS)
RM = @rm -rf

OBJS =
OBJS += $(SRC_C:%.c=$(OBJDIR)/%.o)

all: $(OBJS)
	$(LINK) $(OBJS) -o $(OUTFILE)

$(OBJDIR)/%.o: %.c
	$(COMPILE) -c $+ -o $@


clean:
	$(RM) $(OBJS)
	$(RM) $(OUTFILE)

