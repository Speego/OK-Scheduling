#include <iostream>
#include <cstdlib>
#include <ctime>

#include "Generator.h"
#include "GenethicAlgorithm.h"

using namespace std;

void testingTime(int numberOfTasks, int maxTaskTime, int maxReadyTaskTime, int numberOfBreaks, int maxBreakTime, int maxBreakStartTime, int populationSize) {
    int executionTime;

    ofstream resultsToTestTime;
    resultsToTestTime.open("time1.txt");

    for (int i=1; i<=1; i++) {
        Instance *inst = new Instance();
        inst->generateTasks(numberOfTasks, maxTaskTime, maxReadyTaskTime);
        inst->generateBreaks(numberOfBreaks, maxBreakTime, maxBreakStartTime, i, numberOfTasks);
        resultsToTestTime << "input0" << i << ";";

        executionTime = 15;
        while (executionTime <= 15) {
            Population *p = new Population(populationSize, inst, maxReadyTaskTime, numberOfTasks, numberOfBreaks);
            p->algorithmManager(numberOfTasks, i, executionTime, 0.2, 0.6);
            resultsToTestTime << (*p).population[(int)(*p).population.size()-1].objectiveFunction << ";";
            delete p;

            if (executionTime == 0)
                executionTime = 1;
            else if (executionTime == 1)
                executionTime = 5;
            else
                executionTime += 5;
        }

        resultsToTestTime << endl;
        delete inst;
    }

    resultsToTestTime.close();
}

void testingMutationAndCrossover(int numberOfTasks, int maxTaskTime, int maxReadyTaskTime, int numberOfBreaks, int maxBreakTime, int maxBreakStartTime, int populationSize, int executionTime) {
    float mutationPercent;
    float crossoverPercent;

    ofstream resultsToTestMutationAndCrossover;
    resultsToTestMutationAndCrossover.open("genethic1.txt");

    for (int i=1; i<=5; i++) {
        Instance *inst = new Instance();
        inst->generateTasks(numberOfTasks, maxTaskTime, maxReadyTaskTime);
        inst->generateBreaks(numberOfBreaks, maxBreakTime, maxBreakStartTime, i, numberOfTasks);
        resultsToTestMutationAndCrossover << "input" << i;

        mutationPercent = 0.1;
        while (mutationPercent < 0.91) {
            crossoverPercent = 0.1;

            while (crossoverPercent < 0.91) {
                Population *p = new Population(populationSize, inst, maxReadyTaskTime, numberOfTasks, numberOfBreaks);
                p->algorithmManager(numberOfTasks, i, executionTime, mutationPercent, crossoverPercent);
                resultsToTestMutationAndCrossover << ";" << (*p).population[(int)(*p).population.size()-1].objectiveFunction;
                delete p;

                crossoverPercent += 0.1;
            }

            mutationPercent += 0.1;
	    resultsToTestMutationAndCrossover << endl;
        }

        resultsToTestMutationAndCrossover << endl;
        delete inst;
    }

    resultsToTestMutationAndCrossover.close();
}

void testingPopulationSize(int numberOfTasks, int maxTaskTime, int maxReadyTaskTime, int numberOfBreaks, int maxBreakTime, int maxBreakStartTime, int executionTime, float mutationPercent, float crossoverPercent) {
    int populationSize;

    ofstream resultsToTestPopulationSize;
    resultsToTestPopulationSize.open("population1.txt");

    for (int i=1; i<=10; i++) {
        Instance *inst = new Instance();
        inst->generateTasks(numberOfTasks, maxTaskTime, maxReadyTaskTime);
        inst->generateBreaks(numberOfBreaks, maxBreakTime, maxBreakStartTime, i, numberOfTasks);
        resultsToTestPopulationSize << "input" << i;

        populationSize = 10;
        while (populationSize <= 100) {
            Population *p = new Population(populationSize, inst, maxReadyTaskTime, numberOfTasks, numberOfBreaks);
            p->algorithmManager(numberOfTasks, i, executionTime, mutationPercent, crossoverPercent);
            resultsToTestPopulationSize << ";" << (*p).population[(int)(*p).population.size()-1].objectiveFunction;
            delete p;

            populationSize += 10;
        }

        resultsToTestPopulationSize << endl;
        delete inst;
    }

    resultsToTestPopulationSize.close();
}

void testingNumberOfBreaks(int numberOfTasks, int maxTaskTime, int maxReadyTaskTime, int maxBreakTime, int maxBreakStartTime, int executionTime, float mutationPercent, float crossoverPercent, int populationSize) {
	int numberOfBreaks;
	int bestRandomResult;

	ofstream resultsToTestNumberOfBreaks;
	resultsToTestNumberOfBreaks.open("numberOfBreaks2.txt");

	for (int i=26; i<=30; i++) {
		numberOfBreaks = 2;
		resultsToTestNumberOfBreaks << "input" << i;

		while (numberOfBreaks <= 50) {
			Instance *inst = new Instance();
			inst->generateTasks(numberOfTasks, maxTaskTime, maxReadyTaskTime);
			inst->generateBreaks(numberOfBreaks, maxBreakTime, maxBreakStartTime, i, numberOfTasks);

			Population *p = new Population(populationSize, inst, maxReadyTaskTime, numberOfTasks, numberOfBreaks);
            		bestRandomResult = p->algorithmManager(numberOfTasks, i, executionTime, mutationPercent, crossoverPercent);
            		resultsToTestNumberOfBreaks << ";" << bestRandomResult -  (*p).population[(int)(*p).population.size()-1].objectiveFunction;
			delete inst;
           		delete p;

			numberOfBreaks += 2;
		}

		resultsToTestNumberOfBreaks << endl;
	}

	resultsToTestNumberOfBreaks.close();
}

void testingMaxTaskTime(int numberOfTasks, int maxReadyTaskTime, int numberOfBreaks, int maxBreakTime, int maxBreakStartTime, int executionTime, float mutationPercent, float crossoverPercent, int populationSize) {
	int maxTaskTime;
	int bestRandomResult;

	ofstream resultsToTestMaxTaskTime;
	resultsToTestMaxTaskTime.open("maxTaskTime1.txt");

	for (int i=1; i<=5; i++) {
		maxTaskTime = 10;
		resultsToTestMaxTaskTime << "input" << i;

		while (maxTaskTime <= 200) {
			Instance *inst = new Instance();
			inst->generateTasks(numberOfTasks, maxTaskTime, maxReadyTaskTime);
			inst->generateBreaks(numberOfBreaks, maxBreakTime, maxBreakStartTime, i, numberOfTasks);

			Population *p = new Population(populationSize, inst, maxReadyTaskTime, numberOfTasks, numberOfBreaks);
            		bestRandomResult = p->algorithmManager(numberOfTasks, i, executionTime, mutationPercent, crossoverPercent);
            		resultsToTestMaxTaskTime << ";" << (float)(bestRandomResult -  (*p).population[(int)(*p).population.size()-1].objectiveFunction) / (float)bestRandomResult;
			delete inst;
           		delete p;

			maxTaskTime += 10;
		}

		resultsToTestMaxTaskTime << endl;
	}

	resultsToTestMaxTaskTime.close();
}

void testingMaxReadyTaskTime(int numberOfTasks, int maxTaskTime, int numberOfBreaks, int maxBreakTime, int maxBreakStartTime, int executionTime, float mutationPercent, float crossoverPercent, int populationSize) {
	int maxReadyTaskTime;
	int bestRandomResult;

	ofstream resultsToTestMaxReadyTaskTime;
	resultsToTestMaxReadyTaskTime.open("maxReadyTaskTime2.txt");

	for (int i=26; i<=30; i++) {
		maxReadyTaskTime = 1;
		resultsToTestMaxReadyTaskTime << "input" << i;

		while (maxReadyTaskTime <= numberOfTasks * maxTaskTime) {
			Instance *inst = new Instance();
			inst->generateTasks(numberOfTasks, maxTaskTime, maxReadyTaskTime);
			inst->generateBreaks(numberOfBreaks, maxBreakTime, maxBreakStartTime, i, numberOfTasks);

			Population *p = new Population(populationSize, inst, maxReadyTaskTime, numberOfTasks, numberOfBreaks);
            		bestRandomResult = p->algorithmManager(numberOfTasks, i, executionTime, mutationPercent, crossoverPercent);
            		resultsToTestMaxReadyTaskTime << ";" << (float)(bestRandomResult -  (*p).population[(int)(*p).population.size()-1].objectiveFunction) / (float)bestRandomResult;
			delete inst;
			delete p;

			maxReadyTaskTime += (numberOfTasks * maxTaskTime / 20);
		}

		resultsToTestMaxReadyTaskTime << endl;
	}

	resultsToTestMaxReadyTaskTime.close();
}

void testingMaxBreakTime(int numberOfTasks, int maxTaskTime, int numberOfBreaks, int maxReadyTaskTime, int maxBreakStartTime, int executionTime, float mutationPercent, float crossoverPercent, int populationSize) {
	int maxBreakTime;
	int bestRandomResult;

	ofstream resultsToTestMaxBreakTime;
	resultsToTestMaxBreakTime.open("maxBreakTime1.txt");

	for (int i=1; i<=5; i++) {
		maxBreakTime = 1;
		resultsToTestMaxBreakTime << "input" << i;

		while (maxBreakTime <= (maxTaskTime * 5) + 1) {
			Instance *inst = new Instance();
			inst->generateTasks(numberOfTasks, maxTaskTime, maxReadyTaskTime);
			inst->generateBreaks(numberOfBreaks, maxBreakTime, maxBreakStartTime, i, numberOfTasks);

			Population *p = new Population(populationSize, inst, maxReadyTaskTime, numberOfTasks, numberOfBreaks);
            		bestRandomResult = p->algorithmManager(numberOfTasks, i, executionTime, mutationPercent, crossoverPercent);
            		resultsToTestMaxBreakTime << ";" << (float)(bestRandomResult -  (*p).population[(int)(*p).population.size()-1].objectiveFunction) / (float)bestRandomResult;
			delete inst;
			delete p;

			maxBreakTime += (maxTaskTime / 2);
		}

		resultsToTestMaxBreakTime << endl;
	}

	resultsToTestMaxBreakTime.close();
}

void testingMaxBreakStartTime(int numberOfTasks, int maxTaskTime, int numberOfBreaks, int maxReadyTaskTime, int maxBreakTime, int executionTime, float mutationPercent, float crossoverPercent, int populationSize) {
	int maxBreakStartTime;
	int bestRandomResult;

	ofstream resultsToTestMaxBreakStartTime;
	resultsToTestMaxBreakStartTime.open("maxBreakStartTime1.txt");

	for (int i=1; i<=5; i++) {
		maxBreakStartTime = 1;
		resultsToTestMaxBreakStartTime << "input" << i;

		while (maxBreakStartTime <= maxTaskTime * numberOfTasks) {
			Instance *inst = new Instance();
			inst->generateTasks(numberOfTasks, maxTaskTime, maxReadyTaskTime);
			inst->generateBreaks(numberOfBreaks, maxBreakTime, maxBreakStartTime, i, numberOfTasks);

			Population *p = new Population(populationSize, inst, maxReadyTaskTime, numberOfTasks, numberOfBreaks);
            		bestRandomResult = p->algorithmManager(numberOfTasks, i, executionTime, mutationPercent, crossoverPercent);
            		resultsToTestMaxBreakStartTime << ";" << (float)(bestRandomResult -  (*p).population[(int)(*p).population.size()-1].objectiveFunction) / (float)bestRandomResult;
			delete inst;
			delete p;

			maxBreakStartTime += (maxTaskTime * numberOfTasks / 20);
		}

		resultsToTestMaxBreakStartTime << endl;
	}

	resultsToTestMaxBreakStartTime.close();
}

int main()
{
    srand(time(NULL));

    int numberOfTasks = 100;
int numberOfBreaks = 0.5 * numberOfTasks;
int maxTaskTime = 50;
int maxReadyTaskTime = numberOfTasks * maxTaskTime * 3/5;
int maxBreakTime = maxTaskTime;
int maxBreakStartTime = numberOfTasks * maxTaskTime * 4/5;

    int executionTime = 15;
    float mutationPercent = 0.1;
    float crossoverPercent = 0.6;
    int populationSize = 50;

	//testingTime(numberOfTasks, maxTaskTime, maxReadyTaskTime, numberOfBreaks, maxBreakTime, maxBreakStartTime, populationSize);
	//testingMaxReadyTaskTime(numberOfTasks, maxTaskTime, numberOfBreaks, maxBreakTime, maxBreakStartTime, executionTime, mutationPercent, crossoverPercent, populationSize);

    for (int i=91; i<=93; i++) {
	Instance *inst = new Instance();
	inst->generateTasks(numberOfTasks, maxTaskTime, maxReadyTaskTime);
	inst->generateBreaks(numberOfBreaks, maxBreakTime, maxBreakStartTime, i, numberOfTasks);

	Population *p = new Population(populationSize, inst, maxReadyTaskTime, numberOfTasks, numberOfBreaks);
    p->algorithmManager(numberOfTasks, i, executionTime, mutationPercent, crossoverPercent);
    delete inst;
    delete p;
    }

    return 0;
}
