//
//  agent.cpp
//  Project_Euler
//
//  Created by Free Caenepeel on 16/08/2023.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <iterator>
#include <map>
#include <array>
#include <list>
#include <set>
#include <cstdlib>
#include <numeric>
#include <random>

using namespace std;


const int n_elevators = 1;
const int n_floors = 10;
const int actions[3][1] = {{0},{1},{-1}};

// first tried with 3 elevators
//const int actions[27][3] = {{0,0,0},{0,0,1},{0,0,-1},{0,1,1},{0,1,-1},{0,-1,-1},
//    {0,-1,1},{0,1,0},{0,-1,0},{1,0,0},{1,0,1},{1,0,-1},{1,1,1},{1,1,-1},
//    {1,-1,-1},{1,-1,1},{1,1,0},{1,-1,0},{-1,0,0},{-1,0,1},{-1,0,-1},
//    {-1,1,1},{-1,1,-1},{-1,-1,-1},{-1,-1,1},{-1,1,0},{-1,-1,0}};

const int N = n_elevators*(n_floors+3) + 2 * n_floors + 2;
const int W = (n_elevators*(n_floors+3) + 2 * n_floors + 2) * 3;


// random generator to generate requested floors
class RNG
{
public:
    RNG() : gen(std::random_device()()) {} // Seeds the mt19937.

    double get_random(int b){
        std::uniform_real_distribution<double> dis(0, b);
        double random = dis(gen);
        return random;
    }
    
    int get_random_integer(int b) {
        std::uniform_int_distribution<int> dis(0,b);
        int random = dis(gen);
        return random;
    }
    
private:
    std::mt19937 gen;
};

class DeterministicAgent {
public:
    
    // state parameters
    int floors[n_elevators];          // current floors of elevators
    int sleep[n_elevators];           // how much longer will elevators sleep
    int queue_up[n_floors + 1];       // people waiting to go up
    int queue_down[n_floors + 1];     // peopple waiting to go down
    int waiting_in_elevator[n_elevators][n_floors+1]; // people inside elevator with requested floors

    int full[n_elevators];            // how many people are still in elevator
    int action[n_elevators];          // current action by elevators
    int passengers_dropped_off;
    
    RNG rng;
    
    DeterministicAgent () {
        for (int i = 0; i < n_elevators; i++) {
            floors[i] = 0;
            full[i] = 0;
            sleep[i] = 0;
            for (int j = 0; j <= n_floors; j++) {
                waiting_in_elevator[i][j] = 0;
            }
            action[i] = 0;
        }

        // empty queues
        for (int i = 0; i <= n_floors; i++) {
            queue_up[i] = 0;
            queue_down[i] = 0;
        }
        
        passengers_dropped_off = 0;
    }
    
    int people_above(int j) {
        int total = 0;
        for (int i = this->floors[j]; i <= n_floors; i ++ ) {
            total += this->waiting_in_elevator[j][i];
            total += this->queue_up[i];
        }
        return total;
    }
    
    int people_below(int j) {
        int total = 0;
        for (int i = this->floors[j]; i >= 0; i-- ) {
            total += this->waiting_in_elevator[j][i];
            total += this->queue_down[i];
        }
        return total;
    }
    
    void next_state() {

        for (int i = 0; i < n_elevators; i++) {
            // if elevator was not sleeping yet, and stops then sleep for 3 steps
            if (action[i] == 0 and this->sleep[i] == 0) {
                this->sleep[i] = 3;
            }
            // still sleeping
            else if (this->sleep[i] > 0) {
                this->sleep[i] -= 1;
            }
            // move elevators
            else {
                this->floors[i] = min(max(0,this->floors[i] + action[i]),n_floors);
            }
        }
        
        // check that people can get out at requested floor
        for (int i = 0; i < n_elevators; i++) {
            if (this->full[i] > 0 && action[i] == 0) {
                if (this->waiting_in_elevator[i][this->floors[i]] > 0) {
                    this->passengers_dropped_off += this->waiting_in_elevator[i][this->floors[i]];
                    this->full[i] -= this->waiting_in_elevator[i][this->floors[i]];
                    this->waiting_in_elevator[i][this->floors[i]] = 0;
                    this->sleep[i] = 3;
                }
            }
        }
        
        // check that people waiting outside can get picked up
        int people;
        int up;
        int down;
        bool up_prio;
        bool down_prio;
        for (int i = 0; i < n_elevators; i ++) {
            up_prio = false;
            down_prio = false;
            // situation where people are wanting to go up and down
            if (this->queue_up[this->floors[i]] > 0 && this->queue_down[this->floors[i]] > 0 && action[i] == 0) {
                up = this->people_above(i);
                down = this->people_below(i);
                if (up >= down) {
                    up_prio = true;
                }
                else {down_prio = true;}
                
            }
            if (this->queue_up[this->floors[i]] > 0 && action[i] == 0 && !down_prio) {
                people = min(this->queue_up[this->floors[i]], 10 - this->full[i]);
                this->full[i] += people;
                for (int k = 0; k < people; k++) {
                // generate floor above current floor
                    int f_above = this->floors[i] + ( this->rng.get_random_integer(n_floors - this->floors[i]));
                    this->waiting_in_elevator[i][f_above] += 1;
                }
                this->queue_up[this->floors[i]] -= people;
                this->sleep[i] = 3;
            }
            else if (this->queue_down[this->floors[i]] > 0 && action[i] == 0 && !up_prio) {
                people = min(this->queue_down[this->floors[i]], 10 - this->full[i]);
                this->full[i] += people;
                
                for (int k = 0; k < people; k++) {
                // generate floor below current floor
                    int f_below = this->rng.get_random_integer(max(this->floors[i] - 1,0));
                    this->waiting_in_elevator[i][f_below] += 1;
                }
                this->queue_down[this->floors[i]] -= people;
                this->sleep[i] = 3;
            }
        }
    }
    
    // determine the next action based on current state
    // send the closest elevator to pick up people
    void determine_action() {
        vector<int> empty_elevators;
        vector<vector<int>> distances_to_waiting_floors;
        // send the closest empty elevator
        for (int i = 0; i < n_elevators; i ++) {
            if (this->full[i] > 0) {
                // if elevator just stopped at floor, make it move again
                if (this->sleep[i] > 0) {
                    this->action[i] = 0;
                }
                // if there is somebody waiting on the floor, the person will be able to get
                // out on the next step
                else if (this->waiting_in_elevator[i][this->floors[i]] > 0) {
                    this->action[i] = 0;
                }
                // elevator is going up, stop to pick more people up
                else if (this->queue_up[this->floors[i]] && this->action[i] == 1 && this->full[i] < 10) {
                        this->action[i] = 0;
                }
                // elevator is going down, stop to pick more people up
                else if (this->queue_down[this->floors[i]] && this->action[i] == -1 && this->full[i] < 10) {
                        this->action[i] = 0;
                }
                // elevator stopped at floor, send up or down depending on people
                else if (this->action[i] == 0 and this->sleep[i] == 0) {
                    int index = 1;
                    bool up = false;
                    while (this->floors[i] + index <= n_floors) {
                        if (this->waiting_in_elevator[i][this->floors[i] + index] > 0) {
                            up = true;
                            break;
                        }
                        index += 1;
                    }
                    if (up) {this->action[i] = 1;}
                    else {this->action[i] = -1;}
                }

            }
            // elevator is empty, but can stop to pick up people
            else if (this->queue_up[this->floors[i]] || this->queue_down[this->floors[i]]) {
                    this->action[i] = 0;
            }
            else {
            empty_elevators.push_back(i);
            }
            
        }
        
        int k = int(empty_elevators.size());
        
        // all elevators are full, we're done
        if (k == 0) {
            return;
        }
        
        vector<pair<pair<int,int>,int>> shortest_distances_per_elevator;
        int distance = 0;
        
        // determine shortest elevator
        for (int j = 0; j < k; j++) {
            for (int i = 0; i <= n_floors; i++) {
                if (this->queue_up[i] || this->queue_down[i]) {
                    distance = abs(i - this->floors[empty_elevators[j]]);
                    shortest_distances_per_elevator.push_back(make_pair(make_pair(distance, i),empty_elevators[j]));
                    
                }
            }
            sort(shortest_distances_per_elevator.begin(), shortest_distances_per_elevator.end());
            
        }
        
        vector<int> found;
        int index = 0;
        while (found.size() < k && index < shortest_distances_per_elevator.size()) {
            if (std::find(found.begin(), found.end(), shortest_distances_per_elevator[index].second) == found.end()) {
                found.push_back(shortest_distances_per_elevator[index].second);
                if (shortest_distances_per_elevator[index].first.second > this->floors[shortest_distances_per_elevator[index].second]) {
                    this->action[shortest_distances_per_elevator[index].second] = 1;
                }
                else {
                    this->action[shortest_distances_per_elevator[index].second] = -1;
                }
            }
                
            index += 1;
            }
        
        return;
    }
    
    void run_one_step_of_generated_queue(int timestamp, int floor, bool direction) {
        // add the new person to the correct queue
        if (direction) { // up
            this->queue_up[floor] = min(10,this->queue_up[floor]+1);
        }
        else {
            this->queue_down[floor] = min(10,this->queue_down[floor]+1);
        }
        
        // run for timestamp number of times
        for (int i = 0; i <= timestamp; i++) {
            this->determine_action();
            this->next_state();
        }
        
        
    }
    
    void pretty_print() {
        std::cout << "floors" << endl;
        for (int i = 0; i < n_elevators; i++) {
            std::cout << this->floors[i] << " ";
        }
        
        std::cout << endl;
        std::cout << " people in elevator" << endl;
        std::cout << "full " << this->full[0] << endl;
        for (int i = 0; i < n_elevators; i++) {
            std::cout << "elevator " << i << endl;
            for (int j = 0; j <= n_floors; j++) {
                std::cout << this->waiting_in_elevator[i][j] << " ";
            }
        }
    
        std::cout << endl;
        std::cout << " queuing up " << endl;
        for (int j = 0; j <= n_floors; j++) {
            std::cout << this->queue_up[j] << " ";
        }
        std::cout << endl;
        std::cout << " queuing down " << endl;
        for (int j = 0; j <= n_floors; j++) {
            std::cout << this->queue_down[j] << " ";
        }
        std::cout << endl;
        
    }
    
    
};



