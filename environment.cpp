#include "deterministic_agent.cpp"
#include "neural.cpp"

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

using namespace std;

class Environment {
public:

    NNStateValue network;
    double state_value_w_weights[N][10];                    // w weights for value function
    double state_value_b_weights[N][10];                    // b weights for value function
    double learning_rate_for_state_value;
                                      
    // state parameters
    int floors[n_elevators];                                // current floors of elevators
    int sleep[n_elevators];                                 // sleeping time of elevators
    int current_movement[n_elevators];                      // current movement of elevator
    int queue_up[n_floors + 1];                             // people waiting to go up
    int queue_down[n_floors + 1];                           // peopple waiting to go down
    int waiting_in_elevator[n_elevators][n_floors+1];       // people inside elevator with requested floors
    int full[n_elevators];                                  // how many people are still in elevator
    int passengers_dropped_off;
    
    
    RNG rng;
    
    Environment (vector<double> inputs, vector<vector<double>> w_weights, vector<vector<double>> b_weights):
        network(inputs, w_weights, b_weights)
    {
        for (int i = 0; i < n_elevators; i++) {
            floors[i] = 0;
            full[i] = 0;
            sleep[i] = 0;
            current_movement[i] = 0;
            for (int j = 0; j <= n_floors; j++) {
                waiting_in_elevator[i][j] = 0;
            }
        }
        // initialize state value weights to 0
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < 10; j++) {
                state_value_w_weights[i][j] = 0.0;
                state_value_b_weights[i][j] = 0.0;
            }
        }
        // empty queues
        for (int i = 0; i <= n_floors; i++) {
            queue_up[i] = 0;
            queue_down[i] = 0;
        }
        learning_rate_for_state_value = 0.05;
        passengers_dropped_off = 0;
    }
    
    // set the state of the environment.
    // Used to check probabilities in different states
    void set_state(int floors[n_elevators],
                   int queue_up[n_floors + 1],
                   int queue_down[n_floors + 1],
                   int full[n_elevators]) {
        
        for (int i = 0; i < n_elevators; i++) {
            this->floors[i] = floors[i];
            this->full[i] = full[i];
        }
        
        for (int i = 0; i <= n_floors; i++) {
            this-> queue_up[i] = queue_up[i];
            this-> queue_down[i] = queue_down[i];
        }
        
        // empty all elevators
        for (int i = 0; i < n_elevators; i++) {
            this->sleep[i] = 0;
            this->current_movement[i] = 0;
            for (int j = 0; j <= n_floors; j++) {
                this->waiting_in_elevator[i][j] = 0;
            }
        }
        
    }
    
    // reset state
    // used during training to reset
    void reset_state(int a, int b, int c) {
        
        for (int i = 0; i < n_elevators; i++) {
            this->full[i] = 0;
        }
        
        this->floors[0] = a;
       // this->floors[1] = b;
       // this->floors[2] = c;
        
        for (int i = 0; i <= n_floors; i++) {
            this-> queue_up[i] = 0;
            this-> queue_down[i] = 0;
        }
        
        // empty all elevators for test
        for (int i = 0; i < n_elevators; i++) {
            this->sleep[i] = 0;
            this->current_movement[i] = 0;
            for (int j = 0; j <= n_floors; j++) {
                this->waiting_in_elevator[i][j] = 0;
            }
        }
        
    }
    
    // go to the next state
    // compute the reward used for updating
    int next_state(vector<int> action) {
        // give a negative reward for full elevators
        int reward = 0;
        for (int i = 0; i < n_elevators; i++) {
            for (int j = 0; j <= n_floors; j++) {
                reward -= this->waiting_in_elevator[i][j];
            }
        }
        
        // first move every elevator according to action
        for (int i = 0; i < n_elevators; i++) {
            // if elevator was not sleeping yet, and stops then sleep for 3 steps
            if (action[i] == 0 and this->sleep[i] == 0) {
                this->sleep[i] = 3;
                this->current_movement[i] = 0;
            }
            else if (this->sleep[i] > 0) {
                this->sleep[i] -= 1;
                // give negative reward for going below 0 or above top floor
                if (this->floors[i] + action[i] > n_floors || this->floors[i] + action[i] < 0) {
                    reward -= 10;
                    }
                this->current_movement[i] = 0;
            }
            else {
                // give negative reward for going below 0 or above top floor
                if (this->floors[i] + action[i] > n_floors || this->floors[i] + action[i] < 0) {
                    reward -= 20;
                    }
                this->floors[i] = min(max(0,this->floors[i] + action[i]),n_floors);
                this->current_movement[i] = action[i];
            }
        }
        
        int people = 0;
        
        // check that people can get out at requested floor
        for (int i = 0; i < n_elevators; i++) {
            // elevator needs to be still to let people out and in
            if (action[i] == 0) {
                // give penalty for stopping at wrong floor
                if (this->waiting_in_elevator[i][this->floors[i]] == 0 && this->queue_up[this->floors[i]] == 0 && this->queue_down[this->floors[i]] == 0) {
                    reward -= 5;
                }
                // people can get out
                if (this->waiting_in_elevator[i][this->floors[i]] > 0) {
                    this->passengers_dropped_off += this->waiting_in_elevator[i][this->floors[i]];
                    reward += 100 * this->waiting_in_elevator[i][this->floors[i]];
                    this->full[i] -= this->waiting_in_elevator[i][this->floors[i]];
                    this->waiting_in_elevator[i][this->floors[i]] = 0;
                    this->sleep[i] = 3;
                    }
                
                // people can get in
                if (this->queue_up[this->floors[i]] > 0) {
                    // maximum 10 people in the elevator
                    people = min(this->queue_up[this->floors[i]], 10 - this->full[i]);
                    this->full[i] += people;
                    reward += people * 100;
                    // give penalty for stopping with full elevator
                    if (people == 0) {
                        reward -= 10;
                    }
                    // generator floor above current floor for all newcomers
                    for (int k = 0; k < people; k++) {
                        int f_above = this->floors[i] + ( this->rng.get_random_integer(n_floors - this->floors[i] ));
                        this->waiting_in_elevator[i][f_above] += 1;
                    }
                    this->queue_up[this->floors[i]] -= people;
                    this->sleep[i] = 3;
                }
                if (this->queue_down[this->floors[i]] > 0) {
                    // maximum 10 people in the elevator
                    people = min(this->queue_down[this->floors[i]], 10 - this->full[i]);

                    this->full[i] += people;
                    reward += people * 100;
                    // give penalty for stopping with full elevator
                    if (people == 0) {
                        reward -= 10;
                    }

                    // generator floor below current floor for all newcomers
                    for (int k = 0; k < people; k++) {
                        int f_below = this->rng.get_random_integer(max(this->floors[i] - 1,0));
                        this->waiting_in_elevator[i][f_below] += 1;
                    }
                    this->queue_down[this->floors[i]] -= people;
                    this->sleep[i] = 3;
                }
            }
            // give partial reward for having moved in correct direction
            else if (action[i] == 1) {
                
                if (this->people_above(i)) {
                    reward += 50;
                }
            }
            else {
                if (this->people_below(i)) {
                        reward += 50;
                    }
                }
                    
        }
        
        return reward;
        
        
    }
    
    bool people_above(int j) {
        for (int i = this->floors[j]; i <= n_floors; i ++ ) {
            if (this->waiting_in_elevator[j][i] > 0 || this->queue_up[i] > 0 ) {
                return true;
            }
        }
        return false;
    }
    
    bool people_below(int j) {
        for (int i = this->floors[j]; i >= 0; i-- ) {
            if (this->waiting_in_elevator[j][i] > 0 || this->queue_down[i] > 0 ) {
                return true;
            }
        }
        return false;
    }
    
    
    // feature vector of environment
    // serves as input for neural network
    // for every elevator: sleep time, current floor, current movement, for every floor people in elevator that need to go there
    // for every floor: people wanting to go up
    // for every floor: people wanting to do down
    vector<double> get_feature_vector () {
        vector<double> feature_vector;
        int d;
        for (int i = 0; i < n_elevators; i ++) {
            d = 0;
            feature_vector.push_back(double(this->sleep[i]));
            feature_vector.push_back(double(this->current_movement[i]));
            feature_vector.push_back(double( 5 - this->floors[i]));
            for (int j = 0; j <= n_floors; j++) {
                feature_vector.push_back(double(this->waiting_in_elevator[i][j]));
            }
        }
        for (int j = 0; j <= n_floors; j++) {
            feature_vector.push_back(double((n_floors - j) * this->queue_up[j]));
        }
        for (int j = 0; j <= n_floors; j++) {
            feature_vector.push_back(double(- j * this->queue_down[j]));
        }
    
        // sigmoid function to avoid adjoint blow ups
      //  this->sigmoid(feature_vector);
        return feature_vector;
    }
    
    void sigmoid(vector<double>& v) {
        for (int i = 0; i < v.size(); i++) {
            v[i] = 1.0 / (1.0 + exp(-v[i]));
        }
    }
    
    // compute value function as output of NN
    double value_function() {
        
        vector<double> inputs = this->get_feature_vector();
        vector<vector<double>> w_weights;
        vector<vector<double>> b_weights;
        vector<double> w_temp_vec;
        vector<double> b_temp_vec;
        for (int i = 0; i < N; i++) {
            w_temp_vec.clear();
            b_temp_vec.clear();
            for (int j = 0; j < 10; j++) {
                w_temp_vec.push_back(this->state_value_w_weights[i][j]);
                b_temp_vec.push_back(this->state_value_b_weights[i][j]);
            }
            w_weights.push_back(w_temp_vec);
            b_weights.push_back(b_temp_vec);
        }
        this->network.remake(inputs, w_weights, b_weights);
        double value = this->network.evaluate(this->network.end_node);
        
        return value;
        
    }
    
    // propagate adjoints to get all gradients of weights
    pair<vector<vector<double>>,vector<vector<double>>> propagate() {
        
        vector<double> w_temp_vec;
        vector<double> b_temp_vec;
        
        vector<vector<double>> w_adjoints;
        vector<vector<double>> b_adjoints;
        
        this->network.propagate_adjoints(this->network.end_node);
        for (int i = 0; i < this->network.w_weight_nodes.size(); i++) {
            w_temp_vec.clear();
            b_temp_vec.clear();
            for (int j = 0; j < 10; j ++) {
                w_temp_vec.push_back(this->network.w_weight_nodes[i][j]->myAdjoint);
                b_temp_vec.push_back(this->network.b_weight_nodes[i][j]->myAdjoint);
           //     std::cout << " adjoint " << this->network.b_weight_nodes[i][j]->myAdjoint;
            }
            w_adjoints.push_back(w_temp_vec);
            b_adjoints.push_back(b_temp_vec);
        }
        
        return make_pair(w_adjoints, b_adjoints);
        
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
