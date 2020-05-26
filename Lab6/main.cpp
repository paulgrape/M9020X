#include <omp.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <map>
#include <algorithm>

// Дана последовательность символов С = {с0…сn–1}. Дан набор из N пар кодирующих символов (ai,bi),
// т.е. все символы строки ai заменяются на bi. Создать многопоточное приложение, кодирующее строку С следующим образом:
// строка разделяется на подстроки и каждый поток осуществляет кодирование своей подстроки.
// Количество символов с последовательности, количество кодирующих пар и потоков являются входными параметрами программы,
// количество символов в строке может быть не кратно количеству потоков.

int main(int argc, char *argv[]) {

  /* Аргументы программы:
  1. Исходная строка, где будет производиться замена символов
  2. Количество кодирующих пар
  3. Количество потоков
  После обработки аргументов нужно последовательно ввести все кодирующие пары.
  */

  try {
    if (argc != 4) {
    throw std::invalid_argument("Incorrect amount of arguments");
    }
  } catch (const std::invalid_argument &ex) {
    std::cerr << ex.what() << '\n';
    return 1;
  }

  std::string consequence = argv[1];  // исходная строка, где будет производиться замена символов
  std::cout << "Consequence: " << consequence << '\n';

  const size_t numberOfCodePairs = std::atoi(argv[2]);  // количество кодирующих пар
  std::cout << "Number of code pairs: " << numberOfCodePairs << '\n';

  unsigned int threadsAmount = std::atoi(argv[3]);  // количество потоков
  if (threadsAmount > 4) {
    threadsAmount = 4;
  }
  omp_set_num_threads(threadsAmount);
  std::cout << "Amount of threads: " << threadsAmount << '\n';

  std::map <char, char> codePairs;  // чтение кодирующих пар
  for (size_t n = 0; n < numberOfCodePairs; ++n) {
    std::cout << "\nInsert " << n + 1 << " pair: ";
    char key, value;
    std::cin >> key >> value;
    codePairs.emplace(std::pair<char, char>(key, value));
  }

#pragma omp parallel for schedule(static)
  for (size_t i = 0; i < consequence.length(); ++i) {
    for (auto const& [key, value] : codePairs) {
      if (consequence[i] == key) {
        consequence[i] = value;
      }
    }
  }

  std::cout << "\nFinal consequence: " << consequence << '\n';
  return 0;
}
