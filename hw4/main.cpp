/**
 * Автор: Карташев Кирилл
 * Номер в таблиц студентов курса: 2
 * 
 * Остаток от деления номера на 2: 0
 * Начало инструкции: _mm
 * 
 * Остаток от деления номера на 4: 2
 * Конец инструкции: _pd
 * 
 * Частное от деления номера на 8 без остатка: 0
 * Середина инструкции: _add
 * 
 * Итоговая инструкция: _mm_add_pd
 */

#include <iostream>
#include <chrono>
#include <cmath>
#include <immintrin.h>

// Обычное последовательное сложение
void simd_add(double* a, double* b, double* result, size_t n){
    for(size_t i = 0; i < n; i++){
        result[i] = a[i] + b[i];
    }
}

// SIMD сложение с использованием _mm_add_pd
void simd_add(const double* a, const double* b, double* result, size_t n){
    size_t i = 0;
    
    // Разворачиваем цикл для лучшей производительности
    for(; i + 7 < n; i += 8){
        // Обрабатываем 8 значений за итерацию
        __m128d a0 = _mm_load_pd(a + i);
        __m128d b0 = _mm_load_pd(b + i);
        __m128d r0 = _mm_add_pd(a0, b0);
        _mm_store_pd(result + i, r0);
        
        __m128d a1 = _mm_load_pd(a + i + 2);
        __m128d b1 = _mm_load_pd(b + i + 2);
        __m128d r1 = _mm_add_pd(a1, b1);
        _mm_store_pd(result + i + 2, r1);
        
        __m128d a2 = _mm_load_pd(a + i + 4);
        __m128d b2 = _mm_load_pd(b + i + 4);
        __m128d r2 = _mm_add_pd(a2, b2);
        _mm_store_pd(result + i + 4, r2);
        
        __m128d a3 = _mm_load_pd(a + i + 6);
        __m128d b3 = _mm_load_pd(b + i + 6);
        __m128d r3 = _mm_add_pd(a3, b3);
        _mm_store_pd(result + i + 6, r3);
    }
    
    // Обрабатываем оставшиеся элементы
    for(; i + 1 < n; i += 2){
        __m128d a_vec = _mm_load_pd(a + i);
        __m128d b_vec = _mm_load_pd(b + i);
        __m128d r_vec = _mm_add_pd(a_vec, b_vec);
        _mm_store_pd(result + i, r_vec);
    }
    
    // Последний элемент, если количество нечетное
    if(i < n){
        result[i] = a[i] + b[i];
    }
}

// AVX версия (если процессор поддерживает)
#ifdef __AVX__
void avx_add(const double* a, const double* b, double* result, size_t n){
    size_t i = 0;
    
    for(; i + 7 < n; i += 8){
        // Обрабатываем 4 double за раз (256-битный регистр)
        __m256d a0 = _mm256_load_pd(a + i);
        __m256d b0 = _mm256_load_pd(b + i);
        __m256d r0 = _mm256_add_pd(a0, b0);
        _mm256_store_pd(result + i, r0);
        
        __m256d a1 = _mm256_load_pd(a + i + 4);
        __m256d b1 = _mm256_load_pd(b + i + 4);
        __m256d r1 = _mm256_add_pd(a1, b1);
        _mm256_store_pd(result + i + 4, r1);
    }
    
    // Оставшиеся элементы SSE
    for(; i + 3 < n; i += 4){
        __m128d a0 = _mm_load_pd(a + i);
        __m128d b0 = _mm_load_pd(b + i);
        __m128d r0 = _mm_add_pd(a0, b0);
        _mm_store_pd(result + i, r0);
        
        __m128d a1 = _mm_load_pd(a + i + 2);
        __m128d b1 = _mm_load_pd(b + i + 2);
        __m128d r1 = _mm_add_pd(a1, b1);
        _mm_store_pd(result + i + 2, r1);
    }
    
    // Последние 1-2 элемента
    for(; i < n; i++){
        result[i] = a[i] + b[i];
    }
}
#endif

int main(int argc, char* argv[]){
    //Сопроводительная информация, из комментария в начале файла
    std::cout << "Номер студента: 2\n";
    std::cout << "Демонстрация полезности следующей инструкции: _mm_add_pd\n";
    std::cout << "Дополнительно, если есть поддержка AVX: _mm256_add_pd\n";

    // Параметры программы
    size_t array_size = 1000000;

    // Можно указать размер массива
    if(argc >= 2){
        array_size = std::stoul(argv[1]);
    }
    
    // Выделяем выровненную память для SIMD операций (16-байтовое выравнивание)
    double* a = (double*)_mm_malloc(array_size * sizeof(double), 16);
    double* b = (double*)_mm_malloc(array_size * sizeof(double), 16);
    double* result_sequential = (double*)_mm_malloc(array_size * sizeof(double), 16);
    double* result_sse = (double*)_mm_malloc(array_size * sizeof(double), 16);
    
    // Инициализируем массивы
    for(size_t i = 0; i < array_size; i++){
        a[i] = std::sin(i * 0.001);
        b[i] = std::cos(i * 0.002);
    }
    
    // Тестируем обычное сложение
    auto start_sequential = std::chrono::high_resolution_clock::now();
    simd_add(a, b, result_sequential, array_size);
    auto end_sequential = std::chrono::high_resolution_clock::now();
    auto duration_sequential = std::chrono::duration_cast<std::chrono::milliseconds>(end_sequential - start_sequential);
    
    // Тестируем SIMD-SSE сложение
    auto start_sse = std::chrono::high_resolution_clock::now();
    simd_add(a, b, result_sse, array_size);
    auto end_sse = std::chrono::high_resolution_clock::now();
    auto duration_sse = std::chrono::duration_cast<std::chrono::milliseconds>(end_sse - start_sse);

#ifdef __AVX__
    // Тестируем SIMD-AVX сложение
    auto start_avx = std::chrono::high_resolution_clock::now();
    avx_add(a, b, result_sse, array_size);
    auto end_avx = std::chrono::high_resolution_clock::now();
    auto duration_avx = std::chrono::duration_cast<std::chrono::milliseconds>(end_avx - start_avx);
#endif
    
    // Проверяем корректность (сравниваем результаты)
    bool correct = true;
    for(size_t i = 0; i < array_size; i++){
        if(std::abs(result_sequential[i] - result_sse[i]) > 1e-10){
            correct = false;
            break;
        }
    }
    
    // Выводим результаты
    std::cout << "Array size: " << array_size << " double elements\n";
    std::cout << "Sequential add: " << duration_sequential << std::endl;
    std::cout << "SIMD-SSE add (_mm_add_pd): " << duration_sse << std::endl;
#ifdef __AVX__
    std::cout << "SIMD-AVX add (_mm256_add_pd): " << duration_avx << std::endl;
#endif
    std::cout << "Speedup with SSE: " << (double)duration_sequential.count() / duration_sse.count() << "x\n";
#ifdef __AVX__
    std::cout << "Speedup with AVX: " << (double)duration_sequential.count() / duration_avx.count() << "x\n";
#endif
    std::cout << "Results " << (correct ? "match" : "does not match") << std::endl;
    
    // Освобождаем память
    _mm_free(a);
    _mm_free(b);
    _mm_free(result_sequential);
    _mm_free(result_sse);
    
    return 0;
}