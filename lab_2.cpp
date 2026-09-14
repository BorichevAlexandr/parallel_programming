#include <iostream>
#include <ostream>
#include <cmath>
#include <random>
#include <omp.h>
#include <fstream>
#include <cstdlib>

double measurePerformance(long long numFamilies, int numThreads)
{
    omp_set_num_threads(numThreads);

    long long dummy = 0;

    double startTime = omp_get_wtime();

    #pragma omp parallel
    {
        int threadId = omp_get_thread_num();

        const unsigned int seed = 13;

        std::mt19937 generator(seed + threadId);

        std::uniform_int_distribution<int> genderDist(0, 1);
        std::uniform_int_distribution<int> dayDist(0, 6);

        #pragma omp for schedule(static) reduction(+: dummy)
        for (long long i = 0; i < numFamilies; ++i)
        {
            int gender1 = genderDist(generator);
            int day1 = dayDist(generator);

            int gender2 = genderDist(generator);
            int day2 = dayDist(generator);

            dummy += gender1 + day1 + gender2 + day2;
        }
    }

    double endTime = omp_get_wtime();

    return endTime - startTime;
}




int main()
{
    long long numFamilies = 0;

    std::cout << "Input count families: ";

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
    const int countThreads[] = {1, 2, 4, 6, 8};
const int RUNS = 10;

std::ofstream resultsFile("results.csv");

if (!resultsFile.is_open())
{
    std::cerr << "Error: cannot create results.csv!" << std::endl;
    return 1;
}

resultsFile << "threads,avg_time_sec\n";

std::cout << "\n========================================\n";
std::cout << "        PERFORMANCE TEST\n";
std::cout << "========================================\n\n";

for (int numThreads : countThreads)
{
    double totalTime = 0.0;

    for (int run = 0; run < RUNS; ++run)
    {
        totalTime += measurePerformance(numFamilies, numThreads);
    }

    double averageTime = totalTime / RUNS;

    std::cout << "Threads: " << numThreads
              << " | Average time: " << averageTime
              << " seconds\n";

    resultsFile << numThreads << ","
                << averageTime << "\n";
}

resultsFile.close();

    const unsigned int seed = 13;
    long long createdFamily = 0;

    double startTime = omp_get_wtime();

    long long fitB = 0;
    long long twoGirls = 0;
    long long twoGirlsC = 0;
    long long fitC = 0;    
    long long fitA = 0;

    int printC = 0;

    #pragma omp parallel
    {
        int threadId = omp_get_thread_num();
        std::mt19937 generator(seed + threadId);
        std::uniform_int_distribution<int> genderDist(0, 1);
        std::uniform_int_distribution<int> dayDist(0, 6);

        

        #pragma omp for schedule(static) reduction(+: createdFamily,fitA,twoGirls,fitB,fitC,twoGirlsC)
        for (long long i = 0; i < numFamilies; ++i)
        {
            /* gender: 0 - девочка, 1 - мальчик
                day: 0 - Пн, 1- Вт ....
                gender1 - будем считать старший ребёнок
                gender2 - младший 
            */

            int gender1 = genderDist(generator);
            int day1 = dayDist(generator);
            int gender2 = genderDist(generator);
            int day2 = dayDist(generator);
            createdFamily++;
            ///////////////////////////////////////////////
            if(gender1 == 0 && gender2 == 0){
                    twoGirls++;
                }

            if(gender1 == 0 || gender2 == 0){
                fitA++;
            }

            if(gender1 == 0){
                fitB++;
            }

            if((gender1 == 0 && day1 == 1) || (gender2 == 0 && day2 == 1)){
                fitC++;
                if(gender1 == 0 && gender2 == 0){
                    twoGirlsC++;
                }
                #pragma omp critical
                {
                    if (printC < 10)
                    {
                        printC++;

                        const char* genderName1 = (gender1 == 0) ? "Girl" : "Boy";
                        const char* genderName2 = (gender2 == 0) ? "Girl" : "Boy";

                        const char* dayName1[] = {
                            "Monday", "Tuesday", "Wednesday",
                            "Thursday", "Friday", "Saturday", "Sunday"
                        };

                        const char* dayName2[] = {
                            "Monday", "Tuesday", "Wednesday",
                            "Thursday", "Friday", "Saturday", "Sunday"
                        };

                        std::cout << "\nFamily #" << printC << ":\n";
                        std::cout << "  Older child:   " << genderName1
                                << ", " << dayName1[day1] << '\n';

                        std::cout << "  Younger child: " << genderName2
                                << ", " << dayName2[day2] << '\n';
                    }
                }
                

            }

        }
    }

    double endTime = omp_get_wtime();
    double elapsedTime = endTime - startTime;

    double probabilityA = static_cast<double>(twoGirls) / fitA;
    double probabilityB = static_cast<double>(twoGirls) / fitB;
    double probabilityC = static_cast<double>(twoGirlsC) / fitC;

    double theoreticalA = 1.0 / 3.0;
    double theoreticalB = 1.0 / 2.0;
    double theoreticalC = 13.0 / 27.0;

    double errorA = std::abs(probabilityA - theoreticalA);
    double errorB = std::abs(probabilityB - theoreticalB);
    double errorC = std::abs(probabilityC - theoreticalC);

    std::cout << "\n============================================================\n";
    std::cout << "                 TWO CHILDREN PARADOX\n";
    std::cout << "                 Monte Carlo Simulation\n";
    std::cout << "============================================================\n\n";

    std::cout << "Number of simulated families: " << createdFamily << '\n';
    std::cout << "Execution time:               " << elapsedTime << " seconds\n";

    std::cout << "\n------------------------------------------------------------\n";
    std::cout << "EXPERIMENT A\n";
    std::cout << "Condition: at least one child is a girl\n";
    std::cout << "------------------------------------------------------------\n";

    std::cout << "Families satisfying condition: " << fitA << '\n';
    std::cout << "Families with two girls:        " << twoGirls << '\n';
    std::cout << "Experimental probability:       " << probabilityA << '\n';
    std::cout << "Theoretical probability:        " << theoreticalA << '\n';
    std::cout << "Absolute error:                 " << errorA << '\n';

    std::cout << "\n------------------------------------------------------------\n";
    std::cout << "EXPERIMENT B\n";
    std::cout << "Condition: the older child is a girl\n";
    std::cout << "------------------------------------------------------------\n";

    std::cout << "Families satisfying condition: " << fitB << '\n';
    std::cout << "Families with two girls:        " << twoGirls << '\n';
    std::cout << "Experimental probability:       " << probabilityB << '\n';
    std::cout << "Theoretical probability:        " << theoreticalB << '\n';
    std::cout << "Absolute error:                 " << errorB << '\n';

    std::cout << "\n------------------------------------------------------------\n";
    std::cout << "EXPERIMENT C\n";
    std::cout << "Condition: at least one girl was born on Tuesday\n";
    std::cout << "------------------------------------------------------------\n";

    std::cout << "Families satisfying condition: " << fitC << '\n';
    std::cout << "Families with two girls:        " << twoGirlsC << '\n';
    std::cout << "Experimental probability:       " << probabilityC << '\n';
    std::cout << "Theoretical probability:        " << theoreticalC << '\n';
    std::cout << "Absolute error:                 " << errorC << '\n';

    std::cout << "\n============================================================\n";

    // Запускаем программу визуализации
    int result = std::system(".\\visualizer.exe");

    if (result != 0)
    {
        std::cerr << "Error: visualizer.exe could not be started!"
                << std::endl;
    }

    return 0;
}