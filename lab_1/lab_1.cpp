#include <iostream>
#include <vector>
#include <cstddef>
#include <random>
#include <cstdint>
#include <omp.h>
#include <fstream> 
#include <cstdlib>
#include <string>

bool loadPatternToCenter(std::vector<uint8_t>& field, int X, const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Cannot open pattern file: " << filename << std::endl;
        return false;
    }

    int patternSize = 0;
    file >> patternSize;
    if (patternSize <= 0 || patternSize > X)
    {
        std::cerr << "Invalid pattern size" << std::endl;
        return false;
    }

    const int width = X + 2;
    int offset = (X - patternSize) / 2 + 1;

    for (int i = 0; i < patternSize; ++i)
    {
        for (int j = 0; j < patternSize; ++j)
        {
            int realI = offset + i;
            int realJ = offset + j;
            std::size_t index = static_cast<std::size_t>(realI) * width + realJ;
            field[index] = 0;
        }
    }

    
    int i, j;
    while (file >> i >> j)
    {
        if (i >= 0 && i < patternSize && j >= 0 && j < patternSize)
        {
            int realI = offset + i;
            int realJ = offset + j;
            std::size_t index = static_cast<std::size_t>(realI) * width + realJ;
            field[index] = 1;
        }
    }

    file.close();
    return true;
}

// центральный кусок newField обратно в файл
bool saveCenterPattern(const std::vector<uint8_t>& field, int X, int patternSize, const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Cannot open result pattern file: " << filename << std::endl;
        return false;
    }

    file << patternSize << "\n";

    int offset = (X - patternSize) / 2 + 1;
    const int width = X + 2;

    for (int i = 0; i < patternSize; ++i)
    {
        for (int j = 0; j < patternSize; ++j)
        {
            int realI = offset + i;
            int realJ = offset + j;
            std::size_t index = static_cast<std::size_t>(realI) * width + realJ;

            if (field[index] == 1)
            {
                file << i << " " << j << "\n";
            }
        }
    }

    file.close();
    return true;
}

/////////////////////////////////////////////////////////////
inline int countAliveNeighbors(const std::vector<uint8_t>& field, 
                               int i, 
                               int j, 
                               int width)
{
    int aliveNeighbors = 0;
    
    for (int di = -1; di <= 1; ++di)
    {
        for (int dj = -1; dj <= 1; ++dj)
        {
            if (di == 0 && dj == 0)
            {
                continue;
            }
            
            std::size_t neighborIndex = 
                static_cast<std::size_t>(i + di) * static_cast<std::size_t>(width) + 
                static_cast<std::size_t>(j + dj);
            
            aliveNeighbors += field[neighborIndex];
        }
    }
    
    return aliveNeighbors;
}

inline uint8_t applyGameOfLifeRules(uint8_t currentState, int aliveNeighbors)
{
    uint8_t nextState = 0;
    
    if (currentState == 1)
    {
        if (aliveNeighbors == 2 || aliveNeighbors == 3)
        {
            nextState = 1;
        }
    }
    else
    {
        if (aliveNeighbors == 3)
        {
            nextState = 1;
        }
    }
    
    return nextState;
}

void stepGameOfLife(const std::vector<uint8_t>& oldField, 
                    std::vector<uint8_t>& newField, 
                    int X)
{
    const int width = X + 2;
    
    #pragma omp parallel for schedule(static)
    for (int i = 1; i <= X; ++i)
    {
        for (int j = 1; j <= X; ++j)
        {
            std::size_t currentIndex = 
                static_cast<std::size_t>(i) * static_cast<std::size_t>(width) + 
                static_cast<std::size_t>(j);
    
            int aliveNeighbors = countAliveNeighbors(oldField, i, j, width);
            uint8_t currentState = oldField[currentIndex];
            uint8_t nextState = applyGameOfLifeRules(currentState, aliveNeighbors);
            newField[currentIndex] = nextState;
        }
    }
}

void generateRandomField(std::vector<uint8_t>& field, int X, unsigned int seed)
{
    const int width = X + 2;

    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> distribution(0, 1);

    for (int i = 1; i <= X; ++i)
    {
        for (int j = 1; j <= X; ++j)
        {
      
            std::size_t index = static_cast<std::size_t>(i) * static_cast<std::size_t>(width) + 
            static_cast<std::size_t>(j);

            field[index] = static_cast<uint8_t>(distribution(generator));
        }
    }
}

int main()
{
    int X = 0;
    double startTime = 0.0;
    double endTime = 0.0;
    double avgTime = 0.0;

    std::cout << "Enter field size X: ";

    if (!(std::cin >> X))
    {
        std::cerr << "Input error: expected integer X." << std::endl;
        return 1;
    }

    if (X <= 0)
    {
        std::cerr << "Field size X must be positive." << std::endl;
        return 1;
    }

    const int width = X + 2;
    const int height = X + 2;

    std::size_t totalCells =
        static_cast<std::size_t>(width) *
        static_cast<std::size_t>(height);

    std::vector<uint8_t> oldField(totalCells, 0);
    std::vector<uint8_t> newField(totalCells, 0);

    const unsigned int seed = 13;

    generateRandomField(oldField, X, seed);
    ////////////////////////////////////////////////////////
    int editorResult = system("py visualizer.py edit");
    if (editorResult != 0)
    {
        std::cerr << "Failed to open pattern editor!" << std::endl;
        return 1;
    }


    if (!loadPatternToCenter(oldField, X, "pattern.txt"))
    {
        std::cerr << "Failed to load pattern!" << std::endl;
        return 1;
    }
    
    const int threadsCounts[] = {1,2,4,6,8};
    const int numTests = sizeof(threadsCounts) / sizeof(threadsCounts[0]);
    const int RUNS = 10;

    std::ofstream csv("results.csv");
    if (!csv.is_open())
    {
        std::cerr << "Cannot open results.csv for writing!" << std::endl;
        return 1;
    }
    csv << "threads,avg_time_sec,field_size\n";

    std::cout << "Field size: " << X << " x " << X << std::endl;
    std::cout << "Threads | Time (seconds)" << std::endl;

    for (int t = 0; t < numTests; ++t)
    {
        int numThreads = threadsCounts[t];

        omp_set_num_threads(numThreads);
        double totalTime = 0.0;
        for(int run =0; run <RUNS; ++run)
        {
            newField = oldField;

            startTime = omp_get_wtime();
            stepGameOfLife(oldField, newField, X);
            endTime = omp_get_wtime();

            totalTime += (endTime - startTime);
        }  
        avgTime = totalTime/ RUNS;
        
        std::cout << "   " << numThreads 
                  << "    | " << avgTime << std::endl;

        csv << numThreads << "," << avgTime << "," << X << "\n";
    }

    csv.close();
    
    saveCenterPattern(newField, X, 15, "result_pattern.txt");

    system("py visualizer.py view");
    
    int result = system("py -3 plot_results.py");

    if (result != 0)
    {
        std::cout << "Warning: failed to run plot_results.py" << std::endl;
        std::cout << "You can run it manually later." << std::endl;
    }

    return 0;
}



