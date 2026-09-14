#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <iomanip>

int main()
{
    const std::string csvPath = "results.csv";
    const std::string scriptPath = "plot.gp";
    const std::string outputImage = "performance_graph.png";

    std::ifstream input(csvPath);

    if (!input.is_open())
    {
        std::cerr << "Error: file '" << csvPath << "' not found!" << std::endl;
        return 1;
    }

    std::vector<int> threads;
    std::vector<double> times;

    std::string line;

    // Пропускаем заголовок CSV
    std::getline(input, line);

    while (std::getline(input, line))
    {
        std::stringstream ss(line);

        std::string threadStr;
        std::string timeStr;

        if (std::getline(ss, threadStr, ',') &&
            std::getline(ss, timeStr, ','))
        {
            threads.push_back(std::stoi(threadStr));
            times.push_back(std::stod(timeStr));
        }
    }

    input.close();

    if (threads.empty())
    {
        std::cerr << "Error: CSV file is empty!" << std::endl;
        return 1;
    }

    // Создаём файл с командами для GNUplot
    std::ofstream script(scriptPath);

    if (!script.is_open())
    {
        std::cerr << "Error: cannot create GNUplot script!" << std::endl;
        return 1;
    }

    script << "set terminal pngcairo size 1400,650 enhanced font 'Arial,11'\n";
    script << "set output '" << outputImage << "'\n";

    script << "set datafile separator ','\n";

    // Общие настройки
    script << "set grid\n";
    script << "set xtics (";

    for (std::size_t i = 0; i < threads.size(); ++i)
    {
        script << "'" << threads[i] << "' " << threads[i];

        if (i + 1 < threads.size())
            script << ", ";
    }

    script << ")\n";

    // Вычисляем speedup относительно первого результата
    double baseTime = times[0];

    std::vector<double> speedup;

    for (double time : times)
    {
        speedup.push_back(baseTime / time);
    }

    // Записываем данные для speedup во временный файл
    std::ofstream speedupFile("speedup.csv");

    speedupFile << "threads,speedup\n";

    for (std::size_t i = 0; i < threads.size(); ++i)
    {
        speedupFile << threads[i] << ","
                    << std::fixed << std::setprecision(6)
                    << speedup[i] << "\n";
    }

    speedupFile.close();

    // Два графика рядом
    script << "set multiplot layout 1,2\n";

    // ============================================================
    // ЛЕВЫЙ ГРАФИК — ВРЕМЯ
    // ============================================================

    script << "set xlabel 'Number of threads'\n";
    script << "set ylabel 'Average execution time, sec'\n";
    script << "set title 'Execution Time'\n";

    script << "plot '" << csvPath
           << "' using 1:2 with linespoints "
              "lw 3 pt 7 ps 1.5 "
              "title 'Execution time'\n";

    // ============================================================
    // ПРАВЫЙ ГРАФИК — SPEEDUP
    // ============================================================

    script << "set xlabel 'Number of threads'\n";
    script << "set ylabel 'Speedup'\n";
    script << "set title 'Speedup relative to 1 thread'\n";

    script << "set yrange [0:*]\n";

    script << "plot 'speedup.csv' using 1:2 with boxes "
              "fill solid 0.75 "
              "title 'Speedup', "
              "1 with lines dashtype 2 "
              "title '1x'\n";

    script << "unset multiplot\n";

    script.close();

    // Запускаем GNUplot
    int result = std::system("gnuplot plot.gp");

    if (result != 0)
    {
        std::cerr << "\nError: GNUplot could not be started!\n";
        std::cerr << "Make sure GNUplot is installed and available in PATH.\n";
        return 1;
    }

    std::cout << "\nGraph successfully created: "
              << outputImage << std::endl;

    return 0;
}