


TARGETS :=test



SRCS     := $(wildcard *.c *.h)
OBJS     := $(patsubst %.c,%.o $(SRCS))

INCLUDES := -I ../ex3/lua_inc -I ./

LIB_DIRS := -L ../ex3/lua_lib 

LIBS     := -lpthread -llua -lm -ldl


$(TARGETS):
	gcc -o $(TARGETS) $(SRCS)  $(INCLUDES) $(LIB_DIRS)  (LIBS)


clean:
	rm -rf $(TARGETS)

