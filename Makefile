# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = 

SRC = src/flappy.cpp

EXEC = flappy

BUILD_DIR = build

default:
	@echo "Use [make termux] for Termux or [make linux] for Linux"

termux:
	@echo  "Build for Termux"
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && printf "#define _TERMUX\n" > tmp.cpp 2>/dev/null && cat ../$(SRC) >> tmp.cpp && $(CXX) $(CXXFLAGS) tmp.cpp -o $(EXEC)
	@rm -rf $(BUILD_DIR)/tmp.cpp
	@chmod 777 $(BUILD_DIR)/$(EXEC)
	@echo "Completed"
linux:
	@echo  "Build for Linux"
	@mkdir -p $(BUILD_DIR) >/dev/null 2>&1
	@cd $(BUILD_DIR) && $(CXX) $(CXXFLAGS) ../$(SRC) -o $(EXEC)
	@chmod 777 $(BUILD_DIR)/$(EXEC)
	@echo "Completed"

clean:
	@rm -rf $(BUILD_DIR)

