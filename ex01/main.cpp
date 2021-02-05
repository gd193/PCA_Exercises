#include <iostream>
#include <vector>
#include <random>
#include <chrono>

using Matrix = std::vector<std::vector<double>>;
using Clock = std::chrono::high_resolution_clock;
using DNanosecounds = std::chrono::duration<double, std::nano>;
using DMicrosecounds = std::chrono::duration<double, std::micro>;

void MatrixVektorMultZeilenw(Matrix const& m, std::vector<double> const& x, std::vector<double> &y);
void MatrixVektorMultSpaltenw(Matrix const& m, std::vector<double> const& x, std::vector<double> &y);


int main ()
{
  std::random_device rdevice;
  std::mt19937 rengine(rdevice());
  std::uniform_real_distribution<double> dist(-100., 100);

  std::cout<< "Die Messergebnisse für die spalten- und zeilenweise sequenzielle Matrixmultiplikation:\n";
  std::cout<< "M x N | t_row [us] | t_col [us] | Ratio" << std::endl;

  std::vector<int> matrixSizes = {10, 100, 500, 1000, 5000, 10000};

  FILE *f;
  f = fopen("results.txt", "w+");

  for ( auto size : matrixSizes)
    {
      int N = size, M = size;
      Matrix randomMat(N, std::vector<double>(M));
      std::vector<double> y(N, 0);
      std::vector<double> randomVec1(M, 0.);
      std::vector<double> randomVec2(M, 0.);

      for(auto v : randomMat)
        {
          for(auto e : v)
            {
              e = dist(rengine);
            }
        }
      for(auto e : randomVec1) e = dist(rengine);
      for(auto e : randomVec2) e = dist(rengine);



      auto startZeilenw = Clock::now();
      MatrixVektorMultZeilenw(randomMat, randomVec1, y);
      auto endZeilenw = Clock::now();
      DMicrosecounds resultZeilenw = endZeilenw - startZeilenw;

      auto startSpaltenw = Clock::now();
      MatrixVektorMultSpaltenw(randomMat, randomVec1, y);
      auto endSpaltenw = Clock::now();
      DMicrosecounds resultSpaltenw = endSpaltenw - startSpaltenw;

      fprintf(f, "%d %d %f %f %f \n", M, N, resultZeilenw.count(), resultSpaltenw.count(), resultSpaltenw.count()/resultZeilenw.count());
      std::cout<< M << 'x' << N << " | " << resultZeilenw.count() <<" | "<< resultSpaltenw.count()   << " | " << resultSpaltenw.count() / resultZeilenw.count() << std::endl;
    }
  fclose(f);
  return 0;
}

void MatrixVektorMultSpaltenw(Matrix const& m, std::vector<double> const& x, std::vector<double> &y)
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


void MatrixVektorMultZeilenw(Matrix const& m, std::vector<double> const& x, std::vector<double> &y)
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