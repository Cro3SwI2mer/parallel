/**
 * Автор: Карташев Кирилл
 * Номер в таблиц студентов курса: 2
 * 
 * Задание: сумма шармонического ряда от 1 до n: 1/1 + 1/2 + ... + 1/n
 * 
 * Номер задания: (2 % 4) + 1 = 3
 * Использовать: 3) sse-инструкции
 * 
 * Дополнительные условия:
 * 
▶ Желательно, чтобы это была программа, собирающаяся так
или иначе в зависимости от директивы препроцессора.
Также, в вариантах, зависящих от количества ядер (таких
большинство), желательно задавать количество ядер также
через директиву препроцессора (используйте
NUM_THREADS в качестве переменной препроцессора)

▶ Нужно замерить время последовательного исполнения и
исполнения в параллельном режиме (задача вычислительно
простая, поэтому не исключено, что для некоторых режимов
параллелизации ускорения не будет или оно будет
отрицательное). Время желательно замерять при помощи
библиотеки std::chrono

▶ Программа по результатам работы должна выводить ваш
номер, время и получившуюся сумму с точностью до 20
знаков после запятой. Результаты работы могут отличаться в
разных режимах! Для уменьшения искажения желательно
суммировать в обратном порядке, от маленьких к большим.

▶ Код программы. Комментарии и/или отдельное описание
логики работы.

▶ Приведенные команды сборки программы и
соответствующие результаты (замер времени и
получившееся число). Очень желательно использовать
Makefile или cmake.

 */

#include <iostream>
#include <chrono>
#include <cmath>
#include <immintrin.h>

// Обычное последовательное вычисление
double sequential_calculation(size_t n){
    double result = 0.0;
    for(size_t i = 1; i <= n; i++){
        result += 1.0 / static_cast<double>(i);
    }
    return result;
}

// SIMD сложение с использованием _mm_add_pd
double simd_calculation(size_t n){
    double result = 0.0;
    size_t i = 1;

    // Аллоцируем память под временные массивы
    double* a = (double*)_mm_malloc(2 * sizeof(double), 16);
    double* b = (double*)_mm_malloc(2 * sizeof(double), 16);
    
    // Разворачиваем цикл для лучшей производительности
    for(; i + 7 <= n; i += 8){
        // Заполняем массивы значениями
        a[0] = 1.0 / static_cast<double>(i+0);
        a[1] = 1.0 / static_cast<double>(i+1);
        b[0] = 1.0 / static_cast<double>(i+2);
        b[1] = 1.0 / static_cast<double>(i+3);
        
        __m128d a0 = _mm_load_pd(a);
        __m128d b0 = _mm_load_pd(b);
        __m128d r0 = _mm_add_pd(a0, b0);
        // Горизонтальное сложение
        __m128d sum0 = _mm_hadd_pd(r0, r0);
        // Достаём low double и прибавляем к результату
        result += _mm_cvtsd_f64(sum0);

        // Заполняем массивы значениями
        a[0] = 1.0 / static_cast<double>(i+4);
        a[1] = 1.0 / static_cast<double>(i+5);
        b[0] = 1.0 / static_cast<double>(i+6);
        b[1] = 1.0 / static_cast<double>(i+7);
        
        __m128d a1 = _mm_load_pd(a);
        __m128d b1 = _mm_load_pd(b);
        __m128d r1 = _mm_add_pd(a1, b1);
        // Горизонтальное сложение
        __m128d sum1 = _mm_hadd_pd(r1, r1);
        // Достаём low double и прибавляем к результату
        result += _mm_cvtsd_f64(sum1);
    }
    
    // Обрабатываем оставшиеся элементы
    for(; i + 1 <= n; i += 2){
        // Заполняем массивы значениями
        a[0] = 1.0 / static_cast<double>(i+0);
        a[1] = 1.0 / static_cast<double>(i+1);
        __m128d a_loc = _mm_load_pd(a);
        // Горизонтальное сложение
        __m128d sum_loc = _mm_hadd_pd(a_loc, a_loc);
        // Достаём low double и прибавляем к результату
        result += _mm_cvtsd_f64(sum_loc);
    }
    
    // Последний элемент, если количество нечетное
    if(i <= n){
        result += 1.0 / static_cast<double>(i+0);
    }

    // Освобождаем память
    _mm_free(a);
    _mm_free(b);

    return result;
}

int main(int argc, char* argv[]){
    // выставляем точность вывода вручную
    std::cout.precision(20);

    //Сопроводительная информация, из комментария в начале файла
    std::cout << "Номер студента: 2\n";
    std::cout << "Задание: сумма гармонического ряда от 1 до n: 1/1 + 1/2 + ... + 1/n\n";
    std::cout << "Использовать: 3) sse-инструкции\n";

    // Параметры программы
    size_t N = 10000000;

    // Можно указать размер массива
    if(argc >= 2){
        N = std::stoul(argv[1]);
    }

    // Тестируем последовательное вычисление
    auto start_sequential = std::chrono::high_resolution_clock::now();
    double result_sequential = sequential_calculation(N);
    auto end_sequential = std::chrono::high_resolution_clock::now();
    auto duration_sequential = std::chrono::duration_cast<std::chrono::milliseconds>(end_sequential - start_sequential);
    
    // Тестируем SIMD-SSE вычисление
    auto start_sse = std::chrono::high_resolution_clock::now();
    double result_simd = simd_calculation(N);
    auto end_sse = std::chrono::high_resolution_clock::now();
    auto duration_sse = std::chrono::duration_cast<std::chrono::milliseconds>(end_sse - start_sse);

    // Выводим результаты
    std::cout << "Number of elements: " << N << "\n";
    std::cout << "Sequential result: " << result_sequential << "\n";
    std::cout << "SIMD-SSE result: " << result_simd << "\n";
    std::cout << "Absolute difference: " << std::abs(result_sequential - result_simd) << "\n";
    std::cout << "Sequential execution time: " << duration_sequential.count() << " ms\n";
    std::cout << "SIMD-SSE execution time: " << duration_sse.count() << " ms\n";
    std::cout << "Speedup with SSE: " << (double)duration_sequential.count() / duration_sse.count() << "x\n";

    return 0;
}