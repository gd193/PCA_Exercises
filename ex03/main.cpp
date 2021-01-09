//
// Created by eric on 08.01.21.
//

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>

using Matrix = std::vector<std::vector<double>>;
using Clock = std::chrono::high_resolution_clock;
using DNanosecounds = std::chrono::duration<double, std::nano>;
using DMicrosecounds = std::chrono::duration<double, std::micro>;

void MatrixVektorMultZeilenw(Matrix const& m, std::vector<double> const& x, std::vector<double> &y);
void MatrixVektorMultSpaltenw(Matrix const& m, std::vector<double> const& x, std::vector<double> &y);
void MatrixVectorMult_nth_row(int numberOfThreads, int startingPosition, Matrix const& m, std::vector<double> const& x, std::vector<double> &y);

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
  std::cout<< "M x N | t_seq [us] | t_para [us] | Ratio" << std::endl;

  std::vector<int> matrixSizes = {10, 100, 500, 1000, 5000, 10000};

  FILE *f;
  f = fopen("results.txt", "a");

  int N = matrixSize, M = matrixSize;
  Matrix randomMat(N, std::vector<double>(M));
  std::vector<double> y(N, 0);
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
  MatrixVektorMultZeilenw(randomMat, randomVec1, y);
  auto endZeilenw = Clock::now();
  DMicrosecounds resultZeilenw = endZeilenw - startZeilenw;


  //sequentielle Ausführung mit spaltenweisem Zugriff
  /*auto startSpaltenw = Clock::now();
  MatrixVektorMultSpaltenw(randomMat, randomVec1, y);
  auto endSpaltenw = Clock::now();
  DMicrosecounds resultSpaltenw = endSpaltenw - startSpaltenw; */

  //parallele Ausführung mit zeilenweisem Zugriff mit n=numberThreads Threads
  auto startThreads = Clock::now();
  for (int i = 0; i < numberThreads; ++i) threadVector[i] = std::thread(MatrixVectorMult_nth_row, numberThreads, i, std::cref(randomMat), std::cref(randomVec1), std::ref(y));
  for (int i = 0; i < numberThreads; ++i) threadVector[i].join();
  auto endThreads = Clock::now();
  DMicrosecounds resultThreads = endThreads - startThreads;

  fprintf(f, "%d %d %f %f %f \n", matrixSize, numberThreads, resultZeilenw.count(), resultThreads.count(), resultZeilenw.count()/resultThreads.count());
  std::cout<< matrixSize << 'x' << numberThreads << " | " << resultZeilenw.count() << " | " << resultThreads.count() << " | " << resultZeilenw.count() / resultThreads.count() << std::endl;

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
