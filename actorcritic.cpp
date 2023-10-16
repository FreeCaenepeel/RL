#include "environment.cpp"
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


class ActorCritic {
public:
    Environment env;
    RNG rng;
    NN network;
    double policy_weights[2*W];                 // weights of neural network
    double learning_rate = 0.001;
    double average_reward;
    int reward;
    double learning_rate_reward = 0.01;         // learning rate for reward
    
    ActorCritic (vector<double> inputs, vector<pair<double,double>> weights, vector<double> env_inputs, vector<vector<double>> env_w_weights, vector<vector<double>> env_b_weights):
        env(env_inputs, env_w_weights, env_b_weights),
        network(inputs, weights)
    {
        average_reward = 0.0;
        reward = 0;
        
        for (int i = 0; i < W ; i ++) {
            policy_weights[2*i] = weights[i].first;
            policy_weights[2*i+1] = weights[i].second;
        }
    
    }
    
    // based on current state of environment
    // calculate pi(a|s) for all actions
    vector<double> calculate_probabilities () {
        vector<double> probs;
        vector<pair<double,double>> weights;
        for (int i = 0; i < W; i++) {
            weights.push_back(make_pair(this->policy_weights[2*i],this->policy_weights[2*i+1]));
        }
        
        vector<double> inputs = this->env.get_feature_vector();
        this->network.remake(inputs, weights);
        this->network.evaluate(this->network.end_node);
        for (int i = 0; i < this->network.probability_end_nodes.size(); i++) {
            probs.push_back(this->network.probability_end_nodes[i]->result());
        }
    
        return probs;
        
    }
    
    // given probability distribution, sample action to take
    int sample_action(vector<double> probs) {
        // calculate cdfs
        for (int i = 1; i < probs.size(); i ++) {
            probs[i] += probs[i-1];
        }
        // get random number between 0 and 1
        double r = this->rng.get_random(1);
        int index = 0;
        while (index < probs.size() && r > probs[index]) {
            index += 1;
        }
        return index;
        
    }
    
    // policy function: calculate probabilities, sample action, and possibly propagate adjoints
    pair<vector<int>, pair<vector<double>,vector<double>>> policy (bool propagate) {
        vector<double> probs;
        vector<double> adjoints;
        probs = this->calculate_probabilities();
        int index = this->sample_action(probs);
        vector<int> action;
        for (int i = 0; i < 1; i ++) {
            action.push_back(actions[index][i]);
        }
        
        // propagate the adjoints
        if (propagate) {
            this->network.propagate_adjoints(this->network.end_nodes[index]);
            for (int i = 0; i < this->network.weight_nodes.size(); i++) {
                adjoints.push_back(this->network.weight_nodes[i]->myAdjoint);
            }
        }
        return make_pair(action, make_pair(probs,adjoints));
        
    }
    

    void run_one_step_of_generated_queue(int timestamp, int floor, bool direction, vector<double>& probabilities) {
        // add the new person to the correct queue
        if (direction) { // up
            this->env.queue_up[floor] = this->env.queue_up[floor] + 1;
        }
        else {
            this->env.queue_down[floor] = this->env.queue_down[floor] + 1;
        }
        vector<int> action;
        vector<double> adjoints;
        vector<vector<double>> w_adjoints;
        vector<vector<double>> b_adjoints;
        pair<vector<double>,vector<double>> placeholder;
        
        vector<double> state_feature_vector;
        vector<double> policy_feature_vector;
        double value_old_state;
        int reward;
        double value_new_state;
        double temporal_difference;
        
        
        // run for timestamp number of times
        for (int i = 0; i <= timestamp; i++) {
            tie(action, placeholder) = this->policy(true);
            tie(probabilities,adjoints) = placeholder;

            state_feature_vector = this->env.get_feature_vector();
            value_old_state = this->env.value_function();
            reward = this->env.next_state(action);
            this->reward += reward;
            value_new_state = this->env.value_function();
            temporal_difference = reward - this->average_reward + value_new_state - value_old_state;
            this->average_reward += this->learning_rate_reward * temporal_difference;

            // update state value weights
            tie(w_adjoints, b_adjoints) = this->env.propagate();
            for (int i = 0; i < N; i ++) {
                for (int j = 0; j < 10; j++) {
                    this->env.state_value_w_weights[i][j] += this->env.learning_rate_for_state_value * temporal_difference * w_adjoints[i][j];
                    this->env.state_value_b_weights[i][j] += this->env.learning_rate_for_state_value * temporal_difference * b_adjoints[i][j];
                }
            }
            
            // update policy weights
            for (int i = 0; i < 2*W ; i ++) {
                this->policy_weights[i] = this->learning_rate * temporal_difference * this->network.weight_nodes[i]->myAdjoint + 1.0 * this->policy_weights[i];
            }
            
         //   std::cout << "floor" << this->env.floors[0] << endl;
         //   std::cout << "action " << action[0] << endl;
         //   std::cout << "probs " << probabilities[0] << " " << probabilities[1] << " " << probabilities[2] << endl;
         //   std::cout << endl;
            
        }
        
        
    }
    
    void evaluate_one_step_of_generated_queue(int timestamp, int floor, bool direction, vector<double>& probabilities) {
        // add the new person to the correct queue
        if (direction) { // up
            this->env.queue_up[floor] = this->env.queue_up[floor] + 1;
        }
        else {
            this->env.queue_down[floor] = this->env.queue_down[floor] + 1;
        }
        vector<int> action;
        pair<vector<double>,vector<double>> placeholder;
        vector<double> adjoints;
        for (int i = 0; i <= timestamp; i++) {

            tie(action, placeholder) = this->policy(false);
            tie(probabilities, adjoints) = placeholder;
            std::cout << "floor " << this->env.floors[0] << "and action " << action[0] << endl;
            std::cout << " probs " << probabilities[0] << " " << probabilities[1] << " " << probabilities[2] << endl;
            this->reward += this->env.next_state(action);
        //    this->env.pretty_print();
            
        }
    
        
    }
};
