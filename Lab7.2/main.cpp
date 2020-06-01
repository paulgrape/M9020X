#include <omp.h>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cstring>

void initializeMatrix(float** matrix, const unsigned int size) {
  for (size_t i = 0; i != size; ++i) {
    for (size_t j = 0; j != size; ++j) {
      std::cin >> matrix[i][j];
    }
  }
}

void printMatrix(float** matrix, const unsigned int size) {
  for (size_t i = 0; i != size; ++i) {
    for (size_t j = 0; j != size; ++j) {
      std::cout.precision(3);
      std::cout << *(*(matrix + i) + j) << ' ';
    }
    std::cout << std::endl;
  }
}

void deleteMatrix(float** matrix, const unsigned int rows) {
  for (size_t i = 0; i != rows; ++i) {
    delete[] matrix[i];
  }
  delete[] matrix;
}

float determinantOfMatrix(float** matrix, const unsigned int size, const unsigned int numberOfThreads) {
  if (size == 1) {
    return matrix[0][0];
  }
  if (size == 2) {
    return ((matrix[0][0] * matrix[1][1]) - (matrix[1][0] * matrix[0][1]));
  }
  auto **matrixCopy = new float *[size];
  for (size_t i = 0; i != size; ++i) {
    matrixCopy[i] = new float[size];
  }
  omp_set_num_threads(numberOfThreads);
#pragma omp parallel for schedule(static)
  for (size_t i = 0; i < size; ++i) {
    std::memcpy(matrixCopy[i], matrix[i], sizeof(matrix[0][0]) * size);
  }

  for (size_t step = 0; step != size - 1; ++step) {
#pragma omp parallel for schedule(static)
    for (size_t row = step + 1; row < size; ++row) {
      const float coeff = -matrixCopy[row][step] / matrixCopy[step][step];
      for (size_t col = step; col != size; ++col) {
        matrixCopy[row][col] += matrixCopy[step][col] * coeff;
      }
    }
  }

  float det = 1.0f;
#pragma omp parallel for schedule(static) reduction(*:det)
  for (size_t i = 0; i < size; ++i) {
    det *= matrixCopy[i][i];
  }
  deleteMatrix(matrixCopy, size);
  return det;
}

void getCofactorMatrix(float** matrix, float** result, const unsigned int p, const unsigned  q, const unsigned  n) {
  unsigned int i = 0, j = 0;
#pragma omp parallel for schedule(static) reduction(+:i,j)
  for (size_t row = 0; row < n; ++row) {
    for (size_t col = 0; col < n; ++col) {
      if ((row != p) && (col != q)) {
        result[i][++j] = matrix[row][col];
        if (j == (n - 1)) {
          j = 0;
          ++i;
        }
      }
    }
  }
}

float** calculateAlgebraicComplement(const unsigned int size, float **matrix, const unsigned int numberOfThreads) {
  auto **result = new float *[size];
#pragma omp parallel for schedule(static)
  for (size_t rows = 0; rows < size; ++rows) {
    result[rows] = new float[size];
  }
#pragma omp parallel for schedule(static)
  for (size_t i = 0; i < size; ++i) {
    for (size_t j = 0; j < size; ++j) {
      auto **cofactor = new float *[size - 1];
#pragma omp parallel for schedule(static)
      for (size_t rows = 0; rows < size - 1; ++rows) {
        cofactor[rows] = new float[size - 1];
      }

      getCofactorMatrix(matrix, cofactor, i, j, size);

      int sign = ((j + i) % 2) ? 1 : -1;
      float det = determinantOfMatrix(cofactor, size - 1, numberOfThreads);
      result[i][j] = sign * det;

      deleteMatrix(cofactor, size - 1);
    }
  }
  return result;
}

void launchInteraction(const unsigned int numberOfThreads) {
  std::cout << "Enter size of the matrix: ";
  unsigned int size = 0;
  std::cin >> size;
  if (size == 0) {
    throw std::invalid_argument("Incorrect matrix size\n");
  }
  auto **matrix = new float *[size];
  for (size_t i = 0; i != size; ++i) {
    matrix[i] = new float[size];
  }

  std::cout << "\nEnter matrix values:\n";
  initializeMatrix(matrix, size);
  std::cout << "\nMatrix:\n";
  printMatrix(matrix, size);

  auto **result = calculateAlgebraicComplement(size, matrix, numberOfThreads);

  std::cout << "\nMatrix of algebraic completements:\n";
  printMatrix(result, size);

  deleteMatrix(matrix, size);
  deleteMatrix(result, size);
}

int main (int argc, char** argv) {
//  Найти алгебраическое дополнение для каждого элемента матрицы.
//  целое положительное число n, произвольная матрица А размерности n х n.
//  Решить задачу двумя способами: 1) количество потоков является входным
//  параметром, при этом размерность матриц может быть не кратна количеству
//  потоков; 2) количество потоков заранее неизвестно и не является параметром
//  задачи.
  try {
    if (argc != 2) {
      throw std::invalid_argument("Incorrect number of arguments\n");
    }
    const int threads_amount = std::stoi(argv[1]);
    if (threads_amount < 1) {
      throw std::invalid_argument("Incorrect number of threads\n");
    }
    launchInteraction(static_cast<unsigned int>(threads_amount));
  } catch (const std::invalid_argument &arg) {
    std::cerr << arg.what();
    return 1;
  } catch (const std::out_of_range &ex) {
    std::cerr << ex.what();
    return 1;
  }

  return 0;
}