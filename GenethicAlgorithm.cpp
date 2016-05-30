#include "GenethicAlgorithm.h"

bool compareObjectiveFunctionsReversed(Instance x, Instance y) {
    return x.objectiveFunction > y.objectiveFunction;
}

Population::Population(int amount, Instance *startInstance, int maxReadyTaskTime, int numberOfTasks, int numberOfBreaks){
    populationSize = amount;
    initiatePopulation(populationSize, startInstance, maxReadyTaskTime, numberOfTasks, numberOfBreaks);
}

/*
    manages all genethic algorithm
    -> counts time
    -> mutates and crosses orders in Population
    -> selects orders
        -> if less than a half of execution time went by, selects by tournament
        -> else selects by roulette
    -> saves the best result to a txt file
*/
int Population::algorithmManager(int numberOfTasks, int numberOfInstance, int executionTime, float mutationPercent, float crossoverPercent) {
    int bestRandomResult;
    clock_t startTime;
    clock_t endTime;

    bestRandomResult = population[0].objectiveFunction;
    for (int i=1; i<(int)population.size(); i++)
        if (population[i].objectiveFunction < bestRandomResult)
            bestRandomResult = population[i].objectiveFunction;

    startTime = clock();
    endTime = startTime + executionTime * CLOCKS_PER_SEC;
    while (clock() < endTime) {
        mutation(mutationPercent);
        crossover(crossoverPercent, numberOfTasks);
        if (clock() < (endTime - startTime)/2)
            selectionToNextGenerationTournament();
        else
            selectionToNextGenerationRoulette();
    }

    sortObjectiveFunctionsReversed();
    population[(int)population.size()-1].saveResult(bestRandomResult, numberOfInstance);

	return bestRandomResult;
}

/*
    initiates population and generates random orders
*/
void Population::initiatePopulation(int populationSize, Instance *startInstance, int maxReadyTaskTime, int numberOfTasks, int numberOfBreaks) {
    for (int i=0; i<populationSize; i++) {
        population.push_back(*startInstance);
        population[i].generateRandomSolution(maxReadyTaskTime, numberOfTasks, numberOfBreaks);
    }
}

void Population::sortObjectiveFunctionsReversed() {
    sort(population.begin(), population.end(), compareObjectiveFunctionsReversed);
}

/*
    shuffles orders in Population
*/
void Population::randomShuffle() {
    for (int i=(int)population.size()-1; i>0; i--)
        swap(population[i], population[rand()%i]);
}

/*
    draws Task and mutates it twice; there is not possible to mutate more than 100% of orders in population
*/
void Population::mutation(float mutationPercent) {
    vector <int> temporaryPopulation;
    int mutationNumber = mutationPercent * populationSize;
    int instanceToMutate;

    for (int i=0; i<populationSize; i++)
        temporaryPopulation.push_back(i);

    for (int i=0; i<mutationNumber; i++) {
        instanceToMutate = rand() % (int)temporaryPopulation.size();

        population.push_back(population[temporaryPopulation[instanceToMutate]]);
        population[(int)population.size()-1].mutate();
        population.push_back(population[temporaryPopulation[instanceToMutate]]);
        population[(int)population.size()-1].mutate();

        temporaryPopulation.erase(temporaryPopulation.begin() + instanceToMutate);
    }
}

/*
    draws two Tasks and crosses the first with the second and the other way; there is not possible to cross more than populationSize - 1 of orders in population
*/
void Population::crossover(float crossoverPercent, int numberOfTasks) {
    vector <int> temporaryPopulation;
    int crossoverNumber = crossoverPercent * populationSize;
    int instanceToCross1;
    int instanceToCross2;
    int instanceToErase;
    int wall;

    for (int i=0; i<populationSize; i++)
        temporaryPopulation.push_back(i);

    for (int i=0; i<crossoverNumber; i++) {
        instanceToCross1 = rand() % (int)temporaryPopulation.size();
        instanceToCross2 = rand() % (int)temporaryPopulation.size();
        while (instanceToCross2 == instanceToCross1)
            instanceToCross2 = rand() % (int)temporaryPopulation.size();

        wall = rand() % (numberOfTasks - 1);

        population.push_back(population[temporaryPopulation[instanceToCross1]]);
        population[(int)population.size()-1].cross(wall, &population[temporaryPopulation[instanceToCross2]]);
        population.push_back(population[temporaryPopulation[instanceToCross2]]);
        population[(int)population.size()-1].cross(wall, &population[temporaryPopulation[instanceToCross1]]);

        instanceToErase = rand() % 2;
        if (instanceToErase == 0)
            temporaryPopulation.erase(temporaryPopulation.begin() + instanceToCross1);
        else
            temporaryPopulation.erase(temporaryPopulation.begin() + instanceToCross2);
    }
}

/*
    selects orders to the next population by tournaments
    -> selects few best orders to the new Population (elites)
    -> shuffles other orders
    -> counts number of tournament players which is n or n+1 whether number of candidates to new population divided by population size is not integer
    -> selects tournaments winners to the new population
*/
void Population::selectionToNextGenerationTournament() {
    vector <Instance> temporaryPopulation;
    float multiplier;
    float lowerNumberOfTournamentPlayers;
    float upperNumberOfTournamentPlayers;
    int numberOfLowers;
    int numberOfUppers;
    int numberOfElites = populationSize / 12;
    int currentInstance = 0;
    int bestPlayer;

    sortObjectiveFunctionsReversed();
    if (numberOfElites < 1)
        numberOfElites = 1;
    for (int i=0; i<numberOfElites; i++) {
        temporaryPopulation.push_back(population[(int)population.size()-1]);
        population.pop_back();
    }

    randomShuffle();
    multiplier = (float)population.size() / (populationSize - numberOfElites);
    multiplier = modf(multiplier, &lowerNumberOfTournamentPlayers);
    upperNumberOfTournamentPlayers = lowerNumberOfTournamentPlayers + 1;
    numberOfUppers = multiplier * (populationSize - numberOfElites) + 1;
    numberOfLowers = populationSize - numberOfElites - numberOfUppers;

    for (int i=0; i<numberOfLowers; i++) {
        bestPlayer = currentInstance;
        for (int j=1; j<(int)lowerNumberOfTournamentPlayers; j++)
            if (population[currentInstance+j].objectiveFunction < population[bestPlayer].objectiveFunction)
                bestPlayer = currentInstance + j;

        temporaryPopulation.push_back(population[bestPlayer]);
        currentInstance += lowerNumberOfTournamentPlayers;
    }

    for (int i=0; i<numberOfUppers; i++) {
        bestPlayer = currentInstance;
        for (int j=1; j<(int)upperNumberOfTournamentPlayers; j++)
            if (population[currentInstance+j].objectiveFunction < population[bestPlayer].objectiveFunction)
                bestPlayer = currentInstance + j;

        temporaryPopulation.push_back(population[bestPlayer]);
        currentInstance += upperNumberOfTournamentPlayers;
    }

    population = temporaryPopulation;
}

/*
    select orders to the new population bu roulette
    -> selects few best orders to the new Population (elites)
    -> makes distribution function of orders
    -> draws a double number between 0 and 1 and assigns it to an order
    -> selects assigned order to the new population
*/
void Population::selectionToNextGenerationRoulette() {
    vector <Instance> temporaryPopulation;
    vector <double> distributionFunction;
    int numberOfElites = populationSize / 12;
    int startOfVector;
    int endOfVector;
    int currentPosition;
    double probability;

    sortObjectiveFunctionsReversed();
    if (numberOfElites < 1)
        numberOfElites = 1;
    for (int i=0; i<numberOfElites; i++) {
        temporaryPopulation.push_back(population[(int)population.size()-1]);
        population.pop_back();
    }

    for (int i=0; i<populationSize-numberOfElites; i++) {
        probability = (double)rand() / (RAND_MAX);

        distributionFunction.clear();
        makeDistributionFunction(distributionFunction);

        startOfVector = 0;
        endOfVector = (int)distributionFunction.size() - 1;
        currentPosition = (startOfVector + endOfVector) / 2;

        if (probability == 0.0)
            currentPosition = 0;
        else
            while (!(distributionFunction[currentPosition] < probability && distributionFunction[currentPosition+1] >= probability)) {
                if (distributionFunction[currentPosition] > probability)
                    endOfVector = currentPosition - 1;
                else
                    startOfVector = currentPosition + 1;
                currentPosition = (startOfVector + endOfVector) / 2;
            }

        temporaryPopulation.push_back(population[currentPosition]);
        population.erase(population.begin() + currentPosition);
    }

    population = temporaryPopulation;
}

/*
    makes distribution function to count probability of choosing specific order to the new population
    -> counts point zero which is a number a little bigger than the worst objective function
    -> sums fitnesses that is difference between point zero and objective function
    -> counts probability of each order and makes a distribution function
*/
void Population::makeDistributionFunction(vector <double>& distributionFunction) {
    int pointZero = population[0].objectiveFunction * 1.003;
    long long int sumOfFitnesses = 0;
    double probability;

    for (int i=0; i<(int)population.size(); i++)
        sumOfFitnesses += (pointZero - population[i].objectiveFunction);

    distributionFunction.push_back(0.0);
    for (int i=0; i<(int)population.size(); i++) {
        probability = (double)(pointZero - population[i].objectiveFunction) / (double)sumOfFitnesses;
        distributionFunction.push_back(distributionFunction[i] + probability);
    }
}
