/**
 * Автор: Карташев Кирилл
 * Номер в таблиц студентов курса: 2
 * 
 * Номер задания: (2 % 4) + 1 = 1 (ошибся, должно быть 3)
 * Задание текстом: Найти максимум в массиве (ошибся, должна быть сумма массива, 
 * но так-как это тоже линейный алгоритм, глобально ничего не должно поменяться)
 * 
 * Номер метода параллелизации: ((2 / 4) % 5) + 1
 * Описание метода параллелизации: реализовать при помощи априорного разделения, использовать packaged_task
 * 
 * Номер варианта данных в массиве: (2 / 20) + 1 = 1
 * Тип данных в массиве: int (32-bit)
 */

#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <algorithm>
#include <random>
#include <chrono>
#include <climits>
#include <execution>

// Функция для поиска максимума в части массива
int find_max_in_range(const std::vector<int>& data, size_t start, size_t end){
    int max_val = INT_MIN;
    for(size_t i = start; i < end; ++i){
        if(data[i] > max_val){
            max_val = data[i];
        }
    }
    return max_val;
}

// Параллельный поиск максимума
int parallel_max(const std::vector<int>& data, size_t num_threads){
    if(data.empty()){
        return INT_MIN;
    }
    
    if(num_threads == 0 || num_threads > data.size()){
        num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 1;
        num_threads = std::min(num_threads, data.size());
    }
    
    size_t chunk_size = data.size() / num_threads;
    size_t remainder = data.size() % num_threads;
    
    std::vector<std::packaged_task<int()>> tasks;
    std::vector<std::future<int>> futures;
    std::vector<std::thread> threads;
    
    // Создаем задачи для каждого потока
    size_t start = 0;
    for(size_t i = 0; i < num_threads; ++i){
        size_t end = start + chunk_size + (i < remainder ? 1 : 0);
        
        // Создаем packaged_task
        std::packaged_task<int()> task([&data, start, end](){
            return find_max_in_range(data, start, end);
        });
        
        // Получаем future от задачи
        futures.push_back(task.get_future());
        
        // Кладём задачу в таски
        tasks.push_back(std::move(task));
        
        start = end;
    }

    // Запускаем потоки
    for (auto& task : tasks){
        threads.emplace_back(std::move(task));
    }
    
    // Собираем результаты
    int global_max = INT_MIN;
    for(auto& future : futures){
        int local_max = future.get();
        if(local_max > global_max){
            global_max = local_max;
        }
    }
    
    // Ждем завершения всех потоков
    for(auto& thread : threads){
        thread.join();
    }
    
    return global_max;
}

// Проверка правильности работы (STL поиск для сравнения)
int stl_max(const std::vector<int>& data){
    return *std::max_element(std::execution::par, data.begin(), data.end());
}

int main(int argc, char* argv[]){
    //Сопроводительная информация, из комментария в начале файла
    std::cout << "Номер студента: 2\n";
    std::cout << "Решаемая задача: Поиск максимума в массиве\n";
    std::cout << "Метод параллелизации: при помощи априорного разделения, использовать packaged_task\n";
    std::cout << "Тип данных в массиве: int (32-bit)\n";

    // Параметры программы
    size_t array_size = 1'000'000;
    size_t num_threads = std::thread::hardware_concurrency();
    
    // Можно указать размер массива и число потоков через аргументы командной строки
    if(argc >= 2){
        array_size = std::stoul(argv[1]);
    }
    if(argc >= 3){
        num_threads = std::stoul(argv[2]);
    }
    
    if(num_threads == 0){
        num_threads = 4;
    }
    
    std::cout << "Array size: " << array_size << std::endl;
    std::cout << "Number of threads: " << num_threads << std::endl;
    
    // Генерация случайных чисел
    std::vector<int> data(array_size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(1, 1'000'000'000);
    
    for(size_t i = 0; i < array_size; ++i){
        data[i] = dis(gen);
    }
    
    // Параллельный поиск
    auto start_parallel = std::chrono::high_resolution_clock::now();
    int parallel_result = parallel_max(data, num_threads);
    auto end_parallel = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> parallel_duration = end_parallel - start_parallel;
    
    // Последовательный поиск (для проверки и сравнения производительности)
    auto start_stl = std::chrono::high_resolution_clock::now();
    int stl_result = stl_max(data);
    auto end_stl = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> stl_duration = end_stl - start_stl;
    
    // Вывод результатов
    std::cout << "\nResult:" << std::endl;
    std::cout << "Parallel maximum: " << parallel_result << std::endl;
    std::cout << "STL maximum: " << stl_result << std::endl;
    std::cout << "\nProgram execution time:" << std::endl;
    std::cout << "Parallel: " << parallel_duration.count() << " seconds" << std::endl;
    std::cout << "STL: " << stl_duration.count() << " seconds" << std::endl;
    std::cout << "Speedup: " << stl_duration.count() / parallel_duration.count() << "x" << std::endl;
    
    // Проверка корректности
    if(parallel_result == stl_result){
        std::cout << "\nResults match!" << std::endl;
    } 
    else{
        std::cout << "\nError: results differ!" << std::endl;
        return 1;
    }
    
    return 0;
}