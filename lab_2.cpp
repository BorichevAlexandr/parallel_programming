#include <iostream>
#include <random>
#include <omp.h>

int main()
{
    long long numFamilies = 0;

    std::cout << "Введите количество для генерации семей: ";

    if (!(std::cin >> numFamilies))
    {
        std::cerr << "Ввод не является числом!!!" << std::endl;
        return 1;
    }

    if (numFamilies <= 0)
    {
        std::cerr << "Количество семей должно быть положительным." << std::endl;
        return 1;
    }

    const unsigned int seed = 13;
    long long createdFamily = 0;

    double startTime = omp_get_wtime();

    #pragma omp parallel
    {
        int threadId = omp_get_thread_num();
        std::mt19937 generator(seed + threadId);
        std::uniform_int_distribution<int> genderDist(0, 1);
        std::uniform_int_distribution<int> dayDist(0, 6);

        #pragma omp for schedule(static) reduction(+: createdFamily)
        for (long long i = 0; i < numFamilies; ++i)
        {
            int gender1 = genderDist(generator);
            int day1 = dayDist(generator);
            int gender2 = genderDist(generator);
            int day2 = dayDist(generator);
            createdFamily++;
        }
    }

    double endTime = omp_get_wtime();
    double elapsedTime = endTime - startTime;

    std::cout << "Создано семей: " << createdFamily << std::endl;
    std::cout << "Время выполнения: " << elapsedTime << " секунд." << std::endl;

    return 0;
}