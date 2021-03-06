#ifndef UNSUPERVISEDTRICKIDENTIFIER_H
#define UNSUPERVISEDTRICKIDENTIFIER_H

#include <ostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include "../TrickDatabase/TrickDatabase.h"
#include "../EuclideanDistance/EuclideanDistance.h"

#include <iostream>
using std::cout;
using std::endl;

class UnsupervisedTrickIdentifier {
	public:
		UnsupervisedTrickIdentifier(std::string filename);
		void run(std::vector<std::vector<int> > inputData);
		std::string getName();
		double getCorrelation();
		friend std::ostream& operator <<(std::ostream& out, UnsupervisedTrickIdentifier& object);

	protected:
		TrickDatabase trickDatabase;
		std::vector<std::vector<std::vector<int> > > baseData;
		unsigned int baseDataHeigth;
		unsigned int baseDataWidth;
		unsigned int baseDataDepth;
		std::vector<std::vector<int> > inputData;
		int correlationMaxIndex;
		double correlationMax;
		EuclideanDistance<int> ed;
};

#endif // UNSUPERVISEDTRICKIDENTIFIER_H
