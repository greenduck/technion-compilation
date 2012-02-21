SRC_LEX = input_scanner.lex
SRC_BIS = input_parser.y
SRC_C =
SRC_CC = utils.cc symbol.cc code.cc semantic_rules.cc backpatch.cc linked_list.cc storage.cc

ifndef unittest
	SRC_CC += main.cc
else
	SRC_CC += unittest.cc
endif

SRCDIR = ../src
OBJDIR = ../obj
BINDIR = ../bin
OUTFILE = $(BINDIR)/rcc

CCX = @g++
CFLAGS = -Wall -I$(SRCDIR) -I$(OBJDIR) -g
LFLAGS = -Wall -rdynamic
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

all: toycompiler rxemu

.PHONY: rxemu
rxemu:
	@$(MAKE) -C rxemu

toycompiler: $(GENSRC_BIS) $(GENSRC_LEX) $(OBJS)
	$(LINK) $(OBJS) -o $(OUTFILE)

$(GENSRC_LEX): $(SRC_LEX)
	@flex -o$@ $<

$(GENSRC_BIS): $(SRC_BIS)
	@bison -dv -o $@ $<

$(OBJDIR)/%.o: $(OBJDIR)/%.c
	$(COMPILE) -c $+ -o $@

$(OBJDIR)/%.o: %.c
	$(COMPILE) -c $+ -o $@

$(OBJDIR)/%.o: %.cc
	$(COMPILE) -c $+ -o $@

$(OBJDIR)/%.o: %.cpp
	$(COMPILE) -c $+ -o $@

.PHONY: clean
clean:
	$(RM) $(OBJDIR)/*
	$(RM) $(BINDIR)/*

.PHONY: pack
pack:
	tar --exclude-vcs --exclude=$(OBJDIR)/* --exclude=$(BINDIR)/* -jcvf $(OUTFILE).tar.bz2 ../

