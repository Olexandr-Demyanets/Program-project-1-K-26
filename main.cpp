// g++ (Rev8, Built by MSYS2 project) 15.2.0
// Компиляция через CMake (CMakeLists.txt, стандарт C++23)

#include <iostream>
#include <random>
#include <compare>
#include <vector>
#include <numeric>
#include <algorithm>
#include <limits>
#include <string>

using namespace std;

struct Cell
{
    int x = 0;
    int y = 0;

    auto operator<=>(const Cell &other) const = default;
};

std::ostream &operator<<(std::ostream &out, const Cell &c)
{
    out << "(" << c.x << ", " << c.y << ")";
    return out;
}

class RandomCellGenerator
{
    size_t n;
    std::mt19937 engine;
    std::uniform_int_distribution<int> dist;

    static size_t validate(size_t n_)
    {
        if (n_ == 0)
            throw std::invalid_argument("RandomCellGenerator: n must be positive");
        return n_;
    }

public:
    explicit RandomCellGenerator(size_t n_);
    Cell operator()();
};

RandomCellGenerator::RandomCellGenerator(size_t n_)
    : n(validate(n_)), dist(0, static_cast<int>(n_) - 1)
{
    std::random_device r;
    std::seed_seq seeds{r(), r(), r(), r(), r(), r()};
    engine = std::mt19937(seeds);
}

Cell RandomCellGenerator::operator()()
{
    return Cell{dist(engine), dist(engine)};
}

size_t computeN2(size_t n)
{
    const size_t MAX_N = 100000;
    const size_t MAX_N2 = 100'000'000;

    if (n == 0)
        throw std::invalid_argument("n has to be positive");
    if (n > MAX_N)
        throw std::invalid_argument("n is too large (max " + std::to_string(MAX_N) + ")");

    size_t n2 = n * n;
    if (n2 > MAX_N2)
        throw std::invalid_argument("n^2 exceeds memory limit (" + std::to_string(MAX_N2) + " cells)");

    return n2;
}

std::vector<int> generateMultiplicities(RandomCellGenerator &gen, size_t n, size_t m)
{
    size_t n2 = computeN2(n);
    std::vector<int> counts(n2, 0);

    for (size_t i = 0; i < m; ++i)
    {
        Cell c = gen();
        size_t index = static_cast<size_t>(c.x) * n + static_cast<size_t>(c.y);
        ++counts[index];
    }
    return counts;
}

double averageMultiplicity(const std::vector<int> &values)
{
    if (values.empty())
        throw std::invalid_argument("averageMultiplicity: empty board");

    long long sum = std::accumulate(values.begin(), values.end(), 0LL);
    return static_cast<double>(sum) / values.size();
}

double medianMultiplicity(std::vector<int> values)
{
    if (values.empty())
        throw std::invalid_argument("medianMultiplicity: empty board");

    size_t sz = values.size();
    size_t mid = sz / 2;

    std::nth_element(values.begin(), values.begin() + mid, values.end());

    if (sz % 2 == 1)
    {
        return values[mid];
    }
    else
    {
        auto max_it = std::max_element(values.begin(), values.begin() + mid);
        return (*max_it + values[mid]) / 2.0;
    }
}

size_t readPositiveNumber(const std::string &prompt, size_t maxValue = std::numeric_limits<size_t>::max())
{
    size_t value;
    while (true)
    {
        std::cout << prompt;
        if (std::cin >> value && value > 0 && value <= maxValue)
        {
            return value;
        }

        std::cout << "Invalid input. Please enter a positive integer (<= " << maxValue << ")." << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

void Run(size_t n, size_t m)
{
        std::cout << "n = " << n << ", m = " << m << std::endl;

    RandomCellGenerator gen(n);
    std::vector<int> full = generateMultiplicities(gen, n, m);

    std::cout << "Cells that appeared (multiplicity > 0):" << std::endl;
    for (size_t i = 0; i < full.size(); ++i)
    {
        if (full[i] > 0)
        {
            Cell c{static_cast<int>(i / n), static_cast<int>(i % n)};
            std::cout << c << ": " << full[i] << std::endl;
        }
    }

    std::cout << "Average multiplicity (over all n^2 cells): " << averageMultiplicity(full) << std::endl;
    std::cout << "Median multiplicity (over all n^2 cells): " << medianMultiplicity(full) << std::endl;

    std::cout << std::endl;
}

void RunSeries(size_t n)
{
    size_t n2 = computeN2(n);
    std::cout << "Series for n = " << n << " (n^2 = " << n2 << ")" << std::endl;
    std::cout << "m\tm/n^2\tavg\tmedian" << std::endl;

    std::vector<size_t> ms = {
        std::max<size_t>(1, n2 / 4), std::max<size_t>(1, n2 / 2),
        n2, 2 * n2, 5 * n2, 10 * n2
    };

    RandomCellGenerator gen(n);

    for (size_t m : ms)
    {
        std::vector<int> full = generateMultiplicities(gen, n, m);

        double ratio = static_cast<double>(m) / n2;
        double avg = averageMultiplicity(full);
        double med = medianMultiplicity(full);

        std::cout << m << "\t" << ratio << "\t" << avg << "\t" << med << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    try
    {
        std::cout << "Program project 1, task 9. Made by Demyanets Olexandr K-26" << std::endl << std::endl;

        const size_t MAX_N_FOR_INPUT = 1000;
        const size_t MAX_M_FOR_INPUT = 100'000'000;

        size_t n = readPositiveNumber("Enter board size n (n > 0): ", MAX_N_FOR_INPUT);
        size_t m = readPositiveNumber("Enter number of samples m (m > 0): ", MAX_M_FOR_INPUT);

        Run(n, m);

        int answer = 0;
        while (true)
        {
            std::cout << "Run full series (m/n^2 = 0.25 .. 10) for n = " << n << "? (1-yes, 0-no): ";
            if (std::cin >> answer && (answer == 0 || answer == 1))
            {
                break;
            }
            std::cout << "Invalid option. Please enter 1 or 0." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        if (answer == 1)
            RunSeries(n);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}