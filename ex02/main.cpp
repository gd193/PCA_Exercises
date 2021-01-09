#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include<string>
#include <math.h>

using Matrix = std::vector<std::vector<double>>;
using Clock = std::chrono::high_resolution_clock;
using DNanosecounds = std::chrono::duration<double, std::nano>;
using DMicrosecounds = std::chrono::duration<double, std::micro>;

void initialize(Matrix &A,float m, float H);
void poissonstep(Matrix &A);

void initialize(Matrix &A,  float m, float H){
    double mid = (A.size()-1) / 2.0;
    double max_radius = m / 2.0;

    for(int i = static_cast<int>(mid-max_radius);i<=mid+max_radius;i++){
        for(int j = static_cast<int>(mid-max_radius);j<=mid+max_radius;j++){
            double dist = sqrt((i-mid)*(i-mid)+(j-mid)*(j-mid));
            if (dist<=max_radius) A[i][j] = H;
            //std::cout<<i<<"\t"<<j<<"\t"<<dist<<"\n";
        }
    };

}

void poissonstep(Matrix &A){
    Matrix C = A;
    double phi = 6./25.;
    for(int i=1;i<A.size()-1;i++){
        for(int j=1;j<A.size()-1;j++){
            A[i][j] += phi*(C[i][j-1]-4*C[i][j]+C[i][j+1]+C[i-1][j]+C[i+1][j]);
        }
    }
}

int main (int argc, char *argv[])
{
    int N = 100;
    float m = 10;
    float H = 127;
    if (argc >= 2) N = std::stoi(argv[1]);
    if (argc >= 3) m = std::stoi(argv[2]);
    if (argc >= 4) H = std::stoi(argv[3]);

    Matrix grid(N, std::vector<double>(N));
    initialize(grid, m, H);
    aueto start = Clock::now();
    for(int i=0;i<1000;i++){
        poissonstep(grid);
    };
    auto end = Clock::now();
    std::cout<< "N="<<N<<":\t The runtime of the programm is " << (end - start).count()/1e9  << "s" <<std::endl;
    FILE *f;
    f = fopen("results.txt", "w+");
    fprintf(f, "%d %f %f\n", N, m, (end-start).count()/1e9);
    fclose(f);

    if (N < 20)
    {
        std::cout<< "The result is shown below\n";
        for (auto row : grid)
        {
            for (auto ele : row) std::cout<<ele<<" ";
            std::cout<<std::endl;
        }
        std::cout<<std::endl;
    }
    else std::cout<<"Matrix size is " << N <<" and therefore to large to be printed out\n";
  return N;
}