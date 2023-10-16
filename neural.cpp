
#include <stdio.h>

#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <math.h>

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


// for one elevator. Size of feature vector = number of input nodes of NN
const int N1 = 1*(10+4) + 2 * 10 + 2;

using namespace std;


class Node
{
        
protected:
    

 public:
    vector<Node*> myArguments;
    double myResult;
    double myAdjoint = 0.0;
    bool visited = false;
    
    virtual ~Node() {}

    //  Access result
    double result()
    {
        return myResult;
    }
    //  Access adjoint
    double& adjoint()
    {
        return myAdjoint;
    }
    void resetAdjoints()
    {
        for (auto argument : myArguments) argument->resetAdjoints();
        myAdjoint = 0.0;
    }
    virtual void propagateAdjoint() = 0;
    virtual void evaluate() = 0;
    

    
};
           
class PlusNode : public Node
{
public:
    PlusNode(Node* lhs, Node* rhs)
    {
        myArguments.resize(2);
        myArguments[0] = lhs;
        myArguments[1] = rhs;
        myResult = lhs->result() + rhs->result();
    }
    void propagateAdjoint() override
    {
        myArguments[0]->adjoint() += myAdjoint;
        myArguments[1]->adjoint() += myAdjoint;
    }
    
    void evaluate() override {
        myResult = myArguments[0]->result() + myArguments[1]->result();
    }
};

class TimesNode : public Node
{
public:
    TimesNode(Node* lhs, Node* rhs)
    {
        myArguments.resize(2);
        myArguments[0] = lhs;
        myArguments[1] = rhs;
        myResult = lhs->result() * rhs->result();
    }
    void propagateAdjoint() override
    {
        myArguments[0]->adjoint() += myAdjoint * myArguments[1]->result();
        myArguments[1]->adjoint() += myAdjoint * myArguments[0]->result();
    }
    
    void evaluate() override {
        myResult = myArguments[0]->result() * myArguments[1]->result();
    }
};

class DivisionNode : public Node
{
public:
    DivisionNode(Node* lhs, Node* rhs)
    {
        myArguments.resize(2);
        myArguments[0] = lhs;
        myArguments[1] = rhs;
        myResult = lhs->result() / rhs->result();
    }
    void propagateAdjoint() override
    {
        myArguments[0]->adjoint() += myAdjoint / myArguments[1]->result();
        myArguments[1]->adjoint() += myAdjoint * (-myArguments[0]->result()) / myArguments[1]->result() / myArguments[1]->result() ;
    }
    
    void evaluate() override {
        myResult = myArguments[0]->result() / myArguments[1]->result();
    }
};

class LogNode : public Node
{
public:
    LogNode(Node* arg)
    {
        myArguments.resize(1);
        myArguments[0] = arg;
        myResult = log(arg->result());
    }
    
    void propagateAdjoint() override
        {
            myArguments[0]->adjoint() += myAdjoint / myArguments[0]->result();
        }
    
    void evaluate() override {
        myResult = log(myArguments[0]->result());
    }
};


class SigmoidNode : public Node
{
public:
    SigmoidNode(Node* arg)
    {
        myArguments.resize(1);
        myArguments[0] = arg;
        myResult = 1.0 / (1.0 + exp(-arg->result()));
    }
    
    void propagateAdjoint() override
        {
            myArguments[0]->adjoint() += myAdjoint * (1.0 / (1.0 + exp(-myArguments[0]->result()))) * (1.0 - (1.0 / (1.0 + exp(-myArguments[0]->result()))));
        }
    
    void evaluate() override {
        myResult = 1.0 / (1.0 + exp(-myArguments[0]->result()));
    }
};

class ExpminusNode : public Node
{
public:
    ExpminusNode(Node* arg)
    {
        myArguments.resize(1);
        myArguments[0] = arg;
        myResult = exp(-arg->result());
    }
    
    void propagateAdjoint() override
        {
            myArguments[0]->adjoint() += myAdjoint * (-1.0) * exp(-myArguments[0]->result());
        }
    
    void evaluate() override {
        myResult = exp(-myArguments[0]->result());
    }
};


class Leaf: public Node
{
public:
    
    double myValue;
    Leaf(double val)
    {
        myValue = val;
    }
    double getVal()
    {
        return myValue;
    }
    void setVal(double val)
    {
        myValue = val;
    }
    void propagateAdjoint() override {}
    
    void evaluate() override {
        myResult = myValue;
    }
};

class Number
{
    
public:
    Node* myNode;
    Number(double val)
        : myNode(new Leaf(val))
    {
}
    Number(Node* node)
        : myNode(node) {}
    Node* node()
    {
        return myNode;
    }
    
    Number operator+(Number rhs)
    {
     // Create node: note eagerly computes result
     Node* n = new PlusNode(this->node(), rhs.node());
     // Return result
     return n;
    }
    
    Number operator*(Number rhs)
    {
        //  Create node: note eagerly computes result
        Node* n = new TimesNode(this->node(), rhs.node());
        // Return result
        return n;
    }
    
    Number operator/(Number rhs)
    {
        //  Create node: note eagerly computes result
        Node* n = new DivisionNode(this->node(), rhs.node());
        // Return result
        return n;
    }

    Number log()
    {
        // Create node: note eagerly computes result
        Node* n = new LogNode(this->node());
        // Return result
        return n;
    }
    
    Number sigmoid()
    {
        // Create node: note eagerly computes result
        Node* n = new SigmoidNode(this->node());
        // Return result
        return n;
    }
    
    Number expminus()
    {
        // Create node: note eagerly computes result
        Node* n = new ExpminusNode(this->node());
        // Return result
        return n;
    }
};
        

class NN {
public:
    stack<Node*> traversal;
    int size;
    vector<Node*> weight_nodes;
    
    vector<Node*> probability_end_nodes;
    vector<Node*> end_nodes;
    Node* end_node;
    
    
    // construct the neural network for policy
    NN (vector<double> inputs, vector<pair<double,double>> weights) {
        // input layer contains size of feature vector nodes: n_elevators*(n_floors+4) + 2 * n_floors + 2
        // output later contains number of actions: n_elevator = 1, so 3
        // sigmoid layer
        // softmax
        // ln
        // aggregate to have one output node (doesn't affect adjoints)
        // number of w_weights: 3 * (n_elevators*(n_floors+4) + 2 * n_floors + 2)
        // number of b_weights: 3 * (n_elevators*(n_floors+4) + 2 * n_floors + 2)
        
        this->nn_function(inputs, weights);
    }
    
    void remake(vector<double> inputs, vector<pair<double,double>> weights) {
        // empty stack
        while (!this->traversal.empty()) {
            this->traversal.pop();
        }
        
        this->probability_end_nodes.clear();
        this->end_nodes.clear();
        this->weight_nodes.clear();
        
        this->nn_function(inputs, weights);
        
        
    }
    
    void nn_function(vector<double> inputs, vector<pair<double,double>> weights) {
        
        vector<Number> nodes;
        for (int i = 0; i < inputs.size(); i++) {
            nodes.push_back(Number(inputs[i]));
        }
        for (int i = 0; i < weights.size(); i++) {
            nodes.push_back(Number(weights[i].first));
            nodes.push_back(Number(weights[i].second));
            this->weight_nodes.push_back(nodes[int(inputs.size()) + 2*i].myNode);
            this->weight_nodes.push_back(nodes[int(inputs.size()) + 2*i + 1].myNode);
        }
        
        // output node 1
        Number y0 = nodes[0] * nodes[N1 + 2*0] + nodes[N1 + 2*0 + 1];
        for (int i = 1; i < N1; i++) {
            y0 = y0 + nodes[i] * nodes[N1 + 2*i] + nodes[N1 + 2*i + 1];
        }
        
        // output node 2
        Number y1 = nodes[0] * nodes[3*N1 + 0] + nodes[3*N1 + 1];
        for (int i = 1; i < N1; i++) {
            y1 = y1 + nodes[N1 + i] * nodes[3*N1 + 2*i] + nodes[3*N1 + 2*i + 1];
        }
        
        
        // output node 3
        Number y2 = nodes[0] * nodes[5*N1 + 0] + nodes[5*N1 + 1];
        for (int i = 1; i < N1; i++) {
            y2 = y2 + nodes[i] * nodes[5*N1 + 2*i] + nodes[5*N1 + 2*i + 1];
        }
        
        
        // sigmoid
        Number z0 = y0.sigmoid();
        Number z1 = y1.sigmoid();
        Number z2 = y2.sigmoid();
        
        // softmax
        Number w0 = z0.expminus();
        Number w1 = z1.expminus();
        Number w2 = z2.expminus();
        Number w = w0 + w1 + w2;
        Number e0 = w0 / w;
        Number e1 = w1 / w;
        Number e2 = w2 / w;

        Number f0 = e0.log();
        Number f1 = e1.log();
        Number f2 = e2.log();
        

        this->probability_end_nodes.push_back(e0.myNode);
        this->probability_end_nodes.push_back(e1.myNode);
        this->probability_end_nodes.push_back(e2.myNode);
        
        this->end_nodes.push_back(f0.myNode);
        this->end_nodes.push_back(f1.myNode);
        this->end_nodes.push_back(f2.myNode);
        
        Number y = f0 + f1 + f2;
        this->end_node = y.myNode;
        
    }
    
    // evaluate using DFS and produce the stack
    double evaluate(Node* n) {
        if (n->visited == false) {
            for (auto arg : n->myArguments) {
                this->evaluate(arg);
            }
            n->evaluate();
            
            this->traversal.push(n);
            n->visited = true;
        }
        return this->end_node->result();
    }
    
    void propagate_adjoints(Node* start_node) {
        Node* n = this->traversal.top();
        while (n != start_node) {
            this->traversal.pop();
            n = traversal.top();
        }
        n->myAdjoint = 1.0;
        n->propagateAdjoint();
        this->traversal.pop();
        while ( !this->traversal.empty()) {
            n = this->traversal.top();
            this->traversal.pop();
            n->propagateAdjoint();
        }
        
    }
    
};


class NNStateValue {
public:
    stack<Node*> traversal;
    int size;
    vector<vector<Node*>> w_weight_nodes;
    vector<vector<Node*>> b_weight_nodes;
    Node* end_node;
    
    
    // construct the neural network for state value function
    NNStateValue (vector<double> inputs, vector<vector<double>> w_weights, vector<vector<double>> b_weights) {
        
        // input layer contains length of feature vector of environment: n_elevators*(n_floors+4) + 2 * n_floors + 2
        // hidden layer contains 10 nodes
        // sigmoid layer
        // sum aggregate
        // number of w_weights: (n_elevators*(n_floors+4) + 2 * n_floors + 2) * 10
        // number of b_weights: (n_elevators*(n_floors+4) + 2 * n_floors + 2) * 10
        
        this->nn_function(inputs, w_weights, b_weights);
    }
    
    void remake(vector<double> inputs, vector<vector<double>> w_weights, vector<vector<double>> b_weights) {
        // empty stack
        while (!this->traversal.empty()) {
            this->traversal.pop();
        }
        
        this->w_weight_nodes.clear();
        this->b_weight_nodes.clear();
        
        this->nn_function(inputs, w_weights, b_weights);
        
    }
    
    void nn_function(vector<double> inputs, vector<vector<double>> w_weights, vector<vector<double>> b_weights) {
        vector<Number> input_nodes;
        vector<vector<Number>> w_weight_nodes;
        vector<vector<Number>> b_weight_nodes;
        vector<Number> temp_vec;
        vector<Node*> temp_vec2;
        
        for (int i = 0; i < inputs.size(); i++) {
            input_nodes.push_back(Number(inputs[i]));
        }
        for (int i = 0; i < w_weights.size(); i++) {
            temp_vec.clear();
            temp_vec2.clear();
            for (int j = 0; j < w_weights[i].size(); j++) {
                temp_vec.push_back(w_weights[i][j]);
                temp_vec2.push_back(temp_vec[j].myNode);
            }
            w_weight_nodes.push_back(temp_vec);
            this->w_weight_nodes.push_back(temp_vec2);
            
        }
        for (int i = 0; i < b_weights.size(); i++) {
            temp_vec.clear();
            temp_vec2.clear();
            for (int j = 0; j < b_weights[i].size(); j++) {
                temp_vec.push_back(b_weights[i][j]);
                temp_vec2.push_back(temp_vec[j].myNode);
            }
            b_weight_nodes.push_back(temp_vec);
            this->b_weight_nodes.push_back(temp_vec2);

        }
                    
        
        // hidden layer node 1
        Number y0 = input_nodes[0] * w_weight_nodes[0][0] + b_weight_nodes[0][0];
        for (int i = 1; i < N1; i++) {
            y0 = y0 + input_nodes[i] * w_weight_nodes[i][0] + b_weight_nodes[i][0];;
        }
        
        // hidden layer node 2
        Number y1 = input_nodes[0] * w_weight_nodes[0][1] + b_weight_nodes[0][1];
        for (int i = 1; i < N1; i++) {
            y1 = y1 + input_nodes[i] * w_weight_nodes[i][1] + b_weight_nodes[i][1];;
        }
        
        // hidden layer node 3
        Number y2 = input_nodes[0] * w_weight_nodes[0][2] + b_weight_nodes[0][2];
        for (int i = 1; i < N1; i++) {
            y2 = y2 + input_nodes[i] * w_weight_nodes[i][2] + b_weight_nodes[i][2];;
        }
        
        // hidden layer node 4
        Number y3 = input_nodes[0] * w_weight_nodes[0][3] + b_weight_nodes[0][3];
        for (int i = 1; i < N1; i++) {
            y3 = y3 + input_nodes[i] * w_weight_nodes[i][3] + b_weight_nodes[i][3];;
        }
        
        // hidden layer node 5
        Number y4 = input_nodes[0] * w_weight_nodes[0][4] + b_weight_nodes[0][4];
        for (int i = 1; i < N1; i++) {
            y4 = y4 + input_nodes[i] * w_weight_nodes[i][4] + b_weight_nodes[i][4];;
        }
        
        // hidden layer node 6
        Number y5 = input_nodes[0] * w_weight_nodes[0][5] + b_weight_nodes[0][5];
        for (int i = 1; i < N1; i++) {
            y5 = y5 + input_nodes[i] * w_weight_nodes[i][5] + b_weight_nodes[i][5];;
        }
        
        // hidden layer node 7
        Number y6 = input_nodes[0] * w_weight_nodes[0][6] + b_weight_nodes[0][6];
        for (int i = 1; i < N1; i++) {
            y6 = y6 + input_nodes[i] * w_weight_nodes[i][6] + b_weight_nodes[i][6];;
        }
        
        // hidden layer node 8
        Number y7 = input_nodes[0] * w_weight_nodes[0][7] + b_weight_nodes[0][7];
        for (int i = 1; i < N1; i++) {
            y7 = y7 + input_nodes[i] * w_weight_nodes[i][7] + b_weight_nodes[i][7];;
        }
        
        // hidden layer node 9
        Number y8 = input_nodes[0] * w_weight_nodes[0][8] + b_weight_nodes[0][8];
        for (int i = 1; i < N1; i++) {
            y8 = y8 + input_nodes[i] * w_weight_nodes[i][8] + b_weight_nodes[i][8];;
        }
        
        // hidden layer node 10
        Number y9 = input_nodes[0] * w_weight_nodes[0][9] + b_weight_nodes[0][9];
        for (int i = 1; i < N1; i++) {
            y9 = y9 + input_nodes[i] * w_weight_nodes[i][9] + b_weight_nodes[i][9];;
        }
        
        // sigmoid
        Number z0 = y0.sigmoid();
        Number z1 = y1.sigmoid();
        Number z2 = y2.sigmoid();
        Number z3 = y3.sigmoid();
        Number z4 = y4.sigmoid();
        Number z5 = y5.sigmoid();
        Number z6 = y6.sigmoid();
        Number z7 = y7.sigmoid();
        Number z8 = y8.sigmoid();
        Number z9 = y9.sigmoid();
        
        Number y = z0 + z1 + z2 + z3 + z4 + z5 + z6 + z7 + z8 + z9;
        this->end_node = y.myNode;
    }
    
    // evaluate using DFS and produce the stack
    double evaluate(Node* n) {
        if (n->visited == false) {
            for (auto arg : n->myArguments) {
                this->evaluate(arg);
            }
            n->evaluate();
            
            this->traversal.push(n);
            n->visited = true;
        }
        return this->end_node->result();
    }
    
    void propagate_adjoints(Node* start_node) {
        Node* n = this->traversal.top();
        while (n != start_node) {
            this->traversal.pop();
            n = traversal.top();
        }
        n->myAdjoint = 1.0;
        n->propagateAdjoint();
        this->traversal.pop();
        while ( !this->traversal.empty()) {
            n = this->traversal.top();
            this->traversal.pop();
            n->propagateAdjoint();
        }
        
    }

};
