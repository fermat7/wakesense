#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "Parser/Parser.h"
#include "Receiver/Receiver.h"
#include "Index/Index.h"
#include "PCCTID/PCCTID.h"
#include "MLPTID/MLPTID.h"

/* -------- Constant -------- */

#define MIN_ARGC 2
#define DEFAULT_BASE_FILENAME "../data/base.dat"
#define PORT "/dev/tty.usbserial-A501B6XH"
#define BAUD 9600
#define INTERVAL 20
#define DURATION 1000

/* -------- Main -------- */

int main(int argc, char** argv)
{
  // Argument variables
  std::string type;
  bool in = false;
  bool out = false;
  bool print = false;

  // Parse command line arguments
  if (argc < MIN_ARGC)
  {
    std::cout << "Usage: [-t <PCC2D | PCC3D | MLPQuery | MLPTrain>] [-i] [-o] [-p] " << std::endl;
    exit(0);
  }
  else
  {
    for (int i = 1; i < argc; i++)
    {
      std::string arg(argv[i]);

      if (arg == "-t")
      {
        type = argv[++i];
      }
      else if (arg == "-i")
      {
        in = true;
      }
      else if (arg == "-o")
      {
        out = true;
      }
      else if (arg == "-p")
      {
        print = true;
      }
      else
      {
        std::cout << "Invalid arguments, please try again." << std::endl;
        exit(0);
      }
    }
  }

// Run
  std::vector<std::vector<int> > data;

  while (true)
  {
    // Get data
    if (in) // from file
    {
      std::string filename;
      std::cout << "Enter input filename: ";
      std::cin >> filename;

      Parser inputParser(filename);
      data = inputParser.getData()[0]; // get the first row of the matrix
    }
    else // from xbee
    {
      Receiver receiver(PORT, BAUD, INTERVAL, DURATION);

      if (out) // to file
      {
        std::string filename;
        std::cout << "Enter output filename: ";
        std::cin >> filename;

        receiver.getData(filename.c_str());
        Parser inputParser(filename);
        data = inputParser.getData()[0]; // get the first row of the matrix
      }
      else // not to file
      {
        data = receiver.getData();
      }
    }

    // Run data
		int id;
    std::string name;
		double correlation;
		int grade;

    if (type == "PCC2D")
    {
			// Parse base file
      PCCTID pcctid;
      Parser parser(DEFAULT_BASE_FILENAME);

			// From vector 2D to vector 3D
      std::vector<std::vector<std::vector<int> > > dataTemp;
      dataTemp.push_back(data);

			// Run PCCTID2D
			pcctid.run(true, dataTemp);

			// Get results
			id = pcctid.getId();
      name = parser.getId()[id];
			correlation = pcctid.getCorrelation();
			grade = -1;

      // Print results
      if (print) pcctid.print(std::cout);
    }
    else if (type == "PCC3D")
    {
			// Parse base file
      PCCTID pcctid;
      Parser parser(DEFAULT_BASE_FILENAME);

			// From vector 2D to vector 3D
      std::vector<std::vector<std::vector<int> > > dataTemp;
      dataTemp.push_back(data);

			// Run PCCTID3D
			pcctid.run(false, dataTemp);

			// Get results
			id = pcctid.getId();
      name = parser.getId()[id];
			correlation = pcctid.getCorrelation();
			grade = -1;

      // Print results
      if (print) pcctid.print(std::cout);
    }
    else if (type == "MLPQuery")
    {
			// Format data
			std::vector<int> yaw(data.size(), 0);
			std::vector<int> pitch(data.size(), 0);
			std::vector<int> roll(data.size(), 0);
			for (unsigned int i = 0; i < data.size(); i++)
			{
				yaw[i] = data[i][0];
				pitch[i] = data[i][1];
				roll[i] = data[i][2];
			}

			// Construct entry
			Entry entry("QUERY", yaw, pitch, roll);

			// Add entry to index
			Index index;
			index.setEntry(entry);

			// Query network
      MLPTID mlptid;
			mlptid.queryNetwork(entry.sampleIds, index.getEntryIds());

			// Get results
			id = mlptid.getId();
			name = index.getEntry(index.getEntryIds()[id]).name; // rowid starts from 1
		  correlation = mlptid.getCorrelation();
			grade = -1;

			// Print results
      if (print) 
			{          
				entry.print(std::cout);
				index.print(std::cout);
				mlptid.print(std::cout);
			}
    }
    else if (type == "MLPTrain")
    {
			// Get entry name
			std::cout << "Enter entry name: ";
			std::cin >> name;

			// Format data
			std::vector<int> yaw(data.size(), 0);
			std::vector<int> pitch(data.size(), 0);
			std::vector<int> roll(data.size(), 0);
			for (unsigned int i = 0; i < data.size(); i++)
			{
				yaw[i] = data[i][0];
				pitch[i] = data[i][1];
				roll[i] = data[i][2];
			}

			// Construct entry
			Entry entry(name.c_str(), yaw, pitch, roll);

			// Add entry to index
			Index index;
			index.setEntry(entry);
			std::vector<int> trickIds = index.getEntryIds();

			// Train network
      MLPTID mlptid;
			mlptid.trainNetwork(entry.sampleIds, trickIds, entry.trickId);

			// Get results
			id = entry.trickId;
			name = entry.name;
			correlation = -1;
			grade = -1;

			// Print results
      if (print) 
			{          
				entry.print(std::cout);
				index.print(std::cout);
				mlptid.print(std::cout);
			}
    }

    // Output results
    std::cout
        << std::endl
        << "--------------------------------------------------------" << std::endl
        << "class : Main" << std::endl
        << "field : Id, Name, Correlation, Grade" << std::endl
        << "--------------------------------------------------------" << std::endl
        << std::endl;

    std::cout
				<< std::setw(20) << std::left
        << "Id"
				<< std::setw(20) << std::left
        << "Name"
				<< std::setw(20) << std::left
        << "Correlation"
				<< std::setw(20) << std::left
        << "Grade"
				<< std::endl
				<< std::setw(20) << std::left
				<< id
				<< std::setw(20) << std::left
				<< name
				<< std::setw(20) << std::left
				<< correlation
				<< std::setw(20) << std::left
				<< grade
				<< std::endl
				<< std::endl;
  }

  return 0;
}
