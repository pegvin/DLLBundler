CC = gcc
CXX = g++
OBJC = g++
LN = g++
STRIP = strip
FIND = find src -type f -name
PKGCONFIG = pkg-config
BIN = dllbundler.exe

FLAGS := -Isrc -Wall -MMD -MP
C_FLAGS := --std=c99
CPP_FLAGS := --std=c++17
LN_FLAGS :=
ODIR = build
OBJ = $(patsubst %,$(ODIR)/%.o,$(shell $(FIND) '*.cpp'))
DEP := $(OBJ:.o=.d)

ifeq ($(DEBUG),true)
	FLAGS += -O0 -g -D_DEBUG
else
	FLAGS += -O3
endif

$(ODIR)/%.cpp.o: %.cpp
	$(info CXX - $<)
	@mkdir -p "$$(dirname "$@")"
	@$(CXX) -c $< -o $@ $(FLAGS) $(CPP_FLAGS)

$(BIN): $(OBJ)
	$(info Linking $@)
	@$(LN) $^ -o $@ $(FLAGS) $(LN_FLAGS)

.PHONY: clean

all: $(BIN)

clean:
	rm -rf $(ODIR)

-include $(DEP)

