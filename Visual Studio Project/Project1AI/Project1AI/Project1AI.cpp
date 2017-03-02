#include "stdafx.h"
#include <iostream>
#include <ostream>
#include <fstream>
#include <string>

#include <vector>
#include <algorithm>
#include <stack>
#include <queue>
#include <concurrent_queue.h>
#include <concurrent_priority_queue.h>

using namespace std;

vector<string> expandedTiles;
struct Node {
public:
  Node(Node* p, string set, int movedTile, int d, int c)
  {
    parent = p;
    tileSet = set;
    movedTileIndex = movedTile;
    depth = d;
    cost = c;

    for (int x = 0; x < tileSet.length(); x++)
    {
      char tile = tileSet[x];

      if ((tile == 'B' && x > tileSet.length() / 2) || (tile == 'W' && x < tileSet.length() / 2))
        numNotInPlace++;
    }
  }

  Node* parent;
  vector<Node*> children;
  string tileSet;
  int movedTileIndex;
  int depth;
  int cost;
  int numNotInPlace;

  vector<Node*> GetChildren()
  {
    children.clear();

    // find where the empty tile is
    int emptyIndex = tileSet.find('x');

    // make a child for each tile switching with that empty tile
    for (int x = 0; x < tileSet.length(); x++)
    {
      if (x != emptyIndex)
      {
        string newTileset = tileSet;

        newTileset.at(emptyIndex) = newTileset.at(x);
        newTileset.at(x) = 'x';
        
        if (std::find(expandedTiles.begin(), expandedTiles.end(), newTileset) == expandedTiles.end())
          children.push_back(new Node(this, newTileset, x, depth + 1, cost + 1));
      }
    }

    expandedTiles.push_back(tileSet);

    return children;
  }

  vector<Node*> GetChildrenWithDistanceCost()
  {
    children.clear();

    // find where the empty tile is
    int emptyIndex = tileSet.find('x');

    // make a child for each tile switching with that empty tile
    for (int x = 0; x < tileSet.length(); x++)
    {
      if (x != emptyIndex)
      {
        string newTileset = tileSet;

        int distance = std::abs(emptyIndex - x);

        newTileset.at(emptyIndex) = newTileset.at(x);
        newTileset.at(x) = 'x';

        if (std::find(expandedTiles.begin(), expandedTiles.end(), newTileset) == expandedTiles.end())
          children.push_back(new Node(this, newTileset, x, depth + 1, distance));
      }
    }

    expandedTiles.push_back(tileSet);

    return children;
  }

  bool operator() (const Node& struct1, const Node& struct2)
  {
    return (struct1.numNotInPlace < struct2.numNotInPlace);
  }
  bool operator == (const Node* o) { return cost == o->cost; }
  bool operator < (const Node* o) { return cost < o->cost; }
  bool operator > (const Node* o) { return cost > o->cost; }
};

vector<Node*> BFS(Node* root, string goal);
vector<Node*> DFS(Node* root, string goal);
vector<Node*> UCS(Node* root, string goal, bool costFlag);
vector<Node*> GS(Node* root, string goal, bool costFlag);
vector<Node*> Astar(Node* root, string goal, bool costFlag);
vector<Node*> SortChildrenByNumInPlace(vector<Node*> c);
vector<Node*> SortChildrenByBothHeuristics(vector<Node*> c);
vector<Node*> SortChildrenByCostFlag(vector<Node*> c);
vector<Node*> SortChildrenByDepth(vector<Node*> c);

int main (int argc, char *argv[] )
{
  // .exe search [-cost] <BFS|DFS|UCS|GS|A-star> <inputfile>
  // 0      1       2    3                        4
  
  ifstream inputFile;
  string searchType;
  string fileName;
  bool costFlag = false;

  if (argc < 4 || argc > 5) 
  {
    std::cout << "Missing/Too many arguments. Console arguments are;\nsearch [-cost] <BFS|DFS|UCS|GS|A-star> <inputfile>";
    std::cin.ignore();
    return 0;
  }

  // If there are only 3 arguments (as in, if there is no -cost flag)
  if (argc == 4) 
  {
    // What type of search does the user want?
    searchType = argv[2];
    fileName = argv[3];
  }
  
  // If there are 4 arguments (as in, the -cost flag is enabled)
  else
  {
    // What type of search does the user want?
    searchType = argv[3];
    fileName = argv[4];
    costFlag = true;
  }

  // If the user didn't input a proper search string
  if (searchType != "BFS" && searchType != "DFS" && searchType != "UCS" && searchType != "GS" && searchType != "A-star")
  {
    std::cout << "Please use <BFS|DFS|UCS|GS|A-star> to specify a search algorithm you want to use.";
    std::cin.ignore();
    return 0;
  }

  inputFile.open(fileName);

  if (!inputFile) {
    std::cout << "Could not find file. Check your spelling or place it in the proper directory";
    std::cin.ignore();
    return 0;
  }
  
  string initialState;
  if (inputFile.good())
    getline(inputFile, initialState);
  inputFile.close();

  Node* root = new Node(NULL, initialState, -1, 0, 0);
  string goalState;
  for (int x = 0; x < initialState.length(); x++)
  {
    if (x < initialState.length() / 2)
      goalState += "B";
    else if (x > initialState.length() / 2)
      goalState += "W";
    else
      goalState += "x";
  }

  vector<Node*> path;

  if (searchType == "BFS")
    path = BFS(root, goalState);
  else if (searchType == "DFS")
    path = DFS(root, goalState);
  else if (searchType == "UCS")
    path = UCS(root, goalState, costFlag);
  else if (searchType == "GS")
    path = GS(root, goalState, costFlag);
  else
    path = Astar(root, goalState, costFlag);

  std::cout << "Final Path for " << path[0]->tileSet << endl;
  if(!costFlag)
    for (int x = 1; x < path.size(); x++)
      std::cout << "Step " << x << ": move " << path[x]->movedTileIndex << " " << path[x]->tileSet << endl;
  else
    for (int x = 1; x < path.size(); x++)
      std::cout << "Step " << x << ": move " << path[x]->movedTileIndex << " " << path[x]->tileSet << " (c=" << path[x]->cost << ")" << endl;

  std::cin.ignore();
}

// Returns a vector containing the ath of nodes from the Root node to the passed endNode.
vector<Node*> ReturnPath(Node* endNode) 
{
  vector<Node*> path;
  Node* currentNode = endNode;
  while (currentNode != NULL)
  {
    path.push_back(currentNode);
    currentNode = currentNode->parent;
  }

  reverse(path.begin(), path.end());
  return path;
}

// Function for BFS
// Uses a Queue
vector<Node*> BFS(Node* root, string goal) 
{
  std::cout << "Breadth First Search" << endl;

  std::queue<Node*> Q;
  std::vector<Node*> children;
  Node* goalNode = NULL;

  Q.push(root);
  while (!Q.empty())
  {
    Node* t = Q.front();

    Q.pop();

    // If this type of node hasn't already been expanded
    // (There are multiple ways to arrive to the same tileSet order)
    if (std::find(expandedTiles.begin(), expandedTiles.end(), t->tileSet) == expandedTiles.end())
    {
      if (t->tileSet == goal)
        return ReturnPath(t);

      children = t->GetChildren();

      for (int i = 0; i < children.size(); ++i)
        Q.push(children[i]);
    }
  }

  std::cout << "No path to the goal found." << endl;
  vector<Node*> none;
  return none;
}

// Function for DFS
// Uses a Stack
vector<Node*> DFS(Node* root, string goal)
{
  std::cout << "Depth First Search" << endl;
  std::stack<Node*> Q;
  std::vector<Node*> children;

  Q.push(root);
  while (!Q.empty())
  {
    Node* t = Q.top();

    Q.pop();
    // If this type of node hasn't already been expanded
    // (There are multiple ways to arrive to the same tileSet order)
    if (std::find(expandedTiles.begin(), expandedTiles.end(), t->tileSet) == expandedTiles.end())
    {
      if (t->tileSet == goal)
        return ReturnPath(t);

      children = t->GetChildren();

      std::reverse(children.begin(), children.end());

      for (int i = 0; i < children.size(); ++i)
        Q.push(children[i]);
    }
  }

  std::cout << "No path to the goal found." << endl;
  vector<Node*> none;
  return none;
}

// Function for UCS
// Uses Priority Queue
// g(n) = number of moves executed so far
vector<Node*> UCS(Node* root, string goal, bool costFlag)
{
  std::cout << "Uniform Cost Search" << endl;
  std::queue<Node*> Q;
  std::vector<Node*> children;

  Q.push(root);
  while (!Q.empty())
  {
    Node* t = Q.front();

    Q.pop();

    if (std::find(expandedTiles.begin(), expandedTiles.end(), t->tileSet) == expandedTiles.end())
    {
      if (t->tileSet == goal)
        return ReturnPath(t);
    
      if (!costFlag) 
      {
        children = t->GetChildren();
        children = SortChildrenByDepth(children);
      }
      else 
      {
        children = t->GetChildrenWithDistanceCost();  
        children = SortChildrenByCostFlag(children);
      }

      for (int i = 0; i < children.size(); i++)
        Q.push(children[i]);
    }
  }
  
  vector<Node*> none;
  return none;
}

// Function for GS
// Heuristic = number of tiles out of place
vector<Node*>  GS(Node* root, string goal, bool costFlag)
{
  std::cout << "Greedy Search" << endl;

  std::queue<Node*> Q;
  std::vector<Node*> children;

  Q.push(root);
  while (!Q.empty())
  {
    Node* t = Q.front();

    Q.pop();

    // If this type of node hasn't already been expanded
    // (There are multiple ways to arrive to the same tileSet order)
    if (std::find(expandedTiles.begin(), expandedTiles.end(), t->tileSet) == expandedTiles.end())
    {
      if (t->tileSet == goal)
        return ReturnPath(t);

      if (!costFlag) 
      {
        children = t->GetChildren();
        children = SortChildrenByNumInPlace(children);
      }
      else 
      {
        children = t->GetChildrenWithDistanceCost();
        children = SortChildrenByCostFlag(children);
      }

      for (int i = 0; i < children.size(); ++i)
        Q.push(children[i]);
    }
  }

  std::cout << "No path to the goal found." << endl;
  vector<Node*> none;
  return none;
}

// Function for A*
// Heuristic = number of tiles out of place + number of moves executed so far
vector<Node*>  Astar(Node* root, string goal, bool costFlag)
{
  std::cout << "A* Search" << endl;

  std::queue<Node*> Q;
  std::vector<Node*> children;

  Q.push(root);
  while (!Q.empty())
  {
    Node* t = Q.front();

    Q.pop();

    // If this type of node hasn't already been expanded
    // (There are multiple ways to arrive to the same tileSet order)
    if (std::find(expandedTiles.begin(), expandedTiles.end(), t->tileSet) == expandedTiles.end())
    {
      if (t->tileSet == goal)
        return ReturnPath(t);

      if (!costFlag)
      {
        children = t->GetChildren();
        children = SortChildrenByBothHeuristics(children);
      }
      else 
      {
        children = t->GetChildrenWithDistanceCost();
        children = SortChildrenByCostFlag(children);
      }

      for (int i = 0; i < children.size(); ++i)
        Q.push(children[i]);
    }
  }

  std::cout << "No path to the goal found." << endl;
  vector<Node*> none;
  return none;
}

vector<Node*> SortChildrenByDepth(vector<Node*> c)
{
  vector<Node*> mainVector = c;

  Node* min;
  Node* temp;

  for (int x = 0; x < mainVector.size(); x++)
  {
    min = mainVector[x];
    int loc = x;

    for (int k = x + 1; k < mainVector.size(); k++)
    {
      if (min->depth > mainVector[k]->depth)
      {
        min = mainVector[k];
        loc = k;
      }
    }
    temp = mainVector[x];
    mainVector[x] = mainVector[loc];
    mainVector[loc] = temp;
  }

  return mainVector;
}

vector<Node*> SortChildrenByNumInPlace(vector<Node*> c) 
{
  vector<Node*> mainVector = c;

  Node* min;
  Node* temp;

  for (int x=0; x < mainVector.size(); x++)
  {
    min = mainVector[x];
    int loc = x;

    for (int k = x + 1; k < mainVector.size(); k++) 
    {
      if (min->numNotInPlace > mainVector[k]->numNotInPlace) 
      {
        min = mainVector[k];
        loc = k;
      }
    }
    temp = mainVector[x];
    mainVector[x] = mainVector[loc];
    mainVector[loc] = temp;
  }

  return mainVector;
}

vector<Node*> SortChildrenByBothHeuristics(vector<Node*> c)
{
  vector<Node*> mainVector = c;

  Node* min;
  Node* temp;

  for (int x = 0; x < mainVector.size(); x++)
  {
    min = mainVector[x];
    int loc = x;

    for (int k = x + 1; k < mainVector.size(); k++)
    {
      if (min->numNotInPlace + min->depth > mainVector[k]->numNotInPlace + mainVector[k]->depth)
      {
        min = mainVector[k];
        loc = k;
      }
    }
    temp = mainVector[x];
    mainVector[x] = mainVector[loc];
    mainVector[loc] = temp;
  }

  return mainVector;
}

vector<Node*> SortChildrenByCostFlag(vector<Node*> c) {
  vector<Node*> mainVector = c;

  Node* min;
  Node* temp;

  for (int x = 0; x < mainVector.size(); x++)
  {
    min = mainVector[x];
    int loc = x;

    for (int k = x + 1; k < mainVector.size(); k++)
    {
      if (min->cost > mainVector[k]->cost)
      {
        min = mainVector[k];
        loc = k;
      }
    }
    temp = mainVector[x];
    mainVector[x] = mainVector[loc];
    mainVector[loc] = temp;
  }

  return mainVector;
}