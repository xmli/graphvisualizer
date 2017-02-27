/* Xianming (Sean) Li --Already sent email with assignment before, just resubmitting it
 * CS106L Autumn 2015
 */
#include <iostream>
#include "SimpleGraph.h"
#include <sstream>
#include <fstream>
#include <cmath>

using namespace std;

void Welcome();
void readFile(SimpleGraph &graph);
void initNodes(SimpleGraph &graph, ifstream &input);
void initNodePositions(SimpleGraph &graph, int numNodes);
void initEdges(SimpleGraph &graph, ifstream &input);
void runAlgorithm(SimpleGraph &graph);
void repulsiveForces(SimpleGraph &graph, vector<pair<double,double>> &netDeltas);
void attractiveForces(SimpleGraph &graph, vector<pair<double, double> > &netDeltas);
void moveGraph(SimpleGraph &graph, vector<pair<double, double> > &netDeltas);
int promptTime();
string getLine();

const double REPEL_CONSTANT = 0.01;
const double ATTRACT_CONSTANT = 0.01;
const double PI = 3.1415926535897932384626433832795;

// Main method
int main() {
    Welcome();
    bool graphAgain = true;
    while (graphAgain) {
        SimpleGraph graph;
        readFile(graph);
        InitGraphVisualizer(graph);
        DrawGraph(graph); // draws initial set-up

        double secs = promptTime();
        time_t startTime = time(NULL);
        double elapsedTime = difftime(time(NULL), startTime);
        // Runs program for specified # of seconds
        while (elapsedTime < secs) {
            runAlgorithm(graph);
            DrawGraph(graph);
            if (elapsedTime > secs) break;
            elapsedTime = difftime(time(NULL), startTime);
        }
        cout << "Would you like to draw another graph (Y/N)? ";
        string response = getLine();
        if(response != "Y" && response != "y"){
            graphAgain = false;
        }
    }
    cout << "Thank you for using GraphViz! Goodbye." << endl;
    return 0;
}

/* Prints a message to the console welcoming the user and
 * describing the program. */
void Welcome() {
    cout << "Welcome to CS106L GraphViz!" << endl;
    cout << "This program uses a force-directed graph layout algorithm" << endl;
    cout << "to render sleek, snazzy pictures of various graphs." << endl;
    cout << endl;
}

/* Reads in a file name, checking for validity. Once a valid file
 * name is read, initializes the nodes and edges of the graph
 */
void readFile(SimpleGraph &graph) {
    bool fileSuccess = false;
    ifstream input;
    while(!fileSuccess) {
        cout << "Please enter a file name: ";
        string fileName = getLine();
        input.open(fileName.c_str());
        if (!input.is_open()) {
            cerr << "Cannot open specified file." << endl;
        } else {
            fileSuccess = true;
        }
    }
    initNodes(graph, input);
    initEdges(graph, input);
}

/* Initializes the nodes of a given graph */
void initNodes(SimpleGraph &graph, ifstream &input) {
    int numNodes;
    input >> numNodes >> ws;
    initNodePositions(graph, numNodes);
}

/* Initializes the positions of each node in a circle */
void initNodePositions(SimpleGraph &graph, int numNodes) {
    vector<Node> nodes(numNodes);
    for(int i = 0; i < numNodes; ++i) {
        nodes[i].x = cos(2*PI*i/numNodes);
        nodes[i].y = sin(2*PI*i/numNodes);
    }
    graph.nodes = nodes;
}

/* Reads in lines from the file and initializes the
 * edges of the graph */
void initEdges(SimpleGraph &graph, ifstream &input) {
    while(true) {
        string line;
        getline(input, line);
        if (input.fail()) break;
        int start, end;
        stringstream inputStringStream;
        inputStringStream << line;
        inputStringStream >> start >> end;
        Edge edge;
        edge.start = start;
        edge.end = end;
        graph.edges.push_back(edge);
    }
}

/* Wrapper function that sums up the movements from repulsive
 * and attractive forces and moves the graph accordingly */
void runAlgorithm(SimpleGraph &graph) {
    vector<pair<double,double>> netDeltas(graph.nodes.size());
    repulsiveForces(graph, netDeltas);
    attractiveForces(graph, netDeltas);
    moveGraph(graph, netDeltas);
}

/* Calculates the repulsive forces for each iteration of the
 * algorithm */
void repulsiveForces(SimpleGraph &graph, vector<pair<double,double>> &netDeltas) {
    for(int i = 0; i < graph.nodes.size(); ++i) {
        for(int j = i+1; j < graph.nodes.size(); ++j) {
            Node n1 = graph.nodes[i];
            Node n2 = graph.nodes[j];
            double repelForce = REPEL_CONSTANT / sqrt((n2.x-n1.x)*(n2.x-n1.x) + (n2.y-n1.y)*(n2.y-n1.y));
            double theta = atan2(n2.y-n1.y, n2.x-n1.x);
            double deltaX = repelForce*cos(theta);
            double deltaY = repelForce*sin(theta);
            netDeltas[i].first -= deltaX;
            netDeltas[i].second -= deltaY;
            netDeltas[j].first += deltaX;
            netDeltas[j].second += deltaY;
        }
    }
}

/* Calculates the attractive forces for each iteration of the
 * algorithm */
void attractiveForces(SimpleGraph &graph, vector<pair<double,double>> &netDeltas) {
    for(int i = 0; i < graph.edges.size(); ++i) {
        Edge edge = graph.edges[i];
        Node n1 = graph.nodes[edge.start];
        Node n2 = graph.nodes[edge.end];
        double attractForce = ATTRACT_CONSTANT * ((n2.x-n1.x)*(n2.x-n1.x) + (n2.y-n1.y)*(n2.y-n1.y));
        double theta = atan2(n2.y-n1.y, n2.x-n1.x);
        double deltaX = attractForce*cos(theta);
        double deltaY = attractForce*sin(theta);
        netDeltas[edge.start].first += deltaX;
        netDeltas[edge.start].second += deltaY;
        netDeltas[edge.end].first -= deltaX;
        netDeltas[edge.end].second -= deltaY;
    }
}

/* Calculates the net movements in both x- and y- directions
 * for each node per iteration */
void moveGraph(SimpleGraph &graph, vector<pair<double, double> > &netDeltas) {
    for(int i = 0; i < graph.nodes.size(); ++i) {
        graph.nodes[i].x += netDeltas[i].first;
        graph.nodes[i].y += netDeltas[i].second;
    }
}

/* Prompts the user for a specified number of seconds to run the program */
int promptTime() {
    int value;
    string line;
    while (true) {
        cout << "Enter the number of seconds to run the algorithm: ";
        getline(cin, line);
        istringstream stream(line);
        stream >> value >> ws;
        if (!stream.fail() && stream.eof()) break;
        cout << "Illegal integer format. Try again." << endl;
    }
    return value;
}

/* Wrapper function for reading a line from the user */
string getLine() {
    string line;
    getline(cin, line);
    return line;
}
