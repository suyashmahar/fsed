SRCDIR := src
TESTDIR := test
OBJDIR := obj
MAIN := $(SRCDIR)/main.c
TEST_BIN := $(TESTDIR)/test.c

SRCS := $(filter-out $(MAIN), $(wildcard $(SRCDIR)/*.c))
OBJS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

CFLAGS += 

.PHONY: all clean depend debug backend

# all: Default compilation rule, generates binary with optimzation, not suitable for debugging
all: CFLAGS += -O3 -march=native
all: backend

# debug: Generates a binary which is easier to debug
debug: CFLAGS += -O0 -g
debug: backend

# Actual compilation is handled by function past this comment
backend: depend fsed testbin

testbin: testbingen

testbingen: $(TEST_BIN)
	$(CC) $(CFLAGS) $(TEST_BIN) -o testbin
clean:
	rm -f fsed testbin
	rm -rf $(OBJDIR)

depend: $(OBJDIR)/.depend


$(OBJDIR)/.depend: $(SRCS)
	@mkdir -p $(OBJDIR)
	@rm -f $(OBJDIR)/.depend
	@$(foreach SRC, $(SRCS), $(CC) $(CFLAGS) -Dfsed -MM -MT $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC)) $(SRC) >> $(OBJDIR)/.depend ;)

ifneq ($(MAKECMDGOALS),clean)
-include $(OBJDIR)/.depend
endif


fsed: $(MAIN) $(OBJS) $(SRCDIR)/*.h | depend
	$(CC) $(CFLAGS) -Dfsed -o $@ $(MAIN) $(OBJS)

$(OBJS): | $(OBJDIR)

$(OBJDIR): 
	@mkdir -p $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -Dfsed -c -o $@ $<
