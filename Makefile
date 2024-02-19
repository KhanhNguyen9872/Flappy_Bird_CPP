# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -s -Ofast -flto=auto -march=native -Walloc-size-larger-than=18446744073709551615

SRC = src/flappy.cpp

EXEC = flappy

BUILD_DIR = build

default:
	@echo "Use [make termux] for Termux or [make linux] for Linux"

termux:
	@echo  "Build for Termux"
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && printf "#define _TERMUX\n" > tmp.cpp 2>/dev/null && cat ../$(SRC) >> tmp.cpp && $(CXX) $(CXXFLAGS) tmp.cpp -o $(EXEC)
	@mv $(BUILD_DIR)/$(EXEC) $(EXEC)
	@make clean
	@chmod 777 $(EXEC)
	@echo "Completed!"
	@echo "./flappy # run flappy"
linux:
	@echo  "Build for Linux"
	@mkdir -p $(BUILD_DIR) >/dev/null 2>&1
	@cd $(BUILD_DIR) && $(CXX) $(CXXFLAGS) ../$(SRC) -o $(EXEC)
	@mv $(BUILD_DIR)/$(EXEC) $(EXEC)
	@make clean
	@chmod 777 $(EXEC)
	@echo "Completed"
	@echo "./flappy # run flappy"

clean:
	@echo "Remove" $(BUILD_DIR) "folder..."
	@rm -rf $(BUILD_DIR)
