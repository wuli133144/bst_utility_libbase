




TARGETS :=test



SRCS     := $(wildcard *.cc *.h)
OBJS     := $(patsubst %.cc,%.o, $(SRCS))

INCLUDES :=  -I ./

LIB_DIRS := -L  ./

LIBS     := -lpthread  -lcurl


$(TARGETS):
	gcc -std=c++11 -o $(TARGETS) $(SRCS)  $(INCLUDES) $(LIB_DIRS)  $(LIBS)


clean:
	rm -rf $(TARGETS)



