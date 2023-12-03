CFLAGS:= -Wall -Wextra -pedantic -Iinclude -Ibdwgc/include
CFLAGSDEBUG:= -DDEBUG -g
STRICT_CFLAGS:= -Werror

BUILD:=build
OBJDIR:=build
INCLUDE:=include
SRCDIR:=src
TESTSDIR:=tests
DEBUGDIR:=$(BUILD)/debug
DEBUGOBJDIR:=$(BUILD)/debug


HEADERS:=$(wildcard include/*.h)
SRCS:=$(wildcard $(SRCDIR)/*.c)
OBJ:=$(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEBUGOBJ:=$(SRCS:$(SRCDIR)/%.c=$(DEBUGOBJDIR)/%.o)

creg-debug: creg.c $(DEBUGOBJ)
	$(CC) $(CFLAGSDEBUG) $(CFLAGS) -I$(INCLUDE) \
		-o $(BUILD)/$@ $^

creg: creg.c $(OBJ)
	$(CC) -O3 $(STRICT_CFLAGS) $(CFLAGS) -I$(INCLUDE) \
		-o $(BUILD)/$@ $^


$(DEBUGOBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(CC) $(CFLAGSDEBUG) $(STRICT_CFLAGS) $(CFLAGS) -I$(INCLUDE) -c -o $@  $<

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(CC) $(STRICT_CFLAGS) $(CFLAGS) -I$(INCLUDE) -c -o $@  $<

$(TESTSDIR)/%: $(TESTSDIR)/%.c $(OBJ)
	$(CC) $(STRICT_CFLAGS) $(CFLAGS) -I$(INCLUDE) \
		-o $(BUILD)/$@ $^

clean:
	find $(BUILD) -type f -delete


tags: $(HEADERS) $(SRCS)
	ctags -R .

cscope:
	cscope -b -k -R
