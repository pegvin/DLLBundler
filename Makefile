CXX = g++
LN = g++
STRIP = strip
FIND = find src -type f -name
BIN = dllbundler.exe
DEBUG = true
POSTBUILD =

FLAGS := -Isrc -Wall -MMD -MP
CPP_FLAGS := --std=c++17
LN_FLAGS :=
ODIR = build
OBJ = $(patsubst %,$(ODIR)/%.o,$(shell $(FIND) '*.cpp'))
DEP := $(OBJ:.o=.d)

ifeq ($(DEBUG),true)
	FLAGS += -O0 -g
else
	FLAGS += -O3
	POSTBUILD = $(STRIP) $(BIN)
endif

$(ODIR)/%.cpp.o: %.cpp
	$(info CXX - $<)
	@mkdir -p "$$(dirname "$@")"
	@$(CXX) -c $< -o $@ $(FLAGS) $(CPP_FLAGS)

$(BIN): $(OBJ)
	$(info Linking $@)
	@$(LN) $^ -o $@ $(FLAGS) $(LN_FLAGS)
	@$(POSTBUILD)

.PHONY: clean

all: $(BIN)

clean:
	rm -rf $(ODIR)

-include $(DEP)

