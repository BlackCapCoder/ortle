#
# Makefile for ortle
#


target   := ortle
bold     := $(shell tput bold)
reset    := $(shell tput sgr0)


CXX      ?= g++
CXXFLAGS += -std=c++11 -Wall -Wextra -pedantic # -pg
# LDFLAGS  += -pg
LIBS     := -lX11 -lXcomposite -lXdamage -lXext -lXfixes -lGL


SOURCES  := $(wildcard source/*/*.cpp)
SOURCES  += $(wildcard source/*.cpp)

OBJECTS  := $(SOURCES:.cpp=.o)




.PHONY: all clean debug


all: CXXFLAGS += -Ofast -O3 -frename-registers -funroll-loops -DNDEBUG
all: $(target)


clean:
	@ echo "$(bold)Cleaning up...$(reset)"
	@ rm -fv $(OBJECTS)
	@ rm -fv $(target)


debug: CXXFLAGS += -O0 -g -DDEBUG
debug: CXXFLAGS += -DUTILITY_BACKTRACE_DEMANGLE_CPP
debug: LDFLAGS  += -rdynamic
debug: $(target)



$(target): $(OBJECTS)
	@ echo "Linking $(bold)$(target)$(reset)..."
	@ $(CXX) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS)


%.o: %.cpp
	@ echo "Compiling $(bold)$<$(reset)..."
	@ $(CXX) $(CXXFLAGS) -o $@ -c $<

