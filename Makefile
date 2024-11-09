.PHONY: clean

SRC_DIR = src

TARGET = app.elf
COMPILER = g++
EXTERNAL = third-party

SOURCE_FILES := \
	$(wildcard $(SRC_DIR)/*.cpp)\
	$(wildcard $(SRC_DIR)/*/*.cpp)

OBJECT_FILES := $(patsubst %.cpp,%.o,$(SOURCE_FILES))

CFLAGS = -pedantic -Iinclude -I$(EXTERNAL)/raylib/include -I$(EXTERNAL)/raygui/src
LDFLAGS = -Wl,-rpath=$(EXTERNAL)/raylib/lib -L$(EXTERNAL)/raylib/lib -lraylib -O3 -DNDEBUG -s

RM = rm -f
MKDIR = mkdir -p

%.o: %.cpp
	@$(COMPILER) -c -o $@ $< $(CFLAGS)
	$(info [$(COMPILER)] Compilling $< -> $@...)

$(TARGET): $(OBJECT_FILES)
	@$(COMPILER) -o $@ $^ $(LDFLAGS)
	$(info Generating $(TARGET) executable...)

$(BIN_DIR):
	@$(MKDIR) $(BIN_DIR)
	$(info Generate bin directory...)

clean:
	$(info Cleaning...)
	@$(RM) $(TARGET) $(OBJECT_FILES)

