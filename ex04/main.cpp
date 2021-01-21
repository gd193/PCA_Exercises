//
// Created by eric on 08.01.21.
//

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <algorithm>
#include <omp.h>

using Matrix = std::vector<std::vector<double>>;
using Clock = std::chrono::high_resolution_clock;
using DNanosecounds = std::chrono::duration<double, std::nano>;
using DMicrosecounds = std::chrono::duration<double, std::micro>;

void MatrixVektorMultZeilenw(Matrix const& m, std::vector<double> const& x, std::vector<double> &y);
void MatrixVektorMultSpaltenw(Matrix const& m, std::vector<double> const& x, std::vector<double> &y);
void MatrixVectorMult_nth_row(int numberOfThreads, int startingPosition, Matrix const& m, std::vector<double> const& x, std::vector<double> &y);
void MatrixVectorOpenMP(int numberOfThreads, Matrix const& m, std::vector<double> const& x, std::vector<double> &y);

int main(int argc, char *argv[])
{
  int matrixSize, numberThreads;
  if (argc == 3)
    {
      char *p;
      matrixSize = std::strtol(argv[1], &p, 10);
      numberThreads = std::strtol(argv[2], &p, 10);
    }
  else
    {
      std::cout<< "Not the right amount of arguments given. First argument is matrix size, second is number of threads.\n";
      exit(-1);
    }

  std::random_device rdevice;
  std::mt19937 rengine(rdevice());
  std::uniform_real_distribution<double> dist(-100., 100);

  std::cout<< "Die Messergebnisse sequenzielle und parallele Matrixmultiplikation:\n";
  std::cout<< "M x N | n threads | t_seq [us] | t_para [us] | Ratio" << std::endl;

  std::vector<int> matrixSizes = {10, 100, 500, 1000, 5000, 10000};

  FILE *f;
  f = fopen("results.txt", "a");

  int N = matrixSize, M = matrixSize;
  Matrix randomMat(N, std::vector<double>(M));
  std::vector<double> y(N, 0);
  std::vector<double> y_check(N, 0);
  std::vector<double> randomVec1(M, 0.);
  std::vector<std::thread> threadVector(numberThreads);

  for(auto v : randomMat)
    {
      for(auto e : v)
        {
          e = dist(rengine);
        }
    }
  for(auto e : randomVec1) e = dist(rengine);

  //sequentielle Ausführung mit zeilenweisem Zugriff
  auto startZeilenw = Clock::now();
  MatrixVektorMultZeilenw(randomMat, randomVec1, y_check);
  auto endZeilenw = Clock::now();
  DMicrosecounds resultZeilenw = endZeilenw - startZeilenw;


  //sequentielle Ausführung mit spaltenweisem Zugriff
  /*auto startSpaltenw = Clock::now();
  MatrixVektorMultSpaltenw(randomMat, randomVec1, y);
  auto endSpaltenw = Clock::now();
  DMicrosecounds resultSpaltenw = endSpaltenw - startSpaltenw; */

  //parallele Ausführung mit zeilenweisem Zugriff mit n=numberThreads Threads
  /*auto startThreads = Clock::now();
  for (int i = 0; i < numberThreads; ++i) threadVector[i] = std::thread(MatrixVectorMult_nth_row, numberThreads, i, std::cref(randomMat), std::cref(randomVec1), std::ref(y));
  for (int i = 0; i < numberThreads; ++i) threadVector[i].join();
  auto endThreads = Clock::now();
  DMicrosecounds resultThreads = endThreads - startThreads;*/

  auto startOpenMP = Clock::now();
  MatrixVectorOpenMP(numberThreads, randomMat, randomVec1, y);
  auto endOpenMP = Clock::now();
  DMicrosecounds  resultOpenMP = endOpenMP - startOpenMP;

  if (std::equal(y.begin(), y.end(), y_check.begin())) std::cout<< "Equal results for both methods obtained.\n";
  else std::cout<< "The results are not equal. Error in the multithreaded implementation.\n";

  fprintf(f, "%d %d %f %f %f \n", matrixSize, numberThreads, resultZeilenw.count(), resultOpenMP.count(), resultZeilenw.count()/resultOpenMP.count());
  std::cout<< matrixSize << 'x' << matrixSize << " | " << numberThreads << " | " << resultZeilenw.count() << " | " << resultOpenMP.count() << " | " << resultZeilenw.count() / resultOpenMP.count() << std::endl;

  fclose(f);

  return 0;
}

void MatrixVektorMultZeilenw(Matrix const& m, std::vector<double> const& x, std::vector<double> &y)
{
  for(auto e : y) e = 0.;
  for(int j = 0; j < m[0].size(); j++)
    {
      for(int i = 0; i < m.size(); i++)
        {
          y[i] += m[i][j] * x[j];
        }
    }
}


void MatrixVektorMultSpaltenw(Matrix const& m, std::vector<double> const& x, std::vector<double> &y)
{
  for(int i = 0; i < m.size(); i++)
    {
      y[i] = 0;
      for(int j = 0; j < m[0].size(); j++)
        {
          y[i] += m[i][j] * x[j];
        }
    }
}

void MatrixVectorMult_nth_row(int numberOfThreads, int startingPosition, const Matrix &m, const std::vector<double> &x, std::vector<double> &y)
{
  for(int i = startingPosition; i < m.size(); i += numberOfThreads)
    {
      y[i] = 0;
      for(int j = 0; j < m[0].size(); j++)
        {
          y[i] += m[i][j] * x[j];
        }
    }
}

void MatrixVectorOpenMP(int numberOfThreads, Matrix const& m, std::vector<double> const& x, std::vector<double> &y)
{
  int matrix_size = m.size();

#pragma omp parallel num_threads(numberOfThreads)
  {
    double* results_private = (double*)calloc(matrix_size, sizeof(double));
    for(int k = 0; k < matrix_size ; k++) {
#pragma omp for
        for(int i = 0; i < matrix_size; i++) {
            results_private[k] += x[i]*m[i][k];
          }
      }
#pragma omp critical
    {
      double test = results_private[0];
      for(int j=0; j<matrix_size; j++) y[j] += results_private[j];
    }
    free(results_private);
  }
}
