SRC_LEX = input_scanner.lex
SRC_BIS = input_parser.y
SRC_C = common.c
SRC_CC =

SRCDIR = ../src
OBJDIR = ../obj
BINDIR = ../bin
OUTFILE = $(BINDIR)/comp

CCX = g++
CFLAGS = -Wall -I$(SRCDIR) -I$(OBJDIR) -g
LFLAGS = -Wall
COMPILE = $(CCX) $(CFLAGS)
LINK = $(CCX) $(LFLAGS)
RM = @rm -rf

GENSRC_LEX = $(SRC_LEX:%.lex=$(OBJDIR)/%.c)
GENSRC_BIS = $(SRC_BIS:%.y=$(OBJDIR)/%.c)
OBJS =
OBJS += $(GENSRC_LEX:%.c=%.o)
OBJS += $(GENSRC_BIS:%.c=%.o)
OBJS += $(SRC_C:%.c=$(OBJDIR)/%.o)
OBJS += $(SRC_CC:%.cc=$(OBJDIR)/%.o)

all: $(GENSRC_BIS) $(GENSRC_LEX) $(OBJS)
	$(LINK) $(OBJS) -o $(OUTFILE)

$(GENSRC_LEX): $(SRC_LEX)
	flex -o $@ $<

$(GENSRC_BIS): $(SRC_BIS)
	bison -dv -o $@ $<

$(OBJDIR)/%.o: $(OBJDIR)/%.c
	$(COMPILE) -c $+ -o $@

$(OBJDIR)/%.o: %.c
	$(COMPILE) -c $+ -o $@

$(OBJDIR)/%.o: %.cc
	$(COMPILE) -c $+ -o $@

$(OBJDIR)/%.o: %.cpp
	$(COMPILE) -c $+ -o $@


clean:
	$(RM) $(OBJDIR)/*
	$(RM) $(BINDIR)/*

pack:
	tar --exclude-vcs --exclude=$(OBJDIR)/* --exclude=$(BINDIR)/* -jcvf $(OUTFILE).tar.bz2 ../

