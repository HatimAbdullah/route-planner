#include "route_planner.h"
#include <algorithm>
#include <iostream>
using std::cout;
using std::sort;

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

    // TODO 2: Use the m_Model.FindClosestNode method to find the closest nodes to the starting and ending coordinates.
    // Store the nodes you find in the RoutePlanner's start_node and end_node attributes.
    //*start_node = m_Model.FindClosestNode(start_x, start_y);
    this->start_node = &model.FindClosestNode(start_x, start_y);
    this->end_node = &model.FindClosestNode(end_x, end_y);


}


// TODO 3: Implement the CalculateHValue method.
// Tips:
// - You can use the distance to the end_node for the h value.
// - Node objects have a distance method to determine the distance to another node.

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
      return end_node->distance(*node);
}


// TODO 4: Complete the AddNeighbors method to expand the current node by adding all unvisited neighbors to the open list.
// Tips:
// - Use the FindNeighbors() method of the current_node to populate current_node.neighbors vector with all the neighbors.
// - For each node in current_node.neighbors, set the parent, the h_value, the g_value. 
// - Use CalculateHValue below to implement the h-Value calculation.
// - For each node in current_node.neighbors, add the neighbor to open_list and set the node's visited attribute to true.

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    current_node->FindNeighbors();
    //current_node->visited = true;
    
    for(RouteModel::Node *i: current_node->neighbors) {
        i->parent = current_node;
        i->g_value = current_node->g_value + i->distance(*current_node);
        i->h_value = this->CalculateHValue(i);
        i->visited = true;
        this->open_list.push_back(i);
    }

}


// TODO 5: Complete the NextNode method to sort the open list and return the next node.
// Tips:
// - Sort the open_list according to the sum of the h value and g value.
// - Create a pointer to the node in the list with the lowest sum.
// - Remove that node from the open_list.
// - Return the pointer.

RouteModel::Node *RoutePlanner::NextNode() {

    RouteModel::Node* nextnode;
    sort(this->open_list.begin(), this->open_list.end(), [](const auto &a, const auto &b)
	{
		float f1 =a->h_value + a->g_value; // f1 = g1 + h1
        float f2 = b->h_value + b->g_value; // f2 = g2 + h2
        return f1 < f2; 
	});

    nextnode = open_list.front();
    open_list.erase(open_list.begin());

    return nextnode;
}


// TODO 6: Complete the ConstructFinalPath method to return the final path found from your A* search.
// Tips:
// - This method should take the current (final) node as an argument and iteratively follow the 
//   chain of parents of nodes until the starting node is found.
// - For each node in the chain, add the distance from the node to its parent to the distance variable.
// - The returned vector should be in the correct order: the start node should be the first element
//   of the vector, the end node should be the last element.

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    distance = 0.0f;
    std::vector<RouteModel::Node> path_found;

    // TODO: Implement your solution here.
    RouteModel::Node nextnode = *current_node;
    do {
        path_found.push_back(nextnode);
        distance = distance + nextnode.distance(*nextnode.parent);
        nextnode = *nextnode.parent;
    } while(nextnode.parent);
    
    //add the starting node to the path
    path_found.push_back(*start_node);
    
    //reverse order so start node is first
    std::reverse(path_found.begin(),path_found.end());

    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    return path_found;

}


// TODO 7: Write the A* Search algorithm here.
// Tips:
// - Use the AddNeighbors method to add all of the neighbors of the current node to the open_list.
// - Use the NextNode() method to sort the open_list and return the next node.
// - When the search has reached the end_node, use the ConstructFinalPath method to return the final path that was found.
// - Store the final path in the m_Model.path attribute before the method exits. This path will then be displayed on the map tile.

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;
    cout << "starting search .. \n";
    current_node = this->start_node;
    open_list.push_back(current_node);
    current_node->visited = true;

    do {
        AddNeighbors(current_node);
        current_node = this->NextNode();
        if (current_node->x == end_node->x && current_node->y == end_node->y) {
            cout << "constructing path \n";
            m_Model.path = this->ConstructFinalPath(current_node);
            return;
        }
    } while (this->open_list.size() > 0);
    return;
}