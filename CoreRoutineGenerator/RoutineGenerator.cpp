// push up sphinx/hindu never picked?

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

ifstream exercise_bank_read;

ifstream past_routines_file_read;
ofstream past_routines_file_write;

ifstream new_routine_file_read;
ofstream new_routine_file_write;

vector<std::string> getPreviousRoutine()
{
	new_routine_file_read.open("newRoutine.txt");
	if (!new_routine_file_read.is_open())
	{
		std::cout << "newRoutine.txt was not found";
		return {};
	}

	std::string lineRead;
	vector<std::string> previousRoutine;
	getline(new_routine_file_read, lineRead);
	bool newRoutineFilled = !(lineRead == "");

	if (newRoutineFilled)
	{
		do
		{
			if (lineRead == "Routine for today:" || lineRead == "")
				continue;

			previousRoutine.push_back(lineRead);
		} while (getline(new_routine_file_read, lineRead));
		new_routine_file_read.close();
	}
	else
	{
		new_routine_file_read.close();

		past_routines_file_read.open("pastRoutines.txt");
		if (!past_routines_file_read.is_open())
		{
			std::cout << "pastRoutines.txt was not found\n";
			return {};
		}

		while (getline(past_routines_file_read, lineRead))
		{
			if (lineRead == "Previous Routine:")
				continue;

			if (lineRead == "Past Routine:")
				break;

			bool isExercise = (lineRead != "");
			if (isExercise)
				previousRoutine.push_back(lineRead);
		}
		past_routines_file_read.close();
	}

	return previousRoutine;
}

vector<std::string> getRoutineToMove()
{
	new_routine_file_read.open("newRoutine.txt");
	if (!new_routine_file_read.is_open())
	{
		std::cout << "newRoutine.txt was not found\n";
		return {};
	}

	std::string lineRead;
	vector<std::string> routineToMove;
	while (getline(new_routine_file_read, lineRead))
	{
		(lineRead == "Routine for today:") ?
			routineToMove.push_back("Previous Routine:") :
			routineToMove.push_back(lineRead);
	}
	new_routine_file_read.close();

	return routineToMove;
}

vector<std::string> getPastRoutines()
{
	past_routines_file_read.open("pastRoutines.txt");
	if (!past_routines_file_read.is_open())
	{
		std::cout << "pastRoutines.txt was not found\n";
		return {};
	}

	vector<std::string> pastRoutines;
	std::string lineRead;
	while (getline(past_routines_file_read, lineRead))
	{
		pastRoutines.push_back(lineRead);
	}
	past_routines_file_read.close();

	return pastRoutines;
}

// overwrites pastRoutines.txt with routineToMove and appends pastRoutines
void overwritePastRoutines(vector<std::string> routineToMove, vector<std::string> pastRoutines)
{
	// open() clears the file
	past_routines_file_write.open("pastRoutines.txt");
	if (!past_routines_file_write.is_open())
	{
		std::cout << "pastRoutines.txt was not found\n";
		return;
	}

	for (std::string line : routineToMove)
	{
		past_routines_file_write << line << "\n";
	}
	past_routines_file_write << "\n";

	for (std::string line : pastRoutines)
	{
		(line == "Previous Routine:") ?
			past_routines_file_write << "Past Routine:\n" :
			past_routines_file_write << line << "\n";
	}

	past_routines_file_write.close();
}

void writeNewRoutine(vector<std::string> routine)
{
	new_routine_file_write.open("newRoutine.txt");
	if (!new_routine_file_write.is_open())
	{
		std::cout << "newRoutine.txt was not found\n";
		return;
	}

	new_routine_file_write << "Routine for today:\n\n";
	for (int j = 0; j < routine.size(); j++)
	{
		new_routine_file_write << routine[j] << "\n";
	}
	new_routine_file_write.close();

	std::cout << "See newRoutine.txt\n";
}

// overwrites newRoutine.txt and pastRoutines.txt
void overwriteFiles(vector<std::string> routine)
{
	vector<std::string> routineToMove = getRoutineToMove();
	vector<std::string> pastRoutines = getPastRoutines();
	// don't change past routines if newRoutine file is empty		
	if (routineToMove.size() > 0)
		overwritePastRoutines(routineToMove, pastRoutines);

	writeNewRoutine(routine);
}

std::string getCategory(vector<std::string> diffList, int index)
{
	std::string exercise = diffList[index];
	// exercise category is first word of the exercise
	std::string category = exercise.substr(0, exercise.find(" "));
	return category;
}

bool contains(vector<std::string> vec, std::string str)
{
	return (std::find(vec.begin(), vec.end(), str) != vec.end());
}

bool hasNewCategory(vector<std::string> diffList, vector<std::string> categoriesUsed)
{
	for (int i = 0; i < diffList.size(); i++)
	{
		if (!contains(categoriesUsed, getCategory(diffList, i)))
			return true;
	}
	return false;
}

template<typename T>
void remove(vector<T>* vec, int index)
{
	vector<T> newVec;
	for (int i = 0; i < (*vec).size(); i++)
	{
		if (i == index)
			continue;
		else
			newVec.push_back((*vec)[i]);
	}
	(*vec) = newVec;
}

// remove difficulties past the max difficulty desired
void removeDifficulties(vector<int> *ptrValidListIndexes, int maxDiff)
{
	// size changes as you delete!
	for (int diffIndex = maxDiff; diffIndex < (*ptrValidListIndexes).size(); diffIndex++)
	{
		remove(ptrValidListIndexes, diffIndex);		
		// decrement difficulty index as list changes size so the loop doesn't end prematurely
		diffIndex--;
	}
}

// sometimes stops short of high numExercises inputs,
// especially if previous routine also had many exercises
vector<std::string> generateRoutine(vector<vector<std::string>> diffLists, int numExercises, char uniq, int maxDiff)
{
	srand(time(0));

	int numExercisesToPick = numExercises;

	vector<std::string> routine;
	vector<std::string>* diffList;
	vector<std::string> categoriesUsed;
	vector<std::string> previousRoutine = getPreviousRoutine();

	vector<int> validListIndexes = { 0, 1, 2, 3 };
	vector<int>* ptrValidListIndexes = &validListIndexes;
	
	removeDifficulties(ptrValidListIndexes, maxDiff);
	bool useUniqueExercises = (uniq == 'y' || uniq == 'Y');

	while (numExercisesToPick > 0 && validListIndexes.size() > 0)
	{
		int listIndex = numExercisesToPick % validListIndexes.size();
		diffList = &diffLists[listIndex];

		bool isNewCategoryInList = hasNewCategory((*diffList), categoriesUsed);
		bool isListEmpty = (diffList->size() == 0);

		if (!isNewCategoryInList || isListEmpty)
		{
			remove(ptrValidListIndexes, listIndex);
			continue;
		}

		int randListLine = rand() % diffList->size();
		std::string exercise = (*diffList)[randListLine];
		std::string category = getCategory((*diffList), randListLine);

		if (contains(categoriesUsed, category))
		{
			remove(diffList, randListLine);
			continue;
		}

		if (useUniqueExercises && contains(previousRoutine, exercise))
		{
			remove(diffList, randListLine);
			continue;
		}

		routine.push_back(exercise);
		categoriesUsed.push_back(category);
		remove(diffList, randListLine);
		numExercisesToPick -= 1;
	}
	return routine;
}

void printExercises(vector<vector<std::string>> diffVecs)
{
	// iterate through each of the 4 difficulty vectors
	for (int i = 0; i < diffVecs.size(); i++)
	{
		// print each exercise of the difficulty vector
		for (int j = 0; j < diffVecs[i].size(); j++)
		{
			std::cout << diffVecs[i][j] << "\n";
		}
	}
}

vector< pair<vector<std::string>, std::string> > associateDiff(vector< vector<std::string> > diffVecs)
{
	vector< pair<vector<std::string>, std::string> > result(4);
	std::string diffTerms[] = { "Easy", "Intermediate", "Advanced", "Expert" };

	for (int i = 0; i < diffVecs.size(); i++)
	{
		result[i].first = diffVecs[i];
		result[i].second = diffTerms[i];
	}
	return result;
}

vector<vector<std::string>> fragmentList(vector<std::string> exerciseList)
{
	vector<std::string> easyExercises;
	vector<std::string> intmExercises;
	vector<std::string> advExercises;
	vector<std::string> expExercises;

	vector< vector<std::string> > diffLists = { easyExercises, intmExercises, advExercises, expExercises };
	// each difficulty vector is paired with a difficulty std::string for identification
	vector< pair<vector<std::string>, std::string> > diffListPairs = associateDiff(diffLists);

	std::string lineRead;
	std::string nextDiff;
	// start at 1 to skip the "Easy" difficulty line
	int curExercise = 1;
	for (int i = 0; i < diffLists.size(); i++)
	{
		(i == diffLists.size() - 1) ? nextDiff = "" : nextDiff = diffListPairs[i + 1].second;
		for (int j = curExercise; j < exerciseList.size(); j++)
		{
			lineRead = exerciseList[j];
			if (lineRead == nextDiff)
			{
				curExercise += 1;
				break;
			}
			diffLists[i].push_back(lineRead);
			curExercise += 1;
		}
	}

	return diffLists;
}

vector<std::string> fillExerciseList(vector<std::string> list)
{
	std::string lineRead;
	exercise_bank_read.open("exerciseBank.txt");
	if (!exercise_bank_read.is_open())
	{
		std::cout << "exerciseBank.txt was not found\n";
		return {};
	}

	while (getline(exercise_bank_read, lineRead))
	{
		if (lineRead != "")
			list.push_back(lineRead);
	}
	exercise_bank_read.close();

	return list;
}

void main()
{
	vector<std::string> exerciseList;

	exerciseList = fillExerciseList(exerciseList);

	vector< vector<std::string> > diffLists = fragmentList(exerciseList);

	int numExercises;
	int maxDiff = 0;
	char useUniqueExercises;
	
	std::cout << "Enter the number of desired exercises in routine: ";
	// could check for invalid input later
	cin >> numExercises;

	std::cout << "What is the max difficulty you want?\n1. Easy, 2. Intermediate, 3. Advanced, 4. Expert: ";
	cin >> maxDiff;

	std::cout << "Would you like exercises unique from previous rountine (y/n): ";	
	cin >> useUniqueExercises;

	vector<std::string> routine = generateRoutine(diffLists, numExercises, useUniqueExercises, maxDiff);
	overwriteFiles(routine);
}