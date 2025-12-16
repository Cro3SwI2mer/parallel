/**
 * Автор: Карташев Кирилл
 * Номер в таблиц студентов курса: 2
 * 
 * Номер задания: (2 % 4) + 1 = 3
 * Задание текстом: Найти сумму массива
 * 
 * Номер метода параллелельной реализации: ((2 / 4) % 5) + 1
 * Описание метода параллельной реализации: использование task для динамического распределения работы
 * 
 * Номер варианта данных в массиве: (2 / 20) + 1 = 1
 * Тип данных в массиве: int (32-bit)
 */

#include <iostream>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <algorithm>

// Проверка, что собрано с поддержкой OpenMP
#ifdef _OPENMP
#include <omp.h>
#define HAS_OPENMP 1
#else
#define HAS_OPENMP 0
#endif

// Функция для генерации случайного массива
std::vector<int> generate_random_array(size_t size){
    std::vector<int> arr(size);
    #pragma omp parallel for if (size > 10000) // threshold для оптимизации генерации
    for(size_t i = 0; i < size; i++){
        arr[i] = rand() % 100; // берём от 0 до 100, для упрощения, чтобы минимизировать возможность переполнения
    }
    return arr;
}

// Последовательное суммирование
long long sequential_sum(const std::vector<int>& arr){
    long long sum = 0;
    for(int val : arr){
        sum += val;
    }
    return sum;
}

// Параллельная версия с использованием OpenMP tasks
long long parallel_sum_tasks(const std::vector<int>& arr, int num_threads){
    long long total_sum = 0;
    
#if HAS_OPENMP
    omp_set_num_threads(num_threads);

    #pragma omp parallel
    {
        #pragma omp single
        {
            size_t chunk_size = arr.size() / (num_threads * 4); // делим на чанки
            if(chunk_size < 1000){
                chunk_size = 1000;
            }
            size_t num_chunks = (arr.size() + chunk_size - 1) / chunk_size;
            
            for(size_t i = 0; i < num_chunks; i++){
                size_t start = i * chunk_size;
                size_t end = std::min(start + chunk_size, arr.size());
                
                #pragma omp task shared(total_sum)
                {
                    long long local_sum = 0;
                    for(size_t j = start; j < end; j++){
                        local_sum += arr[j];
                    }
                    
                    #pragma omp atomic
                    total_sum += local_sum;
                }
            }
            
            #pragma omp taskwait
        }
    }
#endif
    
    return total_sum;
}

int main(int argc, char* argv[]){
    // Параметры по умолчанию
    size_t array_size = 10000000;
    bool use_parallel = true;
    int num_threads = 2;
    
    // Обработка аргументов командной строки
    for(int i = 1; i < argc; i++){
        std::string arg = argv[i];
        if((arg == "--sequential") || (arg == "-s")){
            use_parallel = false;
        }
        else if((arg == "--parallel") || (arg == "-p")){
            use_parallel = true;
        }
        else if((arg == "--threads") || (arg == "-t")){
            if ((i + 1) < argc) {
                num_threads = std::atoi(argv[++i]);
                if(num_threads < 1){
                    std::cerr << "Error: Number of threads must be positive\n";
                    return 1;
                }
            }
        }
        else if(arg == "--size" || arg == "-n"){
            if((i + 1) < argc){
                array_size = std::atoll(argv[++i]);
                if (array_size < 1) {
                    std::cerr << "Error: Array size must be positive\n";
                    return 1;
                }
            }
        }
        else if (arg == "--help" || arg == "-h"){
            std::cout << "Array Sum Calculator\n";
            std::cout << "=======================================\n";
            std::cout << "Usage: " << argv[0] << " [options]\n\n";
            std::cout << "Options:\n";
            std::cout << "  -s, --sequential    Run sequential version\n";
            std::cout << "  -p, --parallel      Run parallel version (default)\n";
            std::cout << "  -t, --threads N     Set number of threads (default: 2)\n";
            std::cout << "  -n, --size N        Set array size (default: 10'000'000)\n";
            std::cout << "  -h, --help          Show this help\n\n";
            std::cout << "Examples:\n";
            std::cout << "  " << argv[0] << " --sequential --size 5000000\n";
            std::cout << "  " << argv[0] << " --parallel --threads 8 --size 10000000\n";
            return 0;
        }
        else{
            std::cerr << "Error: Unknown option '" << arg << "'\n";
            std::cerr << "Use '" << argv[0] << " --help' for usage information\n";
            return 1;
        }
    }
    
    // Печать текущей конфигурации программы
    std::cout << "Configuration:\n";
    std::cout << "  Array size: " << array_size << " elements\n";
    if(use_parallel){
        std::cout << "  Threads: " << num_threads;
#if !HAS_OPENMP
        std::cout << " (OpenMP not available, will use sequential)";
#endif
        std::cout << "\n";
    }
    else{
        std::cout << "  Mode: sequential\n";
    }
    
    // Генерация массива
    std::cout << "\nGenerating array...\n";
    std::srand(42);
    auto gen_start = std::chrono::high_resolution_clock::now();
    std::vector<int> arr = generate_random_array(array_size);
    auto gen_end = std::chrono::high_resolution_clock::now();
    auto gen_duration = std::chrono::duration_cast<std::chrono::milliseconds>(gen_end - gen_start);
    
    std::cout << "  Generation time: " << gen_duration.count() << " ms\n";
    
    // Тайминг
    auto start_time = std::chrono::high_resolution_clock::now();
    
    long long sum;
    if(use_parallel){
        sum = parallel_sum_tasks(arr, num_threads);
    }
    else{
        sum = sequential_sum(arr);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    // Верификация результата (сравнение с последовательной версией для валидации)
    auto verify_start = std::chrono::high_resolution_clock::now();
    long long check_sum = sequential_sum(arr);
    auto verify_end = std::chrono::high_resolution_clock::now();
    auto verify_duration = std::chrono::duration_cast<std::chrono::microseconds>(verify_end - verify_start);
    
    // Печать результатов
    std::cout << "\nResults:\n";
    std::cout << "  Sum of elements: " << sum << "\n";
    std::cout << "  Сomputation time: " << duration << " (" << duration.count() / 1000.0 << " ms)\n";
    
    if(use_parallel){
        double speedup = (double)verify_duration.count() / duration.count();
        std::cout << "  Sequential computation time: " << verify_duration << " (" << verify_duration.count() / 1000.0 << " ms)\n";
        std::cout << "  Speedup vs sequential: " << (double)speedup << "x\n";
    }
    
    // Валидация
    if(sum == check_sum){
        std::cout << "  Status: Result verified\n";
    }
    else{
        std::cout << "  Status: Error: result doesn't match!\n";
        std::cout << "    Expected: " << check_sum << "\n";
        std::cout << "    Got: " << sum << "\n";
        return 1;
    }
    
    return 0;
}