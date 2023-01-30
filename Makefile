CXX = g++
LD = g++
CXX_FLAGS = -O0 -g -Isrc/include -I/usr/include/freetype2
LD_FLAGS = 
LD_LIBS = -lGL -lglfw -lfreetype -lutil

TARGET_NAME = baltar
CXX_SRCS = src/Main.cpp src/Debug.cpp src/GlyphManager.cpp src/Shader.cpp \
	src/ShaderSources.cpp src/Terminal.cpp \
	src/3rdparty/Nanoprintf.cpp src/3rdparty/Glad.cpp

# auto-populated vars
TARGET = build/$(TARGET_NAME).elf
OBJS = $(patsubst %.cpp, build/%.cpp.o, $(CXX_SRCS))

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking ..."
	@$(LD) $(OBJS) $(LD_FLAGS) $(LD_LIBS) -o $(TARGET)
	@echo "Done, output @ $(TARGET)"

.PHONY: clean
clean:
	@-rm -r build
	@echo "Cleaned build directories."

.PHONY: run
run: $(TARGET)
	@./$(TARGET)

.PHONY: debug
debug: $(TARGET)
	@gdb $(TARGET)

build/%.cpp.o: %.cpp
	@echo "Compiling $<"
	@mkdir -p $(@D)
	@$(CXX) $(CXX_FLAGS) -c $< -o $@
