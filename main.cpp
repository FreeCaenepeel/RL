#include <stdio.h>

#include "actorcritic.cpp"

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



void generate_queue(string title, int amount) {
    fstream my_file;
    
    my_file.open(title, ios::out);
    if (!my_file) {
        cout << "File not created!";
    }
    else {
        cout << "File successfully created!";
        
        // generate queue
        RNG* rng1 = new RNG();
        RNG* rng2 = new RNG();
        RNG* rng3 = new RNG();
        vector<pair<pair<int,int>,bool>> queue;
        int timestamp;
        int floor;
        bool up;
        for (int i =0; i < amount; i++) {
            timestamp = rng1->get_random_integer(8);  // 12 too high
            floor = rng2->get_random_integer(n_floors);
            if (rng3->get_random_integer(1) == 1) {
                up = true;
            }
            else {up = false;}
            if (floor == n_floors) {up = false;}
            if (floor == 0) {up = true;}
            
            queue.push_back(make_pair(make_pair(timestamp, floor), up));
            if (i%50 == 0) {
                queue.push_back(make_pair(make_pair(-1, floor), 0));
            }
            
            
        }
        
        for (int i =0; i < amount; i ++) {
            my_file << queue[i].first.first << " " << queue[i].first.second << " " << queue[i].second << endl;
        }
        my_file.close();
    }
}

int main() {
    
    // generate queue to train and evaluate
    generate_queue("queue.txt", 20000);
   // generate_queue("queueEval.txt", 1000);
    
    
    fstream file;
    file.open("queue.txt", ios::in);
    // train actor critic
    int timestamp;
    int floor;
    int direction;
    
    
    vector<vector<double>> w_weights;
    vector<vector<double>> b_weights;
    vector<double> w_temp_vec;
    vector<double> b_temp_vec;
    vector<double> first_input;
    
    // initialize all weights to 0
    for (int j = 0; j < 10; j++) {
        w_temp_vec.push_back(0.0);
        b_temp_vec.push_back(0.0);
    }
    
    for (int i = 0; i < N; i++) {
        w_weights.push_back(w_temp_vec);
        b_weights.push_back(b_temp_vec);
        first_input.push_back(0.5);
    }
    
    // weights after 50k training steps
    
    double state_value_w_weights[N][10] = {18.9176, 18.9176, 18.9176, 18.9176, 18.9176, 18.9176, 18.9176, 18.9176, 18.9176, 18.9176, 43.4605, 43.4605, 43.4605, 43.4605, 43.4605, 43.4605, 43.4605, 43.4605, 43.4605, 43.4605, -1.35653, -1.35653, -1.35653, -1.35653, -1.35653, -1.35653, -1.35653, -1.35653, -1.35653, -1.35653, -0.736581, -0.736581, -0.736581, -0.736581, -0.736581, -0.736581, -0.736581, -0.736581, -0.736581, -0.736581, 6.54116, 6.54116, 6.54116, 6.54116, 6.54116, 6.54116, 6.54116, 6.54116, 6.54116, 6.54116, 8.03104, 8.03104, 8.03104, 8.03104, 8.03104, 8.03104, 8.03104, 8.03104, 8.03104, 8.03104, 8.51643, 8.51643, 8.51643, 8.51643, 8.51643, 8.51643, 8.51643, 8.51643, 8.51643, 8.51643, 6.63793, 6.63793, 6.63793, 6.63793, 6.63793, 6.63793, 6.63793, 6.63793, 6.63793, 6.63793, 0.436924, 0.436924, 0.436924, 0.436924, 0.436924, 0.436924, 0.436924, 0.436924, 0.436924, 0.436924, 3.80713, 3.80713, 3.80713, 3.80713, 3.80713, 3.80713, 3.80713, 3.80713, 3.80713, 3.80713, 6.24424, 6.24424, 6.24424, 6.24424, 6.24424, 6.24424, 6.24424, 6.24424, 6.24424, 6.24424, 9.08692, 9.08692, 9.08692, 9.08692, 9.08692, 9.08692, 9.08692, 9.08692, 9.08692, 9.08692, 10.787, 10.787, 10.787, 10.787, 10.787, 10.787, 10.787, 10.787, 10.787, 10.787, 17.4574, 17.4574, 17.4574, 17.4574, 17.4574, 17.4574, 17.4574, 17.4574, 17.4574, 17.4574, -48.602, -48.602, -48.602, -48.602, -48.602, -48.602, -48.602, -48.602, -48.602, -48.602, 38.8045, 38.8045, 38.8045, 38.8045, 38.8045, 38.8045, 38.8045, 38.8045, 38.8045, 38.8045, 57.2307, 57.2307, 57.2307, 57.2307, 57.2307, 57.2307, 57.2307, 57.2307, 57.2307, 57.2307, 49.7028, 49.7028, 49.7028, 49.7028, 49.7028, 49.7028, 49.7028, 49.7028, 49.7028, 49.7028, -15.2485, -15.2485, -15.2485, -15.2485, -15.2485, -15.2485, -15.2485, -15.2485, -15.2485, -15.2485, 33.4736, 33.4736, 33.4736, 33.4736, 33.4736, 33.4736, 33.4736, 33.4736, 33.4736, 33.4736, 17.3056, 17.3056, 17.3056, 17.3056, 17.3056, 17.3056, 17.3056, 17.3056, 17.3056, 17.3056, 6.47477, 6.47477, 6.47477, 6.47477, 6.47477, 6.47477, 6.47477, 6.47477, 6.47477, 6.47477, -0.865701, -0.865701, -0.865701, -0.865701, -0.865701, -0.865701, -0.865701, -0.865701, -0.865701, -0.865701, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4.40222, 4.40222, 4.40222, 4.40222, 4.40222, 4.40222, 4.40222, 4.40222, 4.40222, 4.40222, -17.5173, -17.5173, -17.5173, -17.5173, -17.5173, -17.5173, -17.5173, -17.5173, -17.5173, -17.5173, 1.64922, 1.64922, 1.64922, 1.64922, 1.64922, 1.64922, 1.64922, 1.64922, 1.64922, 1.64922, -33.2385, -33.2385, -33.2385, -33.2385, -33.2385, -33.2385, -33.2385, -33.2385, -33.2385, -33.2385, -19.5801, -19.5801, -19.5801, -19.5801, -19.5801, -19.5801, -19.5801, -19.5801, -19.5801, -19.5801, -21.6027, -21.6027, -21.6027, -21.6027, -21.6027, -21.6027, -21.6027, -21.6027, -21.6027, -21.6027, -23.9744, -23.9744, -23.9744, -23.9744, -23.9744, -23.9744, -23.9744, -23.9744, -23.9744, -23.9744, -39.1419, -39.1419, -39.1419, -39.1419, -39.1419, -39.1419, -39.1419, -39.1419, -39.1419, -39.1419, -10.0636, -10.0636, -10.0636, -10.0636, -10.0636, -10.0636, -10.0636, -10.0636, -10.0636, -10.0636, -150.779, -150.779, -150.779, -150.779, -150.779, -150.779, -150.779, -150.779, -150.779, -150.779};
    
    double state_value_b_weights[N][10] = {2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891, 2.69891};
    
    double policy_weights[2*W] = {0.289997, 0.165633, 0.0802493, 0.165633, 0.0334373, 0.165633, 0.00641678, 0.165633, 0.0298964, 0.165633, 0.0142071, 0.165633, 0.00209213, 0.165633, 0.0147269, 0.165633, 0.00800004, 0.165633, 0.0110052, 0.165633, 0.00941952, 0.165633, 0.015739, 0.165633, 0.0103674, 0.165633, 0.169181, 0.165633, 0.218225, 0.165633, 0.145951, 0.165633, 0.111469, 0.165633, 0.12009, 0.18156, 0.0972547, 0.18156, 0.09507, 0.18156, 0.0773179, 0.18156, 0.0733093, 0.18156, 0.0487173, 0.18156, 0, 0.18156, 0, 0.18156, -0.0536657, 0.18156, -0.0257573, 0.18156, -0.0983481, 0.18156, -0.0503755, 0.18156, -0.0724231, 0.18156, -0.157066, 0.18156, -0.0765627, 0.18156, -0.209102, 0.18156, -0.223537, 0.18156, -0.196185, 0.18156, -9.25034, 0.140405, 0.0114661, 0.140405, 0.00591798, 0.140405, 0.0114661, 0.140405, 0.00316656, 0.140405, 0.0114661, 0.140405, 0.000556548, 0.140405, 0.0114661, 0.140405, 0.00253206, 0.140405, 0.0114661, 0.140405, 0.0013134, 0.140405, 0.0114661, 0.140405, 0.000272917, 0.140405, 0.0114661, 0.140405, 0.00166572, 0.140405, 0.0114661, 0.140405, 0.000954001, 0.140405, 0.0114661, 0.140405, 0.000670581, 0.140405, 0.0114661, 0.140405, 0.000511311, 0.140405, 0.0114661, 0.140405, 0.00154184, 0.140405, 0.0114661, 0.140405, 0.000694209, 0.140405, 0.0114661, 0.140405, 0.012859, 0.140405, 0.0114661, 0.140405, 0.0271304, 0.140405, 0.0114661, 0.140405, 0.0148578, 0.140405, 0.0114661, 0.140405, 0.00820723, 0.140405, 0.0114661, 0.140405, 0.00862847, 0.140405, -3.86778, -0.540036, 9.99182, -0.540036, -0.748929, -0.540036, -0.61184, -0.540036, -0.305566, -0.540036, -0.605323, -0.540036, -0.27685, -0.540036, 0.202495, -0.540036, -0.147186, -0.540036, 0.094613, -0.540036, 0.0632845, -0.540036, 0.21763, -0.540036, 0.953994, -0.540036, 0.674295, -0.540036, -0.0130906, -0.540036, -0.431285, -0.540036, -0.30677, -0.540036, 0.200164, -0.540036, 0.64939, -0.540036, 1.63523, -0.540036, 1.31867, -0.540036, 1.2951, -0.540036, 0.5455, -0.540036, 0, -0.540036, 0, -0.540036, 1.02855, -0.540036, 1.03364, -0.540036, 0.442128, -0.540036, 0.151213, -0.540036, -1.7875, -0.540036, -1.79139, -0.540036, 0.848791, -0.540036, -1.77188, -0.540036, -1.45359, -0.540036, -2.41264, -0.540036};
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < 10; j++) {
            w_weights[i][j] = state_value_w_weights[i][j];
            b_weights[i][j] = state_value_b_weights[i][j];
                         
        }
    }
    
    
    Environment* test_env = new Environment(first_input, w_weights, b_weights);
    vector<double> inputs = test_env->get_feature_vector();
    vector<pair<double,double>> weights;
    for (int i = 0; i < W; i++) {
        weights.push_back(make_pair(0.0,0.0));
    }
    
    for (int i = 0; i < W; i++) {
        weights[i] = make_pair(policy_weights[2*i],policy_weights[2*i+1]);
    }
    

    ActorCritic* actor = new ActorCritic(inputs, weights, inputs, w_weights, b_weights);
    
    vector<double> probabilities;
    vector<double> adjoints;
    probabilities = actor->calculate_probabilities();
    
    bool up;
    int counter = -1;
    int total_timestamp = 0;
    
    
    int checker = 1;
    while (1) {
        counter += 1;
        if (counter == checker * 100) {
            checker += 1;
            std::cout << "step " << counter << endl;
        }
        if (file.eof()) {break;}
        file >> timestamp;
        file >> floor;
        file >> direction;
        total_timestamp += timestamp;
        up = (direction == 1)? true : false;
        if (timestamp == -1) {
            std::cout << "reset and dropped of total " << actor->env.passengers_dropped_off << "\n";
            std::cout << "new floor " << floor << endl;
            actor->env.reset_state(floor, 0, 0);
           
            probabilities = actor->calculate_probabilities();
            for (int i = 0; i < 3; i++) {
                    std::cout << probabilities[i] << " ";
                }
            std::cout<<endl;
        }
        else {
            actor->run_one_step_of_generated_queue(timestamp, floor, direction, probabilities);
        }
    }

    
    std::cout << "passengers dropped of " << actor->env.passengers_dropped_off << endl;
    std::cout << "passengers waiting" << endl;
    for (int i = 0; i < n_elevators; i ++) {
        for (int j =0; j <= n_floors; j++) {
            if (actor->env.waiting_in_elevator[i][j] > 0) {
                std::cout << "eleveator " << i << " and floor " << j << " : " << actor->env.waiting_in_elevator[i][j] << endl;
            }
        }
    }
    std::cout << endl;
    for (int j =0; j <= n_floors; j++) {
        std::cout << "people waiting at floor " << j << ": " << actor->env.queue_up[j] << " and " << actor->env.queue_down[j] << endl;
    }
    std::cout << endl;
    std::cout << "policy weights " << endl;
    for (int i = 0; i < 2*W; i++) {
        std::cout << " " << actor->policy_weights[i] << " ";
    }
    
    std::cout << endl;
    std::cout << "state value w weights " << endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < 10; j++) {
            std::cout << " " << actor->env.state_value_w_weights[i][j] << " ";
        }
      
    }
    
    std::cout << endl;
    std::cout << "state value b weights " << endl;
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < 10; j++) {
            std::cout << " " << actor->env.state_value_b_weights[i][j] << " ";
        }
      
    }

    
    
    
    int floors[1] = {0};
    int queue_up[11] = {0,0,0,0,10,10,10,0,10,0,0};
    int queue_down[11] = {0,0,0,0,0,0,0,0,0,0,0};
    int full[1] = {0};
    
    actor->env.set_state(floors, queue_up, queue_down,full);

    std::cout << endl;
    probabilities = actor->calculate_probabilities();
    std::cout << " check bottom floors, elevator at floor 0, people want to go up" << endl;
    for (int i = 0; i < 3; i++) {
        std::cout << probabilities[i] << " ";
    }
    
    
    std::cout << endl;
    
    int floors2[1] = {10};
    int queue_up2[11] = {0,0,0,0,0,0,0,0,0,0,0};
    int queue_down2[11] = {10,10,10,0,0,0,0,0,0,0,0};
    int full2[1] = {0};
    
    actor->env.set_state(floors2, queue_up2, queue_down2,full2);
    std::cout << endl;
    vector<double> probabilities1;
    probabilities1 = actor->calculate_probabilities();
    std::cout << " check top floors, elevator at floor 10, people want to go down" << endl;
    for (int i = 0; i < 3; i++) {
        std::cout << probabilities1[i] << " ";
    }
      
    std::cout << endl;
     
     // evaluate actor critic
     fstream file2;
     file2.open("queueEval.txt", ios::in);
     
     actor->reward = 0;
     actor->average_reward = 0;
     actor->env.passengers_dropped_off = 0;
     total_timestamp = 0;
    
    int floorsr[1] = {0};
    int queue_upr[11] = {0,0,0,0,0,0,0,0,0,0,0};
    int queue_downr[11] = {0,0,0,0,0,0,0,0,0,0,0};
    int fullr[1] = {0};
        
    actor->env.set_state(floorsr, queue_upr, queue_downr,fullr);
    probabilities = actor->calculate_probabilities();
    
     counter = -1;
     
     while (1) {
         counter += 1;
         if (file2.eof()) {break;}
         file2 >> timestamp;
         file2 >> floor;
         file2 >> direction;
         total_timestamp += timestamp;
         up = (direction == 1)? true : false;
         if (timestamp != -1) {
             actor->evaluate_one_step_of_generated_queue(timestamp, floor, direction, probabilities);
         }
     }
    
    std::cout << "passengers dropped of " << actor->env.passengers_dropped_off << endl;
    
    
    fstream file3;
    file3.open("queueEval.txt", ios::in);

    total_timestamp = 0;
    
    DeterministicAgent* deterministic_agent = new DeterministicAgent();
    
    counter = -1;
    
    while (1) {
        counter += 1;
        if (file3.eof()) {break;}
        file3 >> timestamp;
        file3 >> floor;
        file3 >> direction;
        total_timestamp += timestamp;
        up = (direction == 1)? true : false;
        if (timestamp != -1) {
            deterministic_agent->run_one_step_of_generated_queue(timestamp, floor, direction);
        }

    }
    std::cout << "passengers dropped of by deterministic agent " << deterministic_agent->passengers_dropped_off << endl;
    
     
    

    
    
}


