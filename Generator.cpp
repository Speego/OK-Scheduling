#include "Generator.h"

bool compareBreaks(Break x, Break y) {
    return x.startTime < y.startTime;
}

bool compareReadyTime1(Task x, Task y) {
    return x.readyTime1 < y.readyTime1;
}

bool compareStartTime1(Task x, Task y) {
    return x.startTime1 < y.startTime1;
}

bool compareLengthTime1(Task x, Task y) {
    return x.lengthTime1 < y.lengthTime1;
}

bool compareStartTime2(Task x, Task y) {
    return x.startTime2 < y.startTime2;
}

bool compareIdTask(Task x, Task y) {
    return x.idTask < y.idTask;
}

Task::Task(int i) {
    readyTime1 = 0;
    readyTime2 = 0;
    startTime1 = 0;
    startTime2 = 0;
    lengthTime1 = 0;
    lengthTime2 = 0;
    idTask = i;
}

void Task::generateTask(int maxLengthTime, int maxReadyTime) {
    lengthTime1 = rand() % maxLengthTime + 1;
    lengthTime2 = rand() % maxLengthTime + 1;
    readyTime1 = rand() % maxReadyTime;
    readyTime2 = readyTime1 + lengthTime1;
}

Break::Break(int i, int maxLengthTime, int maxStartTime) {
    idBreak = i;
    startTime = rand() % maxStartTime + 5;
    lengthTime = rand() % maxLengthTime + 1;
}

/*
    creates a vector of new Tasks
*/
void Instance::generateTasks(int numberOfTasks, int maxLengthTime, int maxReadyTime) {
    for (int i=1; i<=numberOfTasks; i++) {
        tasks.push_back(*(new Task(i)));
        tasks[i-1].generateTask(maxLengthTime, maxReadyTime);
    }
}

/*
    creates a vector of new maintenance Breaks and saves the Instance to a txt file
*/
void Instance::generateBreaks(int numberOfBreaks, int maxLengthTime, int maxStartTime, int numberOfInstance, int numberOfTasks) {
    for (int i=1; i<=numberOfBreaks; i++)
        breaks.push_back(*(new Break(i, maxLengthTime, maxStartTime)));

    correctBreaks(numberOfBreaks, maxLengthTime);

    sortReadyTime1();
    tasks[0].readyTime1 = 0;
    sortIdTask();
    saveInstance(numberOfInstance, numberOfTasks, numberOfBreaks);
}


/*
    generates random result for the Instance
    -> divides Tasks sorted by ready time of first operations (readyTime1) into two parts
    -> mixes Tasks in parts to create random order
    -> sets startTimes1
    (T - Task; B - maintenance Break; G - idle gap)
    -> fills gaps like this
          T T T G B
       with Tasks found in further part of order
    -> sets second operations (startTimes2)
*/
void Instance::generateRandomSolution(int maxReadyTaskTime, int numberOfTasks, int numberOfBreaks) {
    int firstStageTime = maxReadyTaskTime / 4;
    int currentPosition = numberOfTasks / 2;
    int startOfVector = 0;
    int endOfVector = tasks.size() - 1;

    sortReadyTime1();

    if (tasks[0].readyTime1 > firstStageTime)
        currentPosition = 0;
    else if (tasks[(int)tasks.size() - 1].readyTime1 <= firstStageTime) currentPosition = (int)tasks.size() - 1;
    else
        while (tasks[currentPosition].readyTime1 > firstStageTime || tasks[currentPosition+1].readyTime1 <= firstStageTime) {
            if (tasks[currentPosition].readyTime1 > firstStageTime)
                endOfVector = currentPosition - 1;
            else
                startOfVector = currentPosition + 1;
            currentPosition = (startOfVector + endOfVector) / 2;
        }

    randomShuffle(1, currentPosition);
    randomShuffle(currentPosition+1, (int)tasks.size()-1);

    setStartTimes1();

    fillGaps();
    sortStartTime1();
    setStartTimes2();

    objectiveFunction = countObjectiveFunction();
}

/*
    mutates two tasks in order
    -> finds two Tasks with similar lengthTime1
    -> swaps first operation (startTime1) and second operation (startTime2) of these Tasks
    -> corrects first operations (startTimes1) of all Tasks
    (T - Task; B - maintenance Break; G - idle gap)
    -> fills gaps like this
          T T T G B
       with Tasks found in further part of order
    -> corrects second operations (startTimes2)
*/
void Instance::mutate() {
    int taskToMutate1;
    int taskToMutate2;
    int rangeStart;
    int rangeEnd;

    sortLengthTime1();

    taskToMutate1 = rand() % (int)tasks.size();
    rangeStart = taskToMutate1 - (int)tasks.size() / 5;
    if (rangeStart < 0)
        rangeStart = 0;
    rangeEnd = taskToMutate1 + (int)tasks.size() / 5;
    if (rangeEnd >= (int)tasks.size())
        rangeEnd = (int)tasks.size() - 1;
    taskToMutate2 = rand() % (rangeEnd - rangeStart + 1) + rangeStart;
    while (taskToMutate2 == taskToMutate1)
        taskToMutate2 = rand() % (rangeEnd - rangeStart + 1) + rangeStart;

    tasks[taskToMutate1].startTime1 += tasks[taskToMutate2].startTime1;
    tasks[taskToMutate2].startTime1 = tasks[taskToMutate1].startTime1 - tasks[taskToMutate2].startTime1;
    tasks[taskToMutate1].startTime1 -= tasks[taskToMutate2].startTime1;

    tasks[taskToMutate1].startTime2 += tasks[taskToMutate2].startTime2;
    tasks[taskToMutate2].startTime2 = tasks[taskToMutate1].startTime2 - tasks[taskToMutate2].startTime2;
    tasks[taskToMutate1].startTime2 -= tasks[taskToMutate2].startTime2;

    sortStartTime1();
    correctStartTimes1();

    //fillGaps();

    correctStartTimes2();
    sortStartTime1();

    objectiveFunction = countObjectiveFunction();
}

/*
    crosses two different orders
    wall - position of the last element which stays the same
    -> puts all Tasks to reorganize into a set
    -> puts all Tasks to reorganize into an order of the second Instance
    -> corrects first operations (startTimes1)
    -> rewrites startTimes2 in an order of first operations (startTimes1) behind the wall
*/
void Instance::cross(int wall, Instance* secondInstance) {
    set <int> tasksToLineUp;
    int temporaryTask = tasks[wall].idTask - 1;
    int temporaryIdTask;

    for (int i=wall+1; i<(int)tasks.size(); i++)
        tasksToLineUp.insert(tasks[i].idTask);

    sortIdTask();

    for (int i=0; i<(int)(*secondInstance).tasks.size(); i++) {
        temporaryIdTask = (*secondInstance).tasks[i].idTask;
        if (tasksToLineUp.find(temporaryIdTask) != tasksToLineUp.end()) {
            tasks[temporaryIdTask-1].startTime1 = tasks[temporaryTask].startTime1 + tasks[temporaryTask].lengthTime1;
            tasks[temporaryIdTask-1].startTime2 = tasks[temporaryTask].startTime2 + tasks[temporaryTask].lengthTime2;
            temporaryTask = temporaryIdTask - 1;
        }
    }

    sortStartTime1();
    correctStartTimes1();
    correctStartTimes2();
    sortStartTime1();

    objectiveFunction = countObjectiveFunction();
}

/*
    saves Instance's input as it is supposed to be done
*/
void Instance::saveInstance(int numberOfInstance, int numberOfTasks, int numberOfBreaks) {
    ofstream fileInput;
    string txtPath;
    ostringstream temporaryTxtPath;

    temporaryTxtPath << "input" << numberOfInstance << ".txt";
    txtPath = temporaryTxtPath.str();

    fileInput.open(txtPath.c_str());
    fileInput << "**** " << numberOfInstance << " ****" << endl << numberOfTasks << endl;

    for (int i=0; i<numberOfTasks; i++)
        //fileInput << tasks[i].idTask << ";" << tasks[i].lengthTime1 << ";" << tasks[i].lengthTime2 << ";1;2;" << tasks[i].readyTime1 << endl;
        fileInput << tasks[i].lengthTime1 << ";" << tasks[i].lengthTime2 << ";1;2;" << tasks[i].readyTime1 << endl;

    fileInput << endl;
    for (int i=0; i<numberOfBreaks; i++)
        fileInput << breaks[i].idBreak << ";1;" << breaks[i].lengthTime << ";" << breaks[i].startTime << endl;

    fileInput << "*** EOF ***";
    fileInput.close();
}

/*
    saves Instance's output as it is supposed to be done
*/
void Instance::saveResult(int bestRandomResult, int numberOfInstance) {
    ofstream fileOutput;
    string txtPath;
    ostringstream temporaryTxtPath;
    vector <pair <int, bool> > breaksAndTasks;
    int currentIdle = 1;
    int sumOfIdles1 = 0;
    int sumOfIdles2 = 0;
    int sumOfBreaks = 0;
    int endTimeOfPrevious = 0;

	if (check() == true)
		cout << "TRUE" << endl;
	else
		cout << "FALSE" << endl;
    makeBreaksAndTasksVector(breaksAndTasks);
    while (breaksAndTasks[(int)breaksAndTasks.size()-1].second == false)
        breaksAndTasks.pop_back();

    temporaryTxtPath << "output" << numberOfInstance << ".txt";
    txtPath = temporaryTxtPath.str();

    fileOutput.open(txtPath.c_str());
    fileOutput << "**** " << numberOfInstance << " ****" << endl;
    fileOutput << objectiveFunction << ", " << bestRandomResult << endl;
    fileOutput << "M1: ";

    for (int i=0; i<(int)breaksAndTasks.size(); i++) {
        if (breaksAndTasks[i].second == true) {
            if (tasks[breaksAndTasks[i].first].startTime1 != endTimeOfPrevious) {
                fileOutput << "idle" << currentIdle << "_M1, " << endTimeOfPrevious << ", " << tasks[breaksAndTasks[i].first].startTime1 - endTimeOfPrevious << "; ";
                currentIdle++;
                sumOfIdles1 += (tasks[breaksAndTasks[i].first].startTime1 - endTimeOfPrevious);
            }
            fileOutput << "op1_" << tasks[breaksAndTasks[i].first].idTask << ", " << tasks[breaksAndTasks[i].first].startTime1 << ", " << tasks[breaksAndTasks[i].first].lengthTime1 << "; ";
            endTimeOfPrevious = tasks[breaksAndTasks[i].first].startTime1 + tasks[breaksAndTasks[i].first].lengthTime1;
        }
        else {
            if (breaks[breaksAndTasks[i].first].startTime != endTimeOfPrevious) {
                fileOutput << "idle" << currentIdle << "_M1, " << endTimeOfPrevious << ", " << breaks[breaksAndTasks[i].first].startTime - endTimeOfPrevious << "; ";
                currentIdle++;
                sumOfIdles1 += (breaks[breaksAndTasks[i].first].startTime - endTimeOfPrevious);
            }
            fileOutput << "maint" << breaks[breaksAndTasks[i].first].idBreak << "_M1, " << breaks[breaksAndTasks[i].first].startTime << ", " << breaks[breaksAndTasks[i].first].lengthTime << "; ";
            sumOfBreaks += breaks[breaksAndTasks[i].first].lengthTime;
            endTimeOfPrevious = breaks[breaksAndTasks[i].first].startTime + breaks[breaksAndTasks[i].first].lengthTime;
        }
    }

    sortStartTime2();
    currentIdle = 1;
    endTimeOfPrevious = 0;
    fileOutput << endl << "M2: ";
    for (int i=0; i<(int)tasks.size(); i++) {
        if (tasks[i].startTime2 != endTimeOfPrevious) {
            fileOutput << "idle" << currentIdle << "_M2, " << endTimeOfPrevious << ", " << tasks[i].startTime2 - endTimeOfPrevious << "; ";
            currentIdle++;
            sumOfIdles2 += (tasks[i].startTime2 - endTimeOfPrevious);
        }
        fileOutput << "op2_" << tasks[i].idTask << ", " << tasks[i].startTime2 << ", " << tasks[i].lengthTime2 << "; ";
        endTimeOfPrevious = tasks[i].startTime2 + tasks[i].lengthTime2;
    }

    fileOutput << endl << sumOfBreaks << endl << "0" << endl << sumOfIdles1 << endl << sumOfIdles2 << endl;
    fileOutput << "**** EOF ****";
}

void Instance::sortReadyTime1() {
    sort(tasks.begin(), tasks.end(), compareReadyTime1);
}

void Instance::sortStartTime1() {
    sort(tasks.begin(), tasks.end(), compareStartTime1);
}

void Instance::sortLengthTime1() {
    sort(tasks.begin(), tasks.end(), compareLengthTime1);
}

void Instance::sortStartTime2() {
    sort(tasks.begin(), tasks.end(), compareStartTime2);
}

void Instance::sortIdTask() {
    sort(tasks.begin(), tasks.end(), compareIdTask);
}

/*
    shuffles Tasks in specified range
*/
void Instance::randomShuffle(int startPosition, int endPosition) {
    for (int i=endPosition; i>startPosition; i--)
        swap(tasks[i], tasks[(rand()%(i-startPosition+1)) + startPosition]);
}

/*
    (T - Task; B - maintenance Break; G - idle gap)
    fills gaps like this
        T T T G B
    with Tasks found in further part of order
    -> if there is no Task that starts at the moment 0, tries to find a Task to put it there
    -> creates vector of Tasks and Breaks sorted by startTime
    -> looks for the gaps
        -> if there is a gap, tries to find Task in further part of this vector to put it there
        -> if a Task is found, creates vector of Tasks and Breaks sorted by startTime again and corrects startTimes1 to make sure there are not any unnecessary idle gap
*/
void Instance::fillGaps() {
    int temporaryTask = -1;
    int gapLength;
    bool foundSomething;
    vector <pair <int, bool> > breaksAndTasks;

    if (tasks[0].startTime1 != 0) {
        gapLength = breaks[0].startTime;
        if (gapLength > 0)
            foundSomething = findTaskToFillGap(-1, 0, gapLength);
        if (foundSomething)
            pushTasksToLeft(breaksAndTasks, 1);
    }

    makeBreaksAndTasksVector(breaksAndTasks);

    for (int i=0; i<(int)breaksAndTasks.size()-1; i++) {
        if (breaksAndTasks[i].second == true) {
            temporaryTask++;
            if (breaksAndTasks[i+1].second == false) {
                gapLength = breaks[breaksAndTasks[i+1].first].startTime - tasks[breaksAndTasks[i].first].startTime1 - tasks[breaksAndTasks[i].first].lengthTime1;
                if (gapLength > 0)
                    foundSomething = findTaskToFillGap(breaksAndTasks[i].first, breaksAndTasks[i+1].first, gapLength);
                if (foundSomething) {
                    breaksAndTasks.clear();
                    makeBreaksAndTasksVector(breaksAndTasks);
                    pushTasksToLeft(breaksAndTasks, 1);
                }
            }
        }
        else
            if (breaksAndTasks[i].second == false && breaksAndTasks[i+1].second == false) {
                gapLength = breaks[breaksAndTasks[i+1].first].startTime - breaks[breaksAndTasks[i].first].startTime - breaks[breaksAndTasks[i].first].lengthTime;
                foundSomething = findTaskToFillGap(temporaryTask, breaksAndTasks[i+1].first, gapLength);
                if (foundSomething) {
                    breaksAndTasks.clear();
                    makeBreaksAndTasksVector(breaksAndTasks);
                    pushTasksToLeft(breaksAndTasks, 1);
                }
        }
    }
}

/*
    counts objective function of an Instance
*/
int Instance::countObjectiveFunction() {
    int temporaryObjectiveFunction = 0;

    for (int i=0; i<(int)tasks.size(); i++)
        temporaryObjectiveFunction += tasks[i].startTime1 + tasks[i].lengthTime1 + tasks[i].startTime2 + tasks[i].lengthTime2;

    return temporaryObjectiveFunction;
}

/*
    after creating maintenance Breaks, makes sure they are separated
*/
void Instance::correctBreaks(int numberOfBreaks, int maxLengthTime) {
    sort(breaks.begin(), breaks.end(), compareBreaks);

    for (int i=1; i<numberOfBreaks; i++)
        if (breaks[i].startTime <= (breaks[i-1].startTime + breaks[i-1].lengthTime))
            breaks[i].startTime = breaks[i-1].startTime + maxLengthTime + 1;

    for (int i=0; i<numberOfBreaks; i++)
        breaks[i].idBreak = i+1;
}

/*
    makes sure that Task is separated from any maintenance Break
*/
void Instance::avoidBreaks(int currentTask, int& currentBreak) {
    while ((currentBreak < (int)breaks.size()) && ((breaks[currentBreak].startTime - tasks[currentTask].startTime1) < tasks[currentTask].lengthTime1)) {
            tasks[currentTask].startTime1 = breaks[currentBreak].startTime + breaks[currentBreak].lengthTime;
            currentBreak++;
        }
}

/*
    sets first operations (startTimes1) of Tasks
    -> makes sure first operations do not start before they are ready
    -> makes sure first operations are separated from maintenance Breaks
*/
void Instance::setStartTimes1() {
    int currentBreak = 0;

    avoidBreaks(0, currentBreak);
    if (tasks[0].startTime1 < tasks[0].readyTime1)
        tasks[0].startTime1 = tasks[0].readyTime1;
    for (int i=1; i<(int)tasks.size(); i++) {
        tasks[i].startTime1 = tasks[i-1].startTime1 + tasks[i-1].lengthTime1;
        if (tasks[i].startTime1 < tasks[i].readyTime1)
            tasks[i].startTime1 = tasks[i].readyTime1;
        avoidBreaks(i, currentBreak);
    }
}

/*
    corrects startTimes1 after mutation and crossover
*/
void Instance::correctStartTimes1() {
    int current;
    vector <pair <int, bool> > breaksAndTasks;

    if (tasks[0].startTime1 < tasks[0].readyTime1)
        tasks[0].startTime1 = tasks[0].readyTime1;
    for (int i=1; i<(int)tasks.size(); i++) {
        tasks[i].startTime1 = tasks[i-1].startTime1 + tasks[i-1].lengthTime1;
        if (tasks[i].startTime1 < tasks[i].readyTime1)
            tasks[i].startTime1 = tasks[i].readyTime1;
    }

    makeBreaksAndTasksVector(breaksAndTasks);

    for (int i=1; i<(int)breaksAndTasks.size()-1; i++) {
        current = breaksAndTasks[i].first;
        if (breaksAndTasks[i].second == false) {
            if (breaksAndTasks[i-1].second == true && breaks[current].startTime < tasks[breaksAndTasks[i-1].first].startTime1 + tasks[breaksAndTasks[i-1].first].lengthTime1) {
                tasks[breaksAndTasks[i-1].first].startTime1 = breaks[current].startTime + breaks[current].lengthTime;
                for (int j=breaksAndTasks[i-1].first + 1; j<(int)tasks.size(); j++) {
                    if (tasks[j].startTime1 < tasks[j-1].startTime1 + tasks[j-1].lengthTime1)
                        tasks[j].startTime1 = tasks[j-1].startTime1 + tasks[j-1].lengthTime1;
                }
                breaksAndTasks.clear();
                makeBreaksAndTasksVector(breaksAndTasks);
            }
            else if (breaksAndTasks[i+1].second == true && tasks[breaksAndTasks[i+1].first].startTime1 < breaks[current].startTime + breaks[current].lengthTime) {
                tasks[breaksAndTasks[i+1].first].startTime1 = breaks[current].startTime + breaks[current].lengthTime;
                for (int j=breaksAndTasks[i+1].first + 1; j<(int)tasks.size(); j++) {
                    if (tasks[j].startTime1 < tasks[j-1].startTime1 + tasks[j-1].lengthTime1)
                        tasks[j].startTime1 = tasks[j-1].startTime1 + tasks[j-1].lengthTime1;
                }
            }
            breaksAndTasks.clear();
            makeBreaksAndTasksVector(breaksAndTasks);
        }
    }
    current = (int)breaksAndTasks.size()-1;
    if (breaksAndTasks[current].second == true && breaksAndTasks[current-1].second == false && tasks[breaksAndTasks[current].first].startTime1 < breaks[current-1].startTime + breaks[current-1].lengthTime)
        tasks[breaksAndTasks[current].first].startTime1 = breaks[current-1].startTime + breaks[current-1].lengthTime;

}

/*
    sets second operations (startTimes2) of Tasks
    -> creates a vector of Tasks which second operation (startTime2) are possible to be set at the given time
    -> draws a Task from this vector and sets its second operation (startTime2) at this moment
        -> repeats until vector is empty
        -> if there is any idle gap, jumps to the next Task
*/
void Instance::setStartTimes2() {
    int currentTask = 1;
    int currentTime;
    int taskToSet;
    vector <int> temporaryTasks;

    for (int i=0; i<(int)tasks.size(); i++)
        tasks[i].readyTime2 = tasks[i].startTime1 + tasks[i].lengthTime1;

    currentTime = tasks[0].readyTime2;
    temporaryTasks.push_back(0);
    while (!temporaryTasks.empty()) {
        taskToSet = rand() % (int)temporaryTasks.size();
        tasks[temporaryTasks[taskToSet]].startTime2 = currentTime;
        currentTime = tasks[temporaryTasks[taskToSet]].startTime2 + tasks[temporaryTasks[taskToSet]].lengthTime2;
        temporaryTasks.erase(temporaryTasks.begin() + taskToSet);

        while (currentTask < (int)tasks.size() && tasks[currentTask].readyTime2 <= currentTime) {
            temporaryTasks.push_back(currentTask);
            currentTask++;
        }

        if (temporaryTasks.empty() && currentTask < (int)tasks.size()) {
            currentTime = tasks[currentTask].readyTime2;
            temporaryTasks.push_back(currentTask);
            currentTask++;
        }
    }
}

/*
    makes sure that the second operation of a Task does not start before the end of a first operation of a Task
*/
void Instance::correctStartTimes2() {
    sortStartTime2();
    for (int i=0; i<(int)tasks.size(); i++)
        tasks[i].readyTime2 = tasks[i].startTime1 + tasks[i].lengthTime1;

    tasks[0].startTime2 = tasks[0].readyTime2;
    for (int i=1; i<(int)tasks.size(); i++) {
        tasks[i].startTime2 = tasks[i-1].startTime2 + tasks[i-1].lengthTime2;
        if (tasks[i].startTime2 < tasks[i].readyTime2)
            tasks[i].startTime2 = tasks[i].readyTime2;
    }
}

/*
    finds Task with the longest possible lengthTime1 to fill a gap
*/
bool Instance::findTaskToFillGap(int currentTask, int currentBreak, int gapLength) {
    int temporaryTask = -1;
    int temporaryMaxTaskLength = 0;

    for (int i=currentTask+1; i<(int)tasks.size(); i++) {
        if (tasks[i].lengthTime1 <= gapLength && tasks[i].lengthTime1 > temporaryMaxTaskLength && tasks[i].readyTime1 <= (breaks[currentBreak].startTime - gapLength)) {
            temporaryMaxTaskLength = tasks[i].lengthTime1;
            temporaryTask = i;
        }
    }

    if (temporaryTask != -1) {
        tasks[temporaryTask].startTime1 = breaks[currentBreak].startTime - gapLength;
        sortStartTime1();
        return true;
    }
    else return false;
}

/*
    makes a vector of breaks and tasks sorted by their startTimes on the first machine
*/
void Instance::makeBreaksAndTasksVector(vector <pair <int, bool> > &breaksAndTasks) {
    int temporaryBreak = 0;
    int temporaryTask = 0;

    while (temporaryBreak < (int)breaks.size() && temporaryTask < (int)tasks.size()) {
        if (breaks[temporaryBreak].startTime < tasks[temporaryTask].startTime1) {
            breaksAndTasks.push_back(make_pair(temporaryBreak, false));
            temporaryBreak++;
            if (temporaryBreak == (int)breaks.size())
                while (temporaryTask < (int)tasks.size()) {
                    breaksAndTasks.push_back(make_pair(temporaryTask, true));
                    temporaryTask++;
                }
        }
        else {
            breaksAndTasks.push_back(make_pair(temporaryTask, true));
            temporaryTask++;
            if (temporaryTask == (int)tasks.size())
                while (temporaryBreak < (int)breaks.size()) {
                    breaksAndTasks.push_back(make_pair(temporaryBreak, false));
                    temporaryBreak++;
                }
        }
    }
}

/*
    makes sure there are not unnecessary idle gaps between first operations of Tasks
*/
void Instance::pushTasksToLeft(vector <pair <int, bool> > breaksAndTasks, int startPoint) {
    for (int i=startPoint; i<(int)breaksAndTasks.size(); i++) {
        if (breaksAndTasks[i].second == true) {
            if (breaksAndTasks[i-1].second == true)
                tasks[breaksAndTasks[i].first].startTime1 = tasks[breaksAndTasks[i-1].first].startTime1 + tasks[breaksAndTasks[i-1].first].lengthTime1;
            else
                tasks[breaksAndTasks[i].first].startTime1 = breaks[breaksAndTasks[i-1].first].startTime + breaks[breaksAndTasks[i-1].first].lengthTime;
            if (tasks[breaksAndTasks[i].first].startTime1 < tasks[breaksAndTasks[i].first].readyTime1)
                tasks[breaksAndTasks[i].first].startTime1 = tasks[breaksAndTasks[i].first].readyTime1;
        }
    }
}

bool Instance::check() {
	for (int i=0; i<(int)tasks.size(); i++) {
		if (tasks[i].readyTime1 > tasks[i].startTime1) {
			cout << "readyTime1;";
			return false;
		}
		if (tasks[i].readyTime2 > tasks[i].startTime2) {
			cout << "readyTime2;";
			return false;
		}
	}
	return true;
}
