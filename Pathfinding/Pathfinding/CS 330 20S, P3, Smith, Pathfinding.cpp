/*
Project: CS330
Program: CS 330 20S, P3, Smith, Pathfinding.cpp
Purpose: Provide implementation of A* pathfinding algorithm.
Author: Jake Smith, jcs0046@uah.edu
Created: March 17, 2020
*/


#include <vector>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
//Define graphs.
//Graphs are a structure with 2 vectors of vectors, one of nodes and the other of vectors.

struct Node {
	int status;
	int costSoFar;
	float estimatedHeuristic;
	float estimatedTotal;
	int connectionBack;
	float locationX;
	float locationZ;
};
struct Connection {
	int fromNode;
	int toNode;
	int cost;
};
struct PathRequest {
	int fromNode;
	int toNode;
};
struct Graph {
	std::vector<Node> nodes;
	std::vector<Connection> connections;
	std::vector<PathRequest> pathRequests;
};
Node makeNode(float x, float z) {
	Node n;
	n.status = 0;
	n.costSoFar = 0;
	n.estimatedHeuristic = 0;
	n.estimatedTotal = 0;
	n.connectionBack = 0;
	n.locationX = x;
	n.locationZ = z;
	return n;
}

Connection makeConnection(int x, int y, int z) {
	Connection c;
	c.fromNode = x;
	c.toNode = y;
	c.cost = z;
	return c;
}

PathRequest makePathRequest(int x, int y) {
	PathRequest p;
	p.fromNode = x;
	p.toNode = y;
	return p;
}

//Initialize global pathfinding variables.
int UNVISITED = 0;
int OPEN = 1;
int CLOSED = 2;

bool trace = false;
std::string mapName = "Adventure Bay";
std::string traceFileName = "CS 330, Pathfinding trace " + mapName + ".txt";
std::string fileName = "CS 330 20S, P3, Smith, Results.txt";
std::ofstream traceFile;
std::ofstream outputFile;


Graph pullGraph(std::string fileName) {
	Graph g;
	std::ifstream file;
	file.open(fileName);
	if (!file) {
		std::cout << "Cannot find file.";
		return g;
	}
	std::string str;
	g.nodes.push_back({ NULL });
	while (std::getline(file, str)) {
		if (str.length() == 0){
		}
		else if (str[0] == 'N') {
			std::istringstream in(str);
			float x;
			float z;
			char sep;
			in >> sep >> x >> z;
			g.nodes.push_back(makeNode(x, z));
		}
		else if (str[0] == 'C') {
			std::istringstream in(str);
			int x;
			int y;
			int z;
			char sep;
			in >> sep >> x >> y >> z;
			g.connections.push_back(makeConnection(x, y, z));
		}
		else if (str[0] == 'R') {
			std::istringstream in(str);
			int x;
			int y;
			char sep;
			in >> sep >> x >> y;
			g.pathRequests.push_back(makePathRequest(x,y));
		}
	}
	return g;
}

//Find shortest path from first node to last node using A* algorithm.

int countNodes(Graph g, int status) {
	std::vector<int> nodeCounts{ 0,0,0 };
	for (int i = 1; i < g.nodes.size(); i++)
		nodeCounts[g.nodes[i].status]++;
	return nodeCounts[status];
}

int findSmallest(Graph g) {
	traceFile.open(traceFileName, std::ios_base::app);
	int smallestNode = NULL;
	float smallestCost = INFINITY;
	for (int i = 1; i < g.nodes.size(); i++) {
		if ((g.nodes[i].estimatedTotal < smallestCost)) {
			if (g.nodes[i].status == OPEN) {
				smallestNode = i;
				smallestCost = g.nodes[i].estimatedTotal;
			}
		}
	}
	traceFile.close();
	return smallestNode;
}

float heuristic(Graph g, int a, int b) {
	float aX = g.nodes[a].locationX;
	float aY = g.nodes[a].locationZ;
	float bX = g.nodes[b].locationX;
	float bY = g.nodes[b].locationZ;
	float x = pow(bX - aX, 2);
	float y = pow(bY - aY, 2);
	float distance = sqrt(x + y);
	return distance;
}

std::vector<int> getConnections(Graph g, int currentNode) {
	std::vector<int> currentConnections;
	for (int i = 0; i < g.connections.size(); i++) 
		if (g.connections[i].fromNode == currentNode)
			currentConnections.push_back(i);
	return currentConnections;
}

void showNodeStatus(Graph g, int iteration, int current) {
	int n = g.nodes.size();
	std::vector<char> fillOptions{ '.','O','X' };
	std::vector<char> fill;
	for (int i = 1; i < n; i++) {
		fill.push_back(fillOptions[g.nodes[i].status]);
	}
	if (current != NULL)
		fill[current] = 'C';
	traceFile.open(traceFileName, std::ios_base::app);
	std::cout << "Iteration: " << iteration << std::endl;
	traceFile << "Iteration: " << iteration << "\n";
	std::cout << "Unvisited nodes: " << countNodes(g, UNVISITED) << std::endl;
	traceFile << "Unvisited nodes: " << countNodes(g, UNVISITED) << "\n";
	std::cout << "Open nodes: " << countNodes(g, OPEN) << std::endl;
	traceFile << "Open nodes: " << countNodes(g, OPEN) << "\n";
	std::cout << "Closed nodes: " << countNodes(g, CLOSED) << std::endl;
	traceFile << "Closed nodes: " << countNodes(g, CLOSED) << "\n";
	for (int i = 0; i < fill.size(); i++) {
		std::cout << fill[i];
		traceFile << fill[i];
	}
	std::cout << std::endl;
	traceFile << "\n";
	traceFile.close();
}


Graph findPath(Graph g, int first, int last) {
	if (trace) {
		std::cout << "A* from " << first << " to " << last << std::endl;
		traceFile.open(traceFileName, std::ios_base::app);
		traceFile << "A* from " << first << " to " << last << std::endl;
		traceFile.close();
	}
	for (int i = 0; i < g.nodes.size(); i++) {
		g.nodes[i].status = UNVISITED;
		g.nodes[i].costSoFar = INT_MAX;
		g.nodes[i].connectionBack = NULL;
	}
	g.nodes[first].status = OPEN;
	g.nodes[first].costSoFar = 0;
	int iteration = 0;
	if (trace)
		showNodeStatus(g, iteration, NULL);
	int currentNode = findSmallest(g);

	while (((countNodes(g, OPEN)) > 0) && (currentNode != last)){
		iteration++;
		std::vector <int> currentConnections = getConnections(g, currentNode);
		for (int connection : currentConnections) {
			int toNode = g.connections[connection].toNode; ///////
			int toCost = g.nodes[currentNode].costSoFar + g.connections[connection].cost;
			if (toCost < g.nodes[toNode].costSoFar) {
				g.nodes[toNode].status = OPEN;
				g.nodes[toNode].costSoFar = toCost;
				g.nodes[toNode].estimatedHeuristic = heuristic(g, toNode, last);
				g.nodes[toNode].estimatedTotal = g.nodes[toNode].costSoFar + g.nodes[toNode].estimatedHeuristic;
				g.nodes[toNode].connectionBack = currentNode;
			}
		}
		if (trace)
			showNodeStatus(g, iteration, currentNode);
		g.nodes[currentNode].status = CLOSED;
		currentNode = findSmallest(g);
	}
	return g;
}

void printAllNodes(Graph g) {
	outputFile.open(fileName, std::ios_base::app);
	outputFile << "Output File for " << mapName << ":\n";
	for (int i = 1; i < g.nodes.size(); i++) {
		outputFile << "Node " << i << ": X:" << g.nodes[i].locationX << " Z:" << g.nodes[i].locationZ << "\n";
	}
	outputFile << "Connections:\n";
	for (int i = 0; i < g.connections.size(); i++)
		outputFile << "Connection " << (i+1) << ": " << g.connections[i].fromNode << " to " << g.connections[i].toNode << " Cost: " << g.connections[i].cost << "\n";
	outputFile << "Pathfinding Results:\n";
	outputFile.close();
}

std::vector<int> retrievePath(Graph g, int first, int last) {
	outputFile.open(fileName, std::ios_base::app);
	std::vector<int> path;
	int current = last;
	while ((current != first) && (current != NULL)) {
		path.push_back(current);
		current = g.nodes[current].connectionBack;
	}
	if (current == first) {
		path.push_back(first);
		std::reverse(path.begin(),path.end());
		if (trace) {
			traceFile.open(traceFileName, std::ios_base::app);
			std::cout << "Path from " << first << " to " << last << std::endl;
			traceFile << "Path from " << first << " to " << last << "\n";
			std::cout << "Path = ";
			traceFile << "Path = ";
			for (int i = 0; i < path.size(); i++) {
				std::cout << path[i] << " ";
				traceFile << path[i] << " ";
			}
			std::cout << std::endl << "Cost: " << g.nodes[last].costSoFar;
			traceFile << "\nCost: " << g.nodes[last].costSoFar;
			std::cout << std::endl;
			traceFile << "\n";
			traceFile.close();
		}
		outputFile << "Path from " << first << " to " << last << "\nPath = ";
		for (int i = 0; i < path.size(); i++) {
			outputFile << path[i] << " ";
		}
		outputFile << "\nCost: " << (g.nodes[last].costSoFar / 100) << "\n";
		outputFile.close();
		return path;
	}
	else {
		std::vector<int> empty{};
		if (trace) {
			traceFile.open(traceFileName, std::ios_base::app);
			std::cout << "Path from " << first << " to " << last << " not found." << std::endl;
			traceFile << "Path from " << first << " to " << last << " not found.\n";
			traceFile.close();
		}
		return empty;
	}
}

Graph makeAStarCompatible(Graph g) {
	for (int i = 0; i < g.connections.size(); i++) {
		g.connections[i].cost *= 100;
	}
	return g;
}

void findAllPaths(Graph g) {
	for (int i = 0; i < g.pathRequests.size(); i++) {
		g = findPath(g, g.pathRequests[i].fromNode, g.pathRequests[i].toNode);
		std::vector<int> path = retrievePath(g, g.pathRequests[i].fromNode, g.pathRequests[i].toNode);
	}
}



void printNodesToConsole(Graph g) {
	std::cout << "Nodes:\n";
	for (int i = 1; i < g.nodes.size(); i++) {
		if(i < 10)
			std::cout << "Node " << i << ":\t\tX:" << g.nodes[i].locationX << " Y:" << g.nodes[i].locationZ << std::endl;
		else
			std::cout << "Node " << i << ":\tX:" << g.nodes[i].locationX << " Y:" << g.nodes[i].locationZ << std::endl;
	}
	std::cout << "Connections:\n";
	for (int i = 0; i < g.connections.size(); i++) {
		std::cout << "Connection " << i << ":\t" << g.connections[i].fromNode << " to " << g.connections[i].toNode << " Cost: " << g.connections[i].cost << std::endl;
	}
	std::cout << "Path Requests:\n";
	for (int i = 0; i < g.pathRequests.size(); i++) {
		std::cout << "Request " << i << ":\t" << g.pathRequests[i].fromNode << " to " << g.pathRequests[i].toNode << std::endl;
	}
}

int main() {
	trace = false;
	std::string pullFileName = "CS 330 20S, Graph Adventure Bay v3.txt";
	Graph g = pullGraph(pullFileName);
	if (trace) {
		std::cout << "CS 330, Pathfinding, Begin, Map Name: " << mapName << std::endl;
		traceFile.open(traceFileName, std::ios_base::app);
		traceFile << "CS 330, Pathfinding, Begin, Map Name: " << mapName << "\n";
		traceFile.close();
	}
	printAllNodes(g);
	g = makeAStarCompatible(g);
	findAllPaths(g);
//	system("pause");
}

