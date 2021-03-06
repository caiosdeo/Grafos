#include "Graph.h"
#include "Node.h"
#include "Edge.h"
#include <iostream>
#include <fstream>
#include <stack>
#include <queue>
#include <list>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <float.h>
#include <iomanip>

using namespace std;

/**************************************************************************************************
 * Defining the Graph's methods
**************************************************************************************************/

// Constructor
Graph::Graph(int order, bool directed, bool weighted_edge, bool weighted_node)
{

    this->order = order;
    this->directed = directed;
    this->weighted_edge = weighted_edge;
    this->weighted_node = weighted_node;
    this->first_node = this->last_node = nullptr;
    this->number_edges = 0;
}

// Destructor
Graph::~Graph()
{

    Node *next_node = this->first_node;

    while (next_node != nullptr)
    {

        next_node->removeEdges();
        Node *aux_node = next_node->getNextNode();
        delete next_node;
        next_node = aux_node;
    }
}

// Getters
int Graph::getOrder()
{

    return this->order;
}
int Graph::getNumberEdges()
{

    return this->number_edges;
}

bool Graph::getDirected()
{

    return this->directed;
}

bool Graph::getWeightedEdge()
{

    return this->weighted_edge;
}

bool Graph::getWeightedNode()
{

    return this->weighted_node;
}

Node *Graph::getFirstNode()
{

    return this->first_node;
}

Node *Graph::getLastNode()
{

    return this->last_node;
}

// Other methods
/*
    The outdegree attribute of nodes is used as a counter for the number of edges in the graph.
    This allows the correct updating of the numbers of edges in the graph being directed or not.
*/
void Graph::insertNode(int id)
{
    // Verifies if the graph has at least one node
    if (this->first_node != nullptr)
    {

        if (!this->searchNode(id))
        {
            // Allocating the new node and setting it as the last node
            Node *new_node = new Node(id);
            this->last_node->setNextNode(new_node);
            this->last_node = new_node;
        }
    }
    else
    {
        // Setting the first and the last nodes with the new node
        this->first_node = this->last_node = new Node(id);
    }
}

void Graph::makeGraph(int id, int target_id, float weight)
{

    Node *source_node;
    Node *target_node;
    // Searching for the nodes in the graph
    bool source = this->searchNode(id);
    bool target = this->searchNode(target_id);

    // Verifies whether the source and the target nodes are in the graph
    if (source && target)
    {
        // Getting the source and target nodes
        source_node = this->getNode(id);
        target_node = this->getNode(target_id);
    }
    // Verifies whether the source and the target nodes are in the graph
    else if (source || target)
    {
        // Verifies whether the source node is in the graph
        if (source)
        {
            // Getting the source node and allocating the target node
            source_node = this->getNode(id);
            this->insertNode(target_id);
            target_node = this->last_node;
        }
        // Verifies whether the target node is in the graph
        if (target)
        {
            // Getting the target node and allocating the source node
            target_node = this->getNode(target_id);
            this->insertNode(id);
            source_node = this->last_node;
        }
    }
    // Occurs when the source and target nodes are not in the graph
    else
    {
        // Allocating both nodes - source and target
        this->insertNode(id);
        source_node = this->last_node;
        this->insertNode(target_id);
        target_node = this->last_node;
    }
    // Verifies whether the graph is directed
    if (this->directed)
    {
        // Inserting an edge of the source node for the target node
        source_node->insertEdge(target_id, weight);
        source_node->incrementOutDegree();
        target_node->incrementInDegree();
    }
    else
    {
        // Inserting an edge in both nodes
        source_node->insertEdge(target_id, weight);
        target_node->insertEdge(id, weight);
        source_node->incrementOutDegree();
        target_node->incrementOutDegree();
    }
    // Incrementing the number of edges in the graph
    this->number_edges++;
}

void Graph::removeNode(int id)
{
    // Verifies whether there are at least one node in the graph
    if (this->first_node != nullptr)
    {
        // Verifies whether the node to remove is in the graph
        if (this->searchNode(id))
        {
            // Necessary attributes
            int count_edges_removed = 0;
            Node *aux_node = this->first_node;
            Node *previous_node = nullptr;
            Node *target_node = nullptr;
            // Searching for the node to be removed
            while (aux_node->getId() != id)
            {

                previous_node = aux_node;
                aux_node = aux_node->getNextNode();
            }
            // Keeping the integrity of the node list
            previous_node->setNextNode(aux_node->getNextNode());
            // Getting the first edge of the node to be removed
            Edge *aux_edge = aux_node->getFirstEdge();
            // Traversing the edges of the node to be removed
            while (aux_edge != nullptr)
            {
                // Getting the node in the edges of the node to be removed
                target_node = this->getNode(aux_edge->getTargetId());
                // Counting the number of edges removed and removing them if there are any
                count_edges_removed += target_node->removeEdge(id, this->directed, target_node);
                // Getting the next edge of the node to be removed
                aux_edge = aux_edge->getNextEdge();
            }

            // Incrementing the number of edges removed of the graph with the outdegree of the node to be removed
            count_edges_removed += aux_node->getOutDegree();
            // Removing all edges of the node to be removed
            aux_node->removeEdges();
            // Keeping the integrity of the node list
            if (this->last_node = aux_node)
                this->last_node = previous_node;

            previous_node->setNextNode(aux_node->getNextNode());

            delete aux_node;
            // Verifies whether the graph is directed
            if (this->directed)
            {

                aux_node = this->first_node;
                // Visiting all nodes in the list of nodes
                while (aux_node != nullptr)
                {
                    // Counting the number of edges removed and removing them if there are any
                    count_edges_removed += aux_node->removeEdge(id, this->directed, aux_node);
                    aux_node = aux_node->getNextNode();
                }
            }
            else
                count_edges_removed /= 2;
            // Decrementing the number of edges in the graph
            this->number_edges -= count_edges_removed;
        }
        // Decrementing the order of the graph
        this->order--;
    }
}

bool Graph::searchNode(int id)
{
    // Verifies whether there are at least one node in the graph
    if (this->first_node != nullptr)
    {
        // Searching for a specific node of id equal to id
        for (Node *aux = this->first_node; aux != nullptr; aux = aux->getNextNode())
            if (aux->getId() == id)
                return true;
    }

    return false;
}

Node *Graph::getNode(int id)
{

    Node *aux = first_node;

    while (aux != NULL)
    {

        if (aux->getId() == id)
        {
            return aux;
        }

        aux = aux->getNextNode();
    }

    return aux;
}

void Graph::printGraph(ofstream &output_file)
{

    if (!this->weighted_edge && !this->weighted_node)
    {

        output_file << "Nodes | Edges " << endl;
        for (Node *p = this->first_node; p != nullptr; p = p->getNextNode())
        {
            output_file << p->getId() << "     | ";
            for (Edge *e = p->getFirstEdge(); e != nullptr; e = e->getNextEdge())
            {
                output_file << " " << e->getTargetId();
            }
            output_file << endl;
        }
    }
    else if (this->weighted_edge && !this->weighted_node)
    {

        output_file << "Nodes | Edges[Weight] " << endl;
        for (Node *p = this->first_node; p != nullptr; p = p->getNextNode())
        {
            output_file << p->getId() << "     | ";
            for (Edge *e = p->getFirstEdge(); e != nullptr; e = e->getNextEdge())
            {
                output_file << " " << e->getTargetId() << "[" << e->getWeight() << "]";
            }
            output_file << endl;
        }
    }
    else if (this->weighted_node && !this->weighted_edge)
    {

        output_file << "Nodes[Weight] | Edges " << endl;
        for (Node *p = this->first_node; p != nullptr; p = p->getNextNode())
        {
            output_file << p->getId() << "[" << p->getWeight() << "]"
                        << " | ";
            for (Edge *e = p->getFirstEdge(); e != nullptr; e = e->getNextEdge())
            {
                output_file << " " << e->getTargetId();
            }
            output_file << endl;
        }
    }
    else if (this->weighted_node && this->weighted_edge)
    {

        output_file << "Nodes[Weight] | Edges[Weight] " << endl;
        for (Node *p = this->first_node; p != nullptr; p = p->getNextNode())
        {
            output_file << p->getId() << "[" << p->getWeight() << "]"
                        << " | ";
            for (Edge *e = p->getFirstEdge(); e != nullptr; e = e->getNextEdge())
            {
                output_file << " " << e->getTargetId() << "[" << e->getWeight() << "]";
            }
            output_file << endl;
        }
    }
}

//Function that verifies if there is a path between two nodes
bool Graph::depthFirstSearch(int initialId, int targetId)
{
    //Creating a bool vector to verify visited nodes
    bool *visited = new bool[order];
    //Verifying if the nodes really exist
    if (getNode(initialId) == nullptr || getNode(targetId) == nullptr)
    {
        return false;
    }
    //Mark all nodes as unvisited before start the dfs
    for (int i = 0; i < order; i++)
    {
        visited[i] = false;
    }
    //Returning the result of the auxiliar method of this function
    return auxDepthFirstSearch(initialId, targetId, visited);
}

void Graph::breadthFirstSearch(ofstream &output_file)
{

    //Queue for visiting nodes
    queue<Node*> toVisit;
    //Vector to know if a node was visited and marking all as unvisited
    bool *visited = new bool[this->order];
    for(int i = 0; i < this->order; i++)
        visited[i] = false;

    Node* auxNode;

    //The alphaNode is already visited and is our root for the BFS
    visited[this->first_node->getId()] = true;
    toVisit.push(this->first_node);

    while(!toVisit.empty()){

        //Auxilar to the node in the queue front
        auxNode = toVisit.front();
        //Then its is removed from the queue
        toVisit.pop();
        //The node's id is printed
        output_file << auxNode->getId() << " ";

        //Recur all of his adjacents
        for(Edge* auxEdge = auxNode->getFirstEdge(); auxEdge != nullptr; auxEdge = auxEdge->getNextEdge()){

            int targetId = auxEdge->getTargetId();
            int respectiveId = this->indexForNodes(targetId);

            //If a adjacent was not visited it is marked as visited and added to the queue
            if(!visited[respectiveId]){

                visited[respectiveId] = true;
                auxNode = this->getNode(targetId);
                toVisit.push(auxNode);

            }

        }

    }

}

Graph *Graph::getComplement()
{
    // Creating the complement graph to be returned and coping the nodes from the orignal graph
    Graph *complement = new Graph(this->order, this->directed, this->weighted_edge, this->weighted_node);

    for (Node *nodeI = this->first_node; nodeI != nullptr; nodeI = nodeI->getNextNode())
    {
        complement->insertNode(nodeI->getId());
    }

    // Comparing the target edges in each node with every other node
    // Inserting edges to the complement graph whenever they don't exist in the original graph
    Node *nodeComplement = complement->first_node;

    for (Node *nodeI = this->first_node; nodeI != nullptr;
         nodeI = nodeI->getNextNode(), nodeComplement = nodeComplement->getNextNode())
    {
        for (Node *nodeJ = this->first_node; nodeJ != nullptr; nodeJ = nodeJ->getNextNode())
        {
            if (nodeI != nodeJ && !nodeI->searchEdge(nodeJ->getId()))
                nodeComplement->insertEdge(nodeJ->getId(), 0);
        }
    }

    return complement;
}

//A function that returns a reverse graph, which is a graph which the arcs have opposite directions to the original graph
Graph* Graph::getTranspose(){

    //Create the reverse as the same order
    Graph *gT = new Graph(this->order, this->directed, this->weighted_edge, this->weighted_node);

    for (Node *n = this->first_node; n != nullptr; n = n->getNextNode())
    {
        for (Edge *e = n->getFirstEdge(); e != nullptr; e = e->getNextEdge())
        {
            gT->makeGraph(e->getTargetId(), n->getId(), e->getWeight());
        }
    }
    return gT;
}

//A function that returns a subjacent of a directed graph, which is a graph which the arcs have opposite directions to the original graph
Graph* Graph::getSubjacent()
{

    //Create the subjacent with the same order
    Graph *gS = new Graph(this->order, 0, this->weighted_edge, this->weighted_node);

    for (Node *n = this->first_node; n != nullptr; n = n->getNextNode())
    {
        for (Edge *e = n->getFirstEdge(); e != nullptr; e = e->getNextEdge())
        {
            gS->makeGraph(n->getId(), e->getTargetId(), e->getWeight());
        }
    }
    return gS;
}

bool Graph::connectedGraph()
{
    if (this->directed)
    {
        Graph *gS = getSubjacent();
        Node *aux = gS->getFirstNode()->getNextNode();
        while (aux != nullptr)
        {
            if (gS->depthFirstSearch(gS->getFirstNode()->getId(), aux->getId()))
            {
                aux = aux->getNextNode();
            }
            else
            {
                return false;
            }
        }
        return true;
    }
    else
    {
        Node *aux = this->getFirstNode();
        aux = aux->getNextNode();
        while (aux != nullptr)
        {
            if (depthFirstSearch(this->first_node->getId(), aux->getId()))
            {
                aux = aux->getNextNode();
            }
            else
            {
                return false;
            }
        }
        return true;
    }
}

//Print the MST, solution edge's weight and the tree weight
void Graph::primMinimumSpanningTree(ofstream &output_file)
{
    //Vector for visited nodes
    int *visited = new int[this->order];
    //Vector for nodes that weren't visited yet 
    int *toVisit = new int[this->order];
    //Variable to set the dynamic size of the visited vector
    int visitedSize = 0;
    //Variable to set the sum of the best weighted edges
    float MSTWeight = 0;
    //Filling toVisit vector with all nodes
    Node *aux = this->getFirstNode();
    while (aux!=nullptr)
    {
        toVisit[indexForNodes(aux->getId())] = aux->getId();
        aux = aux->getNextNode();
    }
    //Preparing loop to find the lighter edge
    aux = this->getFirstNode();
    Edge *auxFirstRun;
    Edge *BestEdgeFR = nullptr;
    int selectedNode;
    //Loop to find the lighter edge
    while (aux!=nullptr)
    {
        auxFirstRun = aux->getFirstEdge();
        while (auxFirstRun!=nullptr)
        {
            if(BestEdgeFR==nullptr||auxFirstRun->getWeight() < BestEdgeFR->getWeight())
            {
                BestEdgeFR = auxFirstRun;
                selectedNode = aux->getId();
            }
            auxFirstRun = auxFirstRun->getNextEdge();
        }
        aux = aux->getNextNode();
    }
    //Marking the initial node and its target as visited, icreasing visited vector dynamic size, and updating the MST Weight
    visited[0] = selectedNode;
    visited[1] = BestEdgeFR->getTargetId();
    visitedSize += 2;
    MSTWeight += BestEdgeFR->getWeight();
    //Printing the first edge of the solution
    output_file << selectedNode << " -- " << BestEdgeFR->getTargetId() << " Peso: " << BestEdgeFR->getWeight() << endl;  
    //Removing initial node and its target of the toVisit vector
    toVisit[indexForNodes(selectedNode)] = -1;
    toVisit[indexForNodes(BestEdgeFR->getTargetId())] = -1;
    //The loop won't stop until all nodes were visited
    while (visitedSize < this->order)
    {
        //Variable to store the best edge for the MST
        Edge *bestEdge = nullptr;
        /*
        Series of loops to search on every visited node for the best edge for the MST 
        The loop won't stop until all visited nodes were analyzed
        */ 
        for(int i = 0; i < visitedSize; i++)
        {
            //Variable to help search edges on the current node
            Edge *auxEdge = this->getNode(visited[i])->getFirstEdge();
            //Loop to analyze every edge on the current node 
            while (auxEdge!=nullptr)
            {
                //See if the target node of the current edge wasn't visited yet
                if(auxEdge->getTargetId()==toVisit[indexForNodes(auxEdge->getTargetId())])
                {
                    //See if the best edge exists in this iteration or the current edge is better than the "best edge"   
                    if(bestEdge == nullptr || auxEdge->getWeight() < bestEdge->getWeight())
                    {
                        //Update the best edge
                        bestEdge = auxEdge;
                        selectedNode = visited[i];
                    }
                }
                auxEdge = auxEdge->getNextEdge();
            }
        }
        //Update the sum with the best edge's weight
        MSTWeight += bestEdge->getWeight();
        //Update the visited vector with the target node of the best edge
        visited[visitedSize]  = bestEdge->getTargetId();
        //Update the dynamic size of the visited vector
        visitedSize++;
        //Remove the target node of the best edge of the toVisit vector
        toVisit[indexForNodes(bestEdge->getTargetId())] = -1;
        //Printing the best edge 
        output_file << selectedNode << " -- " << bestEdge->getTargetId() << " Peso: " << bestEdge->getWeight() << endl;
    }
    //At the end of the loop, print the weight of the tree
    output_file << "Peso da solucao: " << MSTWeight << endl;
}

//Auxiliar methods
bool Graph::hasCircuit()
{

    int *scc = this->stronglyConnectedComponents();
    list<int> aux;

    //We get the graph SCCs and add it to a list
    for (int i = 0; i < this->order; i++)
    {
        aux.push_back(scc[i]);
    }

    //Then the list is sorted
    aux.sort();

    //
    for (list<int>::iterator i = aux.begin(); i != aux.end();)
    {
        int prev = *i;
        i++;
        //If there are equals labels SCCs the graph is cyclic,
        //in this manner is returned that this graph has a circuit
        if (prev == *i)
            return true;
    }

    //If all the labels are different among them, this graph does not have a circuit
    return false;
}

//A function that fixate ordinal indexes for a node list
int Graph::indexForNodes(int id)
{
    int i = 0;
    Node *aux = first_node;
    //Based on the node id, the function will find and return its position on the list
    while (aux != nullptr)
    {
        if (aux->getId() == id)
        {
            return i;
        }
        aux = aux->getNextNode();
        i++;
    }
    //Returning -1 in case of invalid id
    return -1;
}

bool Graph::auxDepthFirstSearch(int initialId, int targetId, bool visited[])
{
    //First mark the current node as visited in the vector
    visited[indexForNodes(initialId)] = true;
    //Base case: see if there is an Edge that connects the current node to the target node
    if (getNode(initialId)->searchEdge(targetId))
    {
        //Mark the target node as visited and return the answer
        visited[indexForNodes(targetId)] = true;
        return true;
    }
    else
    {
        //Try to search in every unvisited node for the target node
        Edge *aux = getNode(initialId)->getFirstEdge();
        while (aux != nullptr)
        {
            if (visited[indexForNodes(aux->getTargetId())] == false)
            {
                if (auxDepthFirstSearch(aux->getTargetId(), targetId, visited))
                {
                    return true;
                }
            }
            aux = aux->getNextEdge();
        }
        //In case there is no path for the target node, return false
        return false;
    }
}

void Graph::exploreOrder(int initialId, int targetId, bool visited[], stack<int> *explored)
{

    //The node is marked that it was visited
    visited[indexForNodes(initialId)] = true;

    //Every node adjacent to it is stacked before
    for (Edge *aux = getNode(initialId)->getFirstEdge(); aux != nullptr; aux = aux->getNextEdge())

        if (visited[indexForNodes(aux->getTargetId())] == false)

            exploreOrder(aux->getTargetId(), targetId, visited, explored);

    //After all the edges of a node are visited, it is stacked
    (*explored).push(initialId);

}

void Graph::auxComponents(int initialId, int targetId, bool visited[], int c[], int label)
{

    //As soon a node is visited, it is labeled with the current component id;
    visited[indexForNodes(initialId)] = true;
    c[indexForNodes(initialId)] = label;

    //And the same is done for every node adjacent to it
    for (Edge *aux = getNode(initialId)->getFirstEdge(); aux != nullptr; aux = aux->getNextEdge())

        if (visited[indexForNodes(aux->getTargetId())] == false)

            auxComponents(aux->getTargetId(), targetId, visited, c, label);


}

// Returns the correspondent node chosen through the alpha coefficient and a random number of a range
Node* Graph::getAlphaNode(Node** nodesSortedByOutDegree, float alpha){

    int alphaId;
    int sizeAlphaRange = floor(this->order * alpha);
    srand((int)time(0));

    if(sizeAlphaRange == 0)
        alphaId = 0;
    else
        alphaId = rand() % sizeAlphaRange;

    return nodesSortedByOutDegree[alphaId];

}

Node** Graph::sortNodesByOutDegree(){

    Node** nodesSortedByOutDegree = new Node*[this->order];
    Node* aux_node = this->first_node;

    for(int i = 0; aux_node != nullptr; i++, aux_node = aux_node->getNextNode())
        nodesSortedByOutDegree[i] = aux_node;

    this->quickSort(nodesSortedByOutDegree, 0, this->order - 1);
    return nodesSortedByOutDegree;

}

void Graph::swap(Node** arr, int i, int j){

    Node* z = arr[i];
    arr[i] = arr[j];
    arr[j] = z;

}

int Graph::partition(Node** arr, int low, int high){

    Node* pivot = arr[high];
    int i = (low - 1);

    for(int j = low; j <= high - 1; j++){

        if(arr[j]->getOutDegree() >= pivot->getOutDegree()) {
            i++;
            this->swap(arr, i, j);
        }

    }

    this->swap(arr, i + 1, high);
    return (i + 1);

}

float** Graph::floydMarshall(){

    float ** dist = new float*[this->order];
    for (int i = 0; i < this->order; i++)
        dist[i] = new float[this->order];

    Node* auxFirstNode = this->first_node;
    Node* auxSecondNode;
    float edgeWeight = 1;

    //building the first matrix with the values of every edge between adjacents nodes.
    for (int i = 0; auxFirstNode != nullptr; auxFirstNode = auxFirstNode->getNextNode(), i++) {
        auxSecondNode = this->first_node;

        for (int j = 0; auxSecondNode != nullptr; auxSecondNode = auxSecondNode->getNextNode(), j++) {   
            Edge *aux = auxFirstNode->hasEdgeBetween(auxSecondNode->getId());
            
            if(this->weighted_edge && aux != nullptr)
                edgeWeight = aux->getWeight();

            if (auxFirstNode->getId() == auxSecondNode->getId()) 
                dist[i][j] = 0;

            else if(aux != nullptr)
                dist[i][j] = edgeWeight;
            
            else
                dist[i][j] = FLT_MAX / 2;
        }
    }

    //Running the floyd Marshall algorithm
    for (int k = 0; k < this->order; k++) {
        // Pick all vertices as source one by one
        for (int i = 0; i < this->order; i++) {
            // Pick all vertices as destination for the
            // above picked source
            for (int j = 0; j < this->order; j++) {
                // If vertex k is on the shortest path from
                // i to j, then update the value of dist[i][j]
                if (dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
    }

    return dist;
}

void Graph::printFloydMatrix(float **dist)
{
    cout << "The following matrix shows the shortest distances"
            " between every pair of vertices " << endl;

    Node *auxNode = this->first_node;

    for (int i = 0; i < this->order + 1; i++) {
        if (i == 0)
            cout << setw(7) << " ";

        else {
            cout << setw(7) << auxNode->getId();
            auxNode = auxNode->getNextNode();
        }
    }

    cout << endl;

    auxNode = this->first_node;

    for (int i = 0; i < this->order; i++) {

        for (int j = 0; j < this->order + 1; j++) {
                
            if (j == 0) {
                cout << setw(7) << auxNode->getId();
                auxNode = auxNode->getNextNode();
            }
            else {
                if (dist[i][j - 1] == FLT_MAX / 2)
                    cout << setw(7) << "INF";

                else
                    cout << setw(7) << dist[i][j - 1];
            }
        }
        cout << endl;
    }
}

float Graph::auxFindWeight()
{
    Node *auxNode = this->getFirstNode();
    Edge *auxEdge = auxNode->getFirstEdge();

    float value = auxEdge->getWeight();

    for(; auxNode != nullptr; auxNode = auxNode->getNextNode())
    {
        for(; auxEdge != nullptr; auxEdge = auxEdge->getNextEdge())
        {
            if(value < auxEdge->getWeight())
                value = auxEdge->getWeight();
        }
    }
    return value;


}

void Graph::quickSort(Node** arr, int low, int high){

    if(low < high){

        int pi = partition(arr, low, high);
        this->quickSort(arr, low, pi - 1);
        this->quickSort(arr, pi + 1, high);

    }

}

// Function to select a alpha by its probability
int Graph::roulette(float* alphaProbabilities, int desiredProbability, int vectorSize){

    float probabilityByNow = 0;
    int i;

    for(i = 0; i < vectorSize; i++){

        probabilityByNow += alphaProbabilities[i] * 100;

        if(probabilityByNow > desiredProbability)
            return i;

    }

}

// Update the q vector
float Graph::updateQ(float* qVector, float* averageVector, int vectorSize, int starSize){

    float sumQ = 0;

    for(int i = 0; i < vectorSize; i++){

        qVector[i] = starSize / averageVector[i];
        sumQ += qVector[i];

    }

    return sumQ;

}

// Update the p vector

void Graph::updateP(float* vectorP, float* vectorQ, int vectorSize, float sumQ){

    for(int i =0; i < vectorSize; i++)
        vectorP[i] = vectorQ[i] / sumQ;

}

//Function to know if the greedy solution is viable
bool Graph::solutionViabilty(list<Node*> solution){

    //Vector to mark all nodes as unvisited
    bool *visited = new bool[this->order];
    for(int i = 0; i < this->order; i++)
        visited[i] = false;

    //Variable to compare if the number of visited nodes is equal to the graph order
    int numVisiteds = 0;

    //Visiting all nodes from each node on the solution
    for(list<Node*>::iterator it = solution.begin(); it != solution.end(); it++){

        int solutionNodeId = this->indexForNodes((*it)->getId());

        if(!visited[solutionNodeId]){
            visited[solutionNodeId] = true;
            numVisiteds++;
        }

        for(Edge* e = (*it)->getFirstEdge(); e != nullptr; e = e->getNextEdge()){

            int targetId = this->indexForNodes(e->getTargetId());

            if(!visited[targetId]){
                visited[targetId] = true;
                numVisiteds++;
            }

        }

    }

    //If all the nodes were visited, we can guarantee that the solution is viable
    if(numVisiteds == this->order)
        return true;

    return false;

}

int Graph::minimalDistanceDijkstra(float *distance, bool *visited)
{
    float min = FLT_MAX / 2;
    int indice;

    for (int i = 0; i < this->order; i++)
        if (distance[i] < min && visited[i] == false) {
            min = distance[i];
            indice = i;
        }

    return indice;
}   

float* Graph::dijkstra(int id)
{
    float *distance = new float[this->order];
    bool *visited = new bool[this->order];
    int *nodes = new int[this->order];

    int i;

    for (i = 0; i < this->order; i++) {
        distance[i] = FLT_MAX / 2;
        visited[i] = false;
    }

    Node *auxNode;

    for (auxNode = getFirstNode(), i = 0; auxNode != nullptr; auxNode = auxNode->getNextNode(), i++)
        nodes[i] = auxNode->getId();

    distance[indexForNodes(id)] = 0;

    for (i = 0; i < this->order - 1; i++) {
        
        int indexMinimalDistance = minimalDistanceDijkstra(distance, visited);

        visited[indexMinimalDistance] = true;
        Node *node = getNode(nodes[indexMinimalDistance]);

        float edgeWeight = 1;
        
        for (Edge *edge = node->getFirstEdge(); edge != nullptr; edge = edge->getNextEdge()) {

            int adjacentNodeIndex = indexForNodes(edge->getTargetId());
            
            if (this->weighted_edge)
                edgeWeight = edge->getWeight();

            if(!visited[adjacentNodeIndex] && distance[indexMinimalDistance] + edgeWeight < distance[adjacentNodeIndex])
                distance[adjacentNodeIndex] = distance[indexMinimalDistance] + edgeWeight;
        }
    }

    return distance;
}