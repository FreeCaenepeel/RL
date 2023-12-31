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
    
    double state_value_w_weights[N][10] = {-1.28653, -1.28653, -1.28653, -1.28653, -1.28653, -1.28653, -1.28653, -1.28653, -1.28653, -1.28653, -0.370331, -0.370331, -0.370331, -0.370331, -0.370331, -0.370331, -0.370331, -0.370331, -0.370331, -0.370331, -2.97016, -2.97016, -2.97016, -2.97016, -2.97016, -2.97016, -2.97016, -2.97016, -2.97016, -2.97016, 0.00971273, 0.00971273, 0.00971273, 0.00971273, 0.00971273, 0.00971273, 0.00971273, 0.00971273, 0.00971273, 0.00971273, 0.0087007, 0.0087007, 0.0087007, 0.0087007, 0.0087007, 0.0087007, 0.0087007, 0.0087007, 0.0087007, 0.0087007, 0.00481273, 0.00481273, 0.00481273, 0.00481273, 0.00481273, 0.00481273, 0.00481273, 0.00481273, 0.00481273, 0.00481273, -0.436094, -0.436094, -0.436094, -0.436094, -0.436094, -0.436094, -0.436094, -0.436094, -0.436094, -0.436094, -0.00265349, -0.00265349, -0.00265349, -0.00265349, -0.00265349, -0.00265349, -0.00265349, -0.00265349, -0.00265349, -0.00265349, -0.00656238, -0.00656238, -0.00656238, -0.00656238, -0.00656238, -0.00656238, -0.00656238, -0.00656238, -0.00656238, -0.00656238, -0.269993, -0.269993, -0.269993, -0.269993, -0.269993, -0.269993, -0.269993, -0.269993, -0.269993, -0.269993, 0.0053054, 0.0053054, 0.0053054, 0.0053054, 0.0053054, 0.0053054, 0.0053054, 0.0053054, 0.0053054, 0.0053054, -0.43, -0.43, -0.43, -0.43, -0.43, -0.43, -0.43, -0.43, -0.43, -0.43, 0.00444151, 0.00444151, 0.00444151, 0.00444151, 0.00444151, 0.00444151, 0.00444151, 0.00444151, 0.00444151, 0.00444151, -0.000669367, -0.000669367, -0.000669367, -0.000669367, -0.000669367, -0.000669367, -0.000669367, -0.000669367, -0.000669367, -0.000669367, -0.275276, -0.275276, -0.275276, -0.275276, -0.275276, -0.275276, -0.275276, -0.275276, -0.275276, -0.275276, -1.88381, -1.88381, -1.88381, -1.88381, -1.88381, -1.88381, -1.88381, -1.88381, -1.88381, -1.88381, -5.09413, -5.09413, -5.09413, -5.09413, -5.09413, -5.09413, -5.09413, -5.09413, -5.09413, -5.09413, -3.04369, -3.04369, -3.04369, -3.04369, -3.04369, -3.04369, -3.04369, -3.04369, -3.04369, -3.04369, -0.487863, -0.487863, -0.487863, -0.487863, -0.487863, -0.487863, -0.487863, -0.487863, -0.487863, -0.487863, -0.668814, -0.668814, -0.668814, -0.668814, -0.668814, -0.668814, -0.668814, -0.668814, -0.668814, -0.668814, -2.64824, -2.64824, -2.64824, -2.64824, -2.64824, -2.64824, -2.64824, -2.64824, -2.64824, -2.64824, -1.23087, -1.23087, -1.23087, -1.23087, -1.23087, -1.23087, -1.23087, -1.23087, -1.23087, -1.23087, -2.27493, -2.27493, -2.27493, -2.27493, -2.27493, -2.27493, -2.27493, -2.27493, -2.27493, -2.27493, 0.194127, 0.194127, 0.194127, 0.194127, 0.194127, 0.194127, 0.194127, 0.194127, 0.194127, 0.194127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.42502, 0.42502, 0.42502, 0.42502, 0.42502, 0.42502, 0.42502, 0.42502, 0.42502, 0.42502, -0.397207, -0.397207, -0.397207, -0.397207, -0.397207, -0.397207, -0.397207, -0.397207, -0.397207, -0.397207, -0.341294, -0.341294, -0.341294, -0.341294, -0.341294, -0.341294, -0.341294, -0.341294, -0.341294, -0.341294, 0.610514, 0.610514, 0.610514, 0.610514, 0.610514, 0.610514, 0.610514, 0.610514, 0.610514, 0.610514, 0.0307214, 0.0307214, 0.0307214, 0.0307214, 0.0307214, 0.0307214, 0.0307214, 0.0307214, 0.0307214, 0.0307214, -1.84111, -1.84111, -1.84111, -1.84111, -1.84111, -1.84111, -1.84111, -1.84111, -1.84111, -1.84111, 2.93504, 2.93504, 2.93504, 2.93504, 2.93504, 2.93504, 2.93504, 2.93504, 2.93504, 2.93504, 0.0528359, 0.0528359, 0.0528359, 0.0528359, 0.0528359, 0.0528359, 0.0528359, 0.0528359, 0.0528359, 0.0528359, 3.82565, 3.82565, 3.82565, 3.82565, 3.82565, 3.82565, 3.82565, 3.82565, 3.82565, 3.82565, 2.0173, 2.0173, 2.0173, 2.0173, 2.0173, 2.0173, 2.0173, 2.0173, 2.0173, 2.0173 };
    
    double state_value_b_weights[N][10] = {-1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849, -1.0849};
    
    double policy_weights[2*W] = {0.532688, 0.171691, 0.0214848, 0.171691, 0.0693165, 0.171691, 0.0960569, 0.171691, 0.0758854, 0.171691, 0.0462332, 0.171691, 0.0521732, 0.171691, 0.0608428, 0.171691, 0.135199, 0.171691, 0.0492541, 0.171691, 0.0174203, 0.171691, -0.00559944, 0.171691, 0.00766702, 0.171691, 0.0304325, 0.171691, 0.267286, 0.171691, 0.0290086, 0.171691, 0.102333, 0.171691, 0.0177148, 0.171691, -0.0833983, 0.191932, 0.559701, 0.191932, -0.0274774, 0.191932, -0.0367757, 0.191932, 0.101763, 0.191932, 0.0519268, 0.191932, 0, 0.191932, 0, 0.191932, -0.0334986, 0.191932, -0.0972729, 0.191932, -0.0767274, 0.191932, -0.114922, 0.191932, -0.43954, 0.191932, -0.0909412, 0.191932, 0.0555251, 0.191932, 0.00671288, 0.191932, -0.22337, 0.191932, -0.31595, 0.191932, -8.92326, 0.104332, 0.0140395, 0.104332, 0.00251864, 0.104332, 0.0140395, 0.104332, 0.0144095, 0.104332, 0.0140395, 0.104332, 0.010436, 0.104332, 0.0140395, 0.104332, 0.00447391, 0.104332, 0.0140395, 0.104332, 0.0027281, 0.104332, 0.0140395, 0.104332, 0.00603898, 0.104332, 0.0140395, 0.104332, 0.00667527, 0.104332, 0.0140395, 0.104332, 0.0126154, 0.104332, 0.0140395, 0.104332, 0.00789403, 0.104332, 0.0140395, 0.104332, 0.00155718, 0.104332, 0.0140395, 0.104332, -0.00146276, 0.104332, 0.0140395, 0.104332, -0.000357577, 0.104332, 0.0140395, 0.104332, 0.00441851, 0.104332, 0.0140395, 0.104332, 0.032842, 0.104332, 0.0140395, 0.104332, -0.0229158, 0.104332, 0.0140395, 0.104332, 0.0151697, 0.104332, 0.0140395, 0.104332, -0.0129821, 0.104332, 0.0140395, 0.104332, -3.24809, -0.36402, -0.174423, -0.36402, 7.68103, -0.36402, -0.143791, -0.36402, -0.203034, -0.36402, -0.351207, -0.36402, -0.43769, -0.36402, -0.408899, -0.36402, -0.118271, -0.36402, 0.0949445, -0.36402, 0.474074, -0.36402, 0.0399021, -0.36402, 0.624393, -0.36402, 0.372651, -0.36402, 1.42107, -0.36402, -1.09532, -0.36402, 2.23237, -0.36402, 1.25894, -0.36402, 1.12264, -0.36402, 0.872886, -0.36402, 1.80738, -0.36402, 1.1449, -0.36402, 1.25718, -0.36402, 1.02014, -0.36402, 0, -0.36402, 0, -0.36402, 0.580516, -0.36402, -0.179347, -0.36402, 0.842881, -0.36402, -0.598001, -0.36402, -0.543941, -0.36402, 0.0616456, -0.36402, -1.29873, -0.36402, -2.05152, -0.36402, -2.93725, -0.36402, -2.93105, -0.36402};
    
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
    
    /*
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

    
    */
    
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


