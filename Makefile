CFLAGS:=-g -Wall -Wextra -pedantic -Iinclude -Ibdwgc/include
STRICT_CFLAGS:= #-Werror

BUILD:=build
OBJDIR:=build
INCLUDE:=include
SRCDIR:=src
TESTSDIR:=tests

HEADERS:=$(wildcard include/*.h)
SRCS:=$(wildcard $(SRCDIR)/*.c)
OBJ:=$(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

creg: creg.c $(OBJ)
	$(CC) $(STRICT_CFLAGS) $(CFLAGS) -I$(INCLUDE) \
		-o $(BUILD)/$@ $^


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
