#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <time.h>
#include <string>
#include <fstream>

#include "AI.h"

std::vector<double> runGame(std::vector<AI>&);

template<typename T>
void print(std::vector<T>&);

std::vector<double> evaluateInstance(std::vector<AI>&);

void saveAIBlocks(AI&);
void readAIBlocks(AI&);

void parseInp1Space(std::string&, double&, double&);
void parseInp2Space(std::string, double&, double&, double&);

double getRandom(double, double);

void run();
void setup();
void initGLFB(); 
void ending(); 

std::vector<double> runPong(std::vector<AI>&);

#define XSTR(x) STR(x)
#define STR(x) #x

#define MARK 1000

#define DATA_SIZE 200
#define KI_ANZAHL 1
#define AI_LAYOUT 6, 6, 5, 5, 3

std::ofstream textFileWrite; 
std::ifstream textFileRead;

std::normal_distribution<double> dis(0.0, 1.0);
std::mt19937 gen;


std::vector<double> score(KI_ANZAHL);
std::vector<AI> nnL;


int generationCounter = 0;
int pos = 0;
double max = score[0];


int main() {

	//runPong(); 

	//initGLFB(); 

	run(); 

	//ending(); 

	return 0; 
}

void run() {

	std::cout << "Initializing..." << std::endl;

	setup();

	std::cout << "Trining the AI..." << "\n\n" << std::endl;

	// Fill AI List
	nnL.push_back(AI({ AI_LAYOUT })); 

	readAIBlocks(nnL[0]);

	for (int i = 1; i < KI_ANZAHL; i++) {
		nnL.push_back(AI({ AI_LAYOUT }));

		for (int i2 = 0; i2 < nnL[i].weights.size(); i2++) {
			for (int i3 = 0; i3 < nnL[i].weights[i2].size(); i3++) {
				nnL[i].weights[i2][i3] = nnL[0].weights[i2][i3] + dis(gen);
			}
		}

		for (int i2 = 0; i2 < nnL[i].biases.size(); i2++) {
			for (int i3 = 0; i3 < nnL[i].biases[i2].size(); i3++) {
				nnL[i].biases[i2][i3] = nnL[0].biases[i2][i3] + dis(gen);
			}
		}

	}

	// Train
	while (max < MARK) {

		generationCounter++;
		
		std::fill(score.begin(), score.end(), 0);  // reset score to 0


		std::vector<double> tempScores = evaluateInstance(nnL); // evaluate AI instances

		for (int i = 0; i < tempScores.size(); i++) {
			score[i] = tempScores[i]; 
		}


		// find best ai instance
		pos = 0; 
		max = 0; 
		for (int i = 0; i < score.size(); i++) {
			if (score[i] > max) {
				max = score[i];
				pos = i;
			}
		}

		/////////////////////////////////////////////////////////////////////

		
		AI temp = nnL[pos];

		nnL.clear();

		nnL.push_back(temp);

		for (int i = 1; i < KI_ANZAHL; i++) {
			nnL.push_back(AI({ AI_LAYOUT }));

			for (int i2 = 0; i2 < nnL[i].weights.size(); i2++) {
				for (int i3 = 0; i3 < nnL[i].weights[i2].size(); i3++) {
					nnL[i].weights[i2][i3] = nnL[0].weights[i2][i3] + dis(gen);
				}
			}

			for (int i2 = 0; i2 < nnL[i].biases.size(); i2++) {
				for (int i3 = 0; i3 < nnL[i].biases[i2].size(); i3++) {
					nnL[i].biases[i2][i3] = nnL[0].biases[i2][i3] + dis(gen);
				}
			}

		}

		std::cout << "Current max: " << max << " in Generation " << generationCounter << std::endl;

	}

	std::cout << std::endl;
	std::cout << "generationCounter: " << generationCounter << std::endl;
	std::cout << "Max: " << max << std::endl << std::endl;

	std::string s = "";

	std::cout << "Save best AI Blocks? y/n: ";
	std::getline(std::cin, s);

	if (s == "y") {
		saveAIBlocks(nnL[0]);
		std::cout << "Saved AI" << std::endl; 
	}
	else {
		std::cout << "Did not save the AI. Are you sure? Save? y/n: " << std::endl; 
		std::getline(std::cin, s);
		if (s == "y") {
			saveAIBlocks(nnL[0]);
			std::cout << "Saved AI" << std::endl;
		}
		else {
			std::cout << "Program terminated. AI was not saved!" << std::endl; 
		}
	}

}

std::vector<double> evaluateInstance(std::vector<AI>& aiList) {

	return runPong(aiList);
}


template<typename T>
void print(std::vector<T>& vec) {


	for (T& i : vec) {
		std::cout << i << " ";
	}

	std::cout << std::endl << std::endl;
}

double getRandom(double min, double max) {
	return min + ((double)rand() / RAND_MAX) * (max - min);
}

void parseInp1Space(std::string& s, double& inp1, double& inp2) {
	for (int i = 0; i < s.size(); i++) {
		if (s[i] == ' ') {

			inp1 = std::stod(s.substr(0, i));
			inp2 = std::stod(s.substr(i + 1));

			return;
		}
	}

}

void parseInp2Space(std::string s, double& inp1, double& inp2, double& inp3) {

	int pos1 = 0;

	for (int i = 0; i < s.size(); i++) {
		if (s[i] == ' ' && pos1 == 0) {
			pos1 = i;
		}
		else {
			if (s[i] == ' ' && pos1 != 0) {
				inp1 = std::stod(s.substr(0, pos1));
				inp2 = std::stod(s.substr(pos1 + 1, (pos1 + 1) - i));
				inp3 = std::stod(s.substr(i + 1));
				return;
			}
		}
	}

}

void saveAIBlocks(AI& ai) {

	textFileWrite.open("AI_Blocks.txt", std::ofstream::trunc);

	textFileWrite << XSTR(AI_LAYOUT) << "\n";

	for (int i = 0; i < ai.weights.size(); i++) {
		for (int i2 = 0; i2 < ai.weights[i].size(); i2++) {
			textFileWrite << i << " " << i2 << " " << ai.weights[i][i2] << "\n";
		}
	}

	textFileWrite << "-\n";


	for (int i = 0; i < ai.biases.size(); i++) {
		for (int i2 = 0; i2 < ai.biases[i].size(); i2++) {
			textFileWrite << i << " " << i2 << " " << ai.biases[i][i2] << "\n";
		}
	}

	textFileWrite.close();
}

void readAIBlocks(AI& ai) {
	textFileRead.open("AI_BLOCKS.txt");

	if (!textFileRead.is_open()) {
		std::cout << "Could not open th File to read from!\n" << std::endl;
		return;
	}

	std::string s;
	double i, i2, w;

	bool weights = true;

	getline(textFileRead, s);

	if (s != XSTR(AI_LAYOUT)) {
		std::cout << "AI Layouts do not match!\n" << std::endl;
		return;
	}

	while (getline(textFileRead, s)) {

		if (s != "-") {

			parseInp2Space(s, i, i2, w);

			if (weights) {
				ai.weights[i][i2] = w;
			}
			else {
				ai.biases[i][i2] = w;
			}
		}
		else {
			weights = false;
		}
	}


	textFileRead.close();

	std::cout << "Loaded AI successfully!\n" << std::endl;
}

void setup() {
	srand(time(NULL));

	gen = std::mt19937(rand() % 1000);
}

/*

Lernmethode verbessern:
	Die besten paar von jeder Generation als Leitbild für die nächste nehmen
	(dann wird hoffentlich nicht mehr auf eine halbgute lösung gesetzt)

training in die AI klasse implementieren
*/

/*
- player y pos
- pipe x pos
- pipe y1 pos
- player vel[y]
*/