CXX = g++
CXXFLAGS = -std=c++23 -O3 -march=native -Wall -Wextra
OPENMP_FLAG = -fopenmp
TARGET = main
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(OPENMP_FLAG) -o $(TARGET) $(SRC)
	@echo "Built $(TARGET) with OpenMP support"

run: $(TARGET)
	@./$(TARGET)

benchmark: $(TARGET)
	@echo "=== Тест производительности ==="
	@echo "Массив из 100'000'000 элементов"
	@echo ""
	@echo "1. Последовательный режим:"
	@./$(TARGET) --sequential --size 100000000 | grep "Сomputation time"
	@echo ""
	@echo "2. Параллельный режим (2 потока):"
	@./$(TARGET) --parallel --threads 2 --size 100000000 | grep -E "Сomputation time|Sequential|Speedup"
	@echo ""
	@echo "3. Параллельный режим (4 потока):"
	@./$(TARGET) --parallel --threads 4 --size 100000000 | grep -E "Сomputation time|Sequential|Speedup"
	@echo ""
	@echo "4. Параллельный режим (8 потоков):"
	@./$(TARGET) --parallel --threads 8 --size 100000000 | grep -E "Сomputation time|Sequential|Speedup"

clean:
	rm -f $(TARGET) *.o

help:
	@echo "Available targets:"
	@echo "  make all           - Собрать программу (по умолчанию)"
	@echo "  make benchmark     - Запуск теста производительности"
	@echo "  make clean         - Удалить сгенерированные файлы"
	@echo "  make help          - Показать справку"

.PHONY: all benchmark clean help