========================================================================
    CONSOLE APPLICATION : Project1AI Project Overview
========================================================================

AI project.

A simple search engine that supports multiple search strategies to solve a
1-dimensional tile ordering problem. 

console arguments:

	search	[-cost]	<DFS|BFS|UCS|GS|A-star> <fileName>

-cost is an optional flag that changes the cost heuristics of UCS, GS, and A-star

*****************************************************************************
Given a row containing an equal number of black and white tiles, and a single 
empty space in an arbitrary initial ordering, this program rearranges the
tiles such that all black tiles are to the left of the empty position, and 
all white tiles are to the right of the empty position. It is only allowed to 
move a tile from its current position to the empty position; you cannot swap two 
tiles directly.

Each input file contains a single line specifying the initial state using B to 
represent a black tile, W to represent white, and x to represent the space.

Example:

When inputting an example file of WxB with the following arguments;
- search DFS tileExample.txt

It prints out the following

Step 0: WxB
Step 1: move 2 WBx
Step 2: move 0 xBW
Step 3: move 1 BxW

The program supports five different search strategies, selectable at run-time using a
command-line keyword: 
	- Breadth-First Search 	(BFS)
	- Depth-First Search 	(DFS)
	- Uniform-Cost Search	(UCS)
	- Greedy search 	(GS)
	- A* 			(A-star) 

For UCS, it is assumed that g(n) = number of moves executed so far
For GS, it is assumed that h(n) = number of tiles out of place (e.g., in the 3-position example we
started with 2 out-of-place tiles)
For A*, estimate the total path cost as f(n) = g(n)+h(n) where g and h are defined as above.

Adding the -cost floag changes the cost heuristic to weigh the cost of each move by the number of positions
the tile moves. For example, the new costs of performing the operations in Figure 1 are 1,2,1
(respectively), resulting in a total cost of 5.

In addition to the operator and state path, this version of your program should print out these
operation costs:
Step 0: WxB
Step 1: move 2 WBx (c=1)
Step 2: move 0 xBW (c=2)
Step 3: move 1 BxW (c=1)

console arguments;