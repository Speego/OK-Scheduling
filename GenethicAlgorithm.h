#ifndef GENETHICALGORITHM_H
#define GENETHICALGORITHM_H

#include <cmath>
#include <ctime>

#include "Generator.h"

class Population
{
public:
    int populationSize;
    vector<Instance> population;

    Population(int, Instance*, int, int, int);

    int algorithmManager(int, int, int, float, float);

private:
    void initiatePopulation(int, Instance*, int, int, int);

    void sortObjectiveFunctionsReversed();

    void randomShuffle();

    void mutation(float);
    void crossover(float, int);

    void selectionToNextGenerationTournament();
    void selectionToNextGenerationRoulette();

    void makeDistributionFunction(vector <double>&);
};

bool compareObjectiveFunctionsReversed(Instance, Instance);

#endif // GENETHICALGORITHM_H
