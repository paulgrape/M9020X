#include <omp.h>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>

// Дана последовательность натуральных чисел {a0…an–1}. Создать
// OpenMP-приложение для вычисления общей суммы и всех промежуточных
// сумм простых чисел последовательности.

const unsigned int MIN_THREAD_NUMBER = 1;
const unsigned int MAX_THREAD_NUMBER = 8;

bool isNotPrime(unsigned int n) {
  if (n > 1) {
    for (size_t i = 2; i < n; ++i) {
      if (n % i == 0) {
        return true;
      }
    }
  return false;
  } else {
    return true;
  }
}

int main(int argc, char *argv[]) {

  /* Аргументы программы:
  1. Последовательность натуральных чисел
  После обработки аргументов нужно будет ввести количество потоков.
  */

  std::vector<unsigned int> sequence;
  for (size_t i = 0; i < argc - 1; ++i) {
      sequence.push_back(std::atoi(argv[i + 1]));
      std::cout << std::atoi(argv[i + 1]) << " ";
  }

  unsigned int numThreads = MIN_THREAD_NUMBER;
  std::cout << "\nEnter number of threads: ";
  std::cin >> numThreads;
  if (numThreads < MIN_THREAD_NUMBER) {
    numThreads = MIN_THREAD_NUMBER;
  } else if (numThreads > MAX_THREAD_NUMBER) {
    numThreads = MAX_THREAD_NUMBER;
  }
  omp_set_num_threads(numThreads);
  std::cout << "\nNumber of threads: " << numThreads << '\n';

  sequence.erase(std::remove_if(sequence.begin(), sequence.end(), isNotPrime), sequence.end());

  unsigned int sum = 0;
  std::vector<unsigned int> runningTotals(sequence.size(), 0);

#pragma omp parallel for schedule(static) reduction(+:sum)
  for (size_t i = 0; i < sequence.size(); ++i) {
    sum += sequence[i];
    runningTotals[i] += sum;
  }

  std::cout << "\nRunning totals: ";
  for (size_t runningTotal : runningTotals) {
    std::cout << runningTotal << " ";
  }
  std::cout << "\n\nTotal sum of prime number: " << sum << '\n';

  return 0;
}