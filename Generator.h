#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include <set>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

class Task {
public:
    int readyTime1;
    int readyTime2;
    int startTime1;
    int startTime2;
    int lengthTime1;
    int lengthTime2;
    int idTask;

    Task(int);
    void generateTask(int, int);
};

class Break {
public:
    int idBreak;
    int startTime;
    int lengthTime;

    Break(int, int, int);
};

class Instance {
public:
    int objectiveFunction;
    vector<Task> tasks;
    vector<Break> breaks;

    void generateTasks(int, int, int);
    void generateBreaks(int, int, int, int, int);

    void generateRandomSolution(int, int, int);

    void mutate();
    void cross(int, Instance*);

    void saveInstance(int, int, int);
    void saveResult(int, int);

	bool check();
private:
    void sortReadyTime1();
    void sortStartTime1();
    void sortLengthTime1();
    void sortStartTime2();
    void sortIdTask();

    void randomShuffle(int, int);
    void fillGaps();

    int countObjectiveFunction();

    void correctBreaks(int, int);
    void avoidBreaks(int, int&);
    void setStartTimes1();
    void setStartTimes2();
    void correctStartTimes1();
    void correctStartTimes2();
    bool findTaskToFillGap(int, int, int);
    void makeBreaksAndTasksVector(vector <pair <int, bool> >&);
    void pushTasksToLeft(vector <pair <int, bool> >, int);
};

bool compareBreaks(Break, Break);
bool compareReadyTime1(Task, Task);
bool compareStartTime1(Task, Task);
bool compareLengthTime1(Task, Task);
bool compareStartTime2(Task, Task);
bool compareIdTask(Task, Task);

#endif // GENERATOR_H
