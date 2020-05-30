#include "Graph.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <limits>

Graph::Graph() {

}


Graph::~Graph() {
	if (nodes_) {
		for (int i = 0; i < nodes_->getLength(); i++) {
			delete[] matrice_[i];
			delete[] availableConductivity_[i];
		}
		
		delete[] matrice_;
		delete[] availableConductivity_;
		delete nodes_;
	}
}


void Graph::input (std::string inFile) {
	std::fstream in (inFile, std::ios::in);
	char tempName;
	char afterChar;
	
	int stage = 0; // Goes from 0 to 2 as there are 3 fields in the presumed input file.
	int position[2]; // Just here to hold node1 and node2.
	int number; // Is here to hold conductivity.
	
	if (in.good()) {
		nodes_ = new NodeList;
		
		while (!in.eof()) { // Just counting number of cities and filling the list of them.
			if (stage < 2) {
				in >> tempName;
				afterChar = in.get();
				
				if (afterChar == ' ') {
					nodes_->addUnique (tempName);
					
					if (tempName == 'S' && start_ < 0) {
						start_ = nodes_->find ('S');
					}
					
					if (tempName == 'T' && terminator_ < 0) {
						terminator_ = nodes_->find ('T');
					}
					
					stage++;
				} else {
					throw std::invalid_argument ("Graph::input - Provided file is incorrect!");
					break;
				}
			} else {
				while (in.get() != '\n' && !in.eof()) { // There still might be errors missed
				
				} // But I'll ignore that. For now.
				
				stage = 0;
			}
		}
		
		in.close();
		
		// Matrices initial setup begin!
		matriceInit();
		// Matrices initial setup end!
		
		in.open (inFile, std::ios::in);
		
		while (!in.eof()) { // Now filling in the matrice.
			if (stage < 2) { // Which nodes those are?
				in >> tempName;
				afterChar = in.get();
				
				if (afterChar == ' ') {
					position[stage] = nodes_->find (tempName);
					stage++;
				} else {
					throw std::invalid_argument ("Graph::input - Provided file is incorrect!");
					break;
				}
			} else { // And now to read the number and input it into the matrice.
				in >> number;
				afterChar = in.get();
				
				if (number >= 0 || afterChar != '\n') {
					matrice_[position[0]][position[1]] = number;
					stage = 0;
				} else {
					throw std::invalid_argument ("Graph::input - Provided file is incorrect!");
					break;
				}
			}
		}
		
		in.close();
		
		for (int i = 0; i < nodes_->getLength(); i++)
			for (int j = 0; j < nodes_->getLength(); j++)
				availableConductivity_[i][j] = matrice_[i][j];
	} else {
		throw std::invalid_argument ("Graph::input - Provided file is incorrect!");
	}
}


void Graph::checkMatrice() {
	for (int i = 0; i < nodes_->getLength(); i++) {
		for (int j = 0; j < nodes_->getLength(); j++) {
			if (matrice_[i][j] == INT_MAX) std::cout << "S ";
			else std::cout << matrice_[i][j] << ' ';
		}
		
		std::cout << "\n";
	}
}


void Graph::displayNodes() {
	nodes_->display();
}


void Graph::matriceInit() {
	if (matrice_ || availableConductivity_) {
		for (int i = 0; i < nodes_->getLength(); i++) {
			delete[] matrice_[i];
			delete[] availableConductivity_[i];
		}
		
		delete [] matrice_;
		delete [] availableConductivity_;
	}
	
	matrice_ = new int*[nodes_->getLength()];
	availableConductivity_ = new int*[nodes_->getLength()];
	
	for (int i = 0; i < nodes_->getLength(); i++) {
		matrice_[i] = new int[nodes_->getLength()];
		availableConductivity_[i] = new int[nodes_->getLength()];
	}
	
	for (int i = 0; i < nodes_->getLength(); i++)
		for (int j = 0; j < nodes_->getLength(); j++)
			matrice_[i][j] = 0;
			
	for (int i = 0; i < nodes_->getLength(); i++)
		matrice_[i][i] = 0;
}


Graph::IteratorByWidth* Graph::initEdmondsKarpIterator() {
	return new Graph::IteratorByWidth (nodes_->getLength(), availableConductivity_, start_);
}


int Graph::solve() {

}

//
// FROM HERE ON LIES ITERATOR STUFF
//

Graph::IteratorByWidth::IteratorByWidth (int length, int** routes, int start) {
	routes_ = routes;
	queue_ = new int[length];
	prev_ = new int[length];
	visited_ = new bool[length];
	
	length_ = length;
	
	
	for (int i = 0; i < length; i++) {
		queue_[i] = prev_[i] = -1;
		visited_[i] = false;
	}
	
	queue_[0] = start;
}


int* Graph::IteratorByWidth::getCurrentRoutes() {
	int *returnArray = new int[length_];
	
	for (int i = 0; i < length_; i++)
		returnArray[i] = prev_[i];
		
	return returnArray;
}

int Graph::IteratorByWidth::step() {
	current_++;
	
	while (current_ < length_ && queue_[queueCurrent_] != -1) {
		if (routes_[queue_[queueCurrent_]][current_] == 0)
			current_++;
		else {
			if (visited_[current_]) current_++;
			else {
				visited_[current_] = true;
				queue_[queueLength_++] = current_;
				prev_[current_] = queue_[queueCurrent_];
				return current_;
			}
		}
		
		if (current_ == length_) {
			queueCurrent_++;
			current_ = 0;
		}
	}
	
	return -1;
}