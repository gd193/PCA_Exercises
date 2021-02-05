//
// Created by eric on 08.01.21.
//

#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include <mpi.h>

int main(int argc, char *argv[])
{
  int Ping_Pong_count, size_of_message;
  if (argc == 3)
    {
      char *p;
      size_of_message = std::strtol(argv[1], &p , 10);
      Ping_Pong_count = std::strtol(argv[2], &p, 10);
    }
  else
    {
      std::cout<< "Not the right amount of arguments given.\n";
      exit(-1);
    }

  std::vector<std::string> messageSend = {};
  std::vector<std::string> messageRecv = {};
  std::vector<double> runtime_PingPong = {};

  messageSend.emplace_back(pow(2, size_of_message), 'a');

  int rank, size;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  for (int i = 0; i < messageSend.size(); ++i)
    {
      runtime_PingPong.push_back(0);
      for (int j = 0; j < Ping_Pong_count+1; ++j)
        {
          int partner_rank = (rank % 2 == 0) ? rank+1 : rank-1;
          char *buffer = (char*) malloc(messageSend[i].length());

          double start = MPI_Wtime();
          if (rank == (j % 2))
            {
              MPI_Send(messageSend[i].c_str(), messageSend[i].length(), MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD);
              MPI_Recv(buffer, messageSend[i].length(), MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD, &status);
              messageRecv.emplace_back(buffer, messageSend[i].length());
              free(buffer);
            }
          else
            {
              MPI_Recv(buffer, messageSend[i].length(), MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD, &status);
              messageRecv.emplace_back(buffer, messageSend[i].length());
              MPI_Send(messageSend[i].c_str(), messageSend[i].length(), MPI_CHAR, partner_rank, 0, MPI_COMM_WORLD);
              free(buffer);
            }
          double end = MPI_Wtime();

          if (messageSend[i] == messageRecv[i]) printf("%d sent and received message of size %lu to %d\n", rank, messageSend[i].length(), partner_rank);
          else printf("Error when PingPonging with %d a message of size %lu to %d\n", rank, messageSend[i].length(), partner_rank);
          if (j != 0) runtime_PingPong[i] += end - start;
        }
      runtime_PingPong[i] /= Ping_Pong_count;
    }

  if (rank == 0)
    {
      std::cout<< "Die Messergebnisse für PingExchange in MPI\n";
      std::cout<< "Node | Message size | runtime" << std::endl;
      FILE *f;
      f = fopen("resultsPingExchange.txt", "a");

      for( auto time : runtime_PingPong)
        {
          std::cout<< rank << " | " << size_of_message << " | " << time << std::endl;
          fprintf(f, "%d %f", size_of_message, time);
        }

      fclose(f);
    }

  MPI_Finalize();
  return 0;
}
