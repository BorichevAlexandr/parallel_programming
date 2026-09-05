#include <iostream>
#include <vector>
#include <cstddef>
#include <random>
#include <cstdint>
#include <omp.h>

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

void printField(const std::vector<uint8_t>& field, int X)
{
    const int width = X + 2;

    for (int i = 1; i <= X; ++i)
    {
        for (int j = 1; j <= X; ++j)
        {
            std::size_t index = 
                static_cast<std::size_t>(i) * static_cast<std::size_t>(width) + 
                static_cast<std::size_t>(j);

            std::cout << (field[index] == 1 ? "X " : ". ");
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
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
    double elapsedTime = 0.0;

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

    std::cout << "Initial state:" << std::endl;


    startTime = omp_get_wtime();
    stepGameOfLife(oldField, newField, X);
    endTime = omp_get_wtime();

    elapsedTime = endTime - startTime;
    std::cout << "State after one step:" << std::endl;
    // printField(newField, X);
    std::cout << "Step execution time: " << elapsedTime << " seconds" << std::endl;

    return 0;
}