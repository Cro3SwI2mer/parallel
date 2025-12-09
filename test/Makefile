CXX = g++
CXXFLAGS = -std=c++23 -O2 -march=native -msse2 -Wall -Wextra
LDFLAGS = 
TARGET = main
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	@./$(TARGET)

run-custom: $(TARGET)
	./$(TARGET) $(N)

clean:
	rm -f $(TARGET) *.o

benchmark: $(TARGET)
	@echo "=== Тест производительности ==="
	@echo ""
	@echo "N = 1'000'000"
	@./$(TARGET) 1000000 | grep -E "(Sequential execution time|SIMD-SSE execution time|Speedup)"
	@echo ""
	@echo "N = 10'000'000"
	@./$(TARGET) 10000000 | grep -E "(Sequential execution time|SIMD-SSE execution time|Speedup)"
	@echo ""
	@echo "N = 100'000'000"
	@./$(TARGET) 100000000 | grep -E "(Sequential execution time|SIMD-SSE execution time|Speedup)"
	@echo ""
	@echo "N = 1'000'000'000"
	@./$(TARGET) 1000000000 | grep -E "(Sequential execution time|SIMD-SSE execution time|Speedup)"

help:
	@echo "Доступные команды:"
	@echo "  make all           - Собрать программу (по умолчанию)"
	@echo "  make run           - Собрать и запустить программу"
	@echo "  make run-custom    - Собрать и запустить программу с указанием параметра N"
	@echo "  make clean         - Очистить сгенерированные файлы"
	@echo "  make benchmark     - Тест на производительность с разным размером входного N"
	@echo "  make help          - Показать справку"

.PHONY: all run run-custom clean benchmark help