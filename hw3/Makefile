CXX = g++
CXXFLAGS = -std=c++23 -pthread -O3 -Wall -Wextra
TARGET = main
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

run-custom: $(TARGET)
	./$(TARGET) $(ARRAY_SIZE) $(NUM_THREADS)

clean:
	rm -f $(TARGET)

benchmark: $(TARGET)
	@echo "=== Тест производительности ==="
	@echo "Массив 1000 элементов, 2 потока:"
	@./$(TARGET) 1000 2 | grep -E "(Program execution time|Speedup)"
	@echo ""
	@echo "Массив 10'000 элементов, 2 потока:"
	@./$(TARGET) 10000 2 | grep -E "(Program execution time|Speedup)"
	@echo ""
	@echo "Массив 100'000 элементов, 2 потока:"
	@./$(TARGET) 100000 2 | grep -E "(Program execution time|Speedup)"
	@echo ""
	@echo "Массив 1'000'000 элементов, 2 потока:"
	@./$(TARGET) 1000000 2 | grep -E "(Program execution time|Speedup)"
	@echo ""
	@echo "Массив 10'000'000 элементов, 2 потока:"
	@./$(TARGET) 10000000 2 | grep -E "(Program execution time|Speedup)"
	@echo ""
	@echo "Массив 100'000'000 элементов, 2 потока:"
	@./$(TARGET) 100000000 2 | grep -E "(Program execution time|Speedup)"

.PHONY: all run run-custom clean benchmark