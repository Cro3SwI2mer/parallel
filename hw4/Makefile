CXX = g++
CXXFLAGS = -std=c++23 -O2 -march=native -msse2 -Wall -Wextra
LDFLAGS = 
TARGET = main
SRC = main.cpp

AVX_FLAG = $(shell $(CXX) -march=native -dM -E - < /dev/null | grep -q "__AVX__" && echo "-mavx")

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(AVX_FLAG) -o $(TARGET) $(SRC)

run: $(TARGET)
	@./$(TARGET)

run-custom: $(TARGET)
	./$(TARGET) $(ARRAY_SIZE)

debug:
	$(CXX) -O0 -g -msse2 $(AVX_FLAG) -o $(TARGET)_debug $(SRC)

check-cpu:
	@echo "Проверка поддержки SIMD инструкций:"
	@if command -v lscpu > /dev/null; then \
		lscpu | grep -E "SSE|AVX|SIMD"; \
	elif sysctl -a 2>/dev/null | grep -q machdep.cpu; then \
		sysctl -a | grep -E "sse|avx"; \
	else \
		echo "Используйте cat /proc/cpuinfo на Linux для проверки"; \
	fi

assembly:
	$(CXX) $(CXXFLAGS) -S -masm=intel $(SRC) -o $(TARGET).asm
	@echo "Ассемблерный код сгенерирован в $(TARGET).asm"

clean:
	rm -f $(TARGET) $(TARGET)_debug $(TARGET)_release *.o *.asm sequential.asm simd.asm

benchmark: $(TARGET)
	@echo "=== Тест производительности ==="
	@echo "Массив 1'000'000 элементов"
	@./$(TARGET) 1000000 | grep -E "(Sequential add|SIMD-SSE add|SIMD-AVX add|Speedup with SSE|Speedup with AVX)"
	@echo ""
	@echo "Массив 10'000'000 элементов"
	@./$(TARGET) 10000000 | grep -E "(Sequential add|SIMD-SSE add|SIMD-AVX add|Speedup with SSE|Speedup with AVX)"
	@echo ""
	@echo "Массив 100'000'000 элементов"
	@./$(TARGET) 100000000 | grep -E "(Sequential add|SIMD-SSE add|SIMD-AVX add|Speedup with SSE|Speedup with AVX)"

help:
	@echo "Доступные команды:"
	@echo "  make all           - Собрать программу (по умолчанию)"
	@echo "  make run           - Собрать и запустить программу"
	@echo "  make run-custom    - Собрать и запустить программу с указанием параметра ARRAY_SIZE"
	@echo "  make debug         - Собрать с отладочной информацией"
	@echo "  make check-cpu     - Проверить поддержку SIMD инструкций"
	@echo "  make assembly      - Сгенерировать ассемблерный код"
	@echo "  make clean         - Очистить сгенерированные файлы"
	@echo "  make benchmark     - Тест на производительность с разным размером входного массива"
	@echo "  make help          - Показать справку"

.PHONY: all run run-custom debug check-cpu assembly clean benchmark help