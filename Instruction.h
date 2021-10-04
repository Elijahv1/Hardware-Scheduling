#pragma once

#include <string>
#include <vector>
#include <sstream>
using namespace std;

// Instruction types
const string LOAD = "L.D";
const string STORE = "S.D";
const string INT_ADD = "ADD";
const string IMM_ADD = "ADDI";
const string FLT_ADD = "ADD.D";
const string FLT_SUB = "SUB.D";
const string INT_SUB = "SUB";
const string FLT_MUL = "MUL.D";
const string FLT_DIV = "DIV.D";

// Instruction time in number of cycles
const int FLOAT_ADD_DUR = 2;
const int FLOAT_MUL_DUR = 10;
const int FLOAT_DIV_DUR = 40;
const int INT_UNIT_DUR = 1;

// Enum for Operation type
enum OP_TYPE { FP_ADD, FP_MUL, FP_DIV, IN_ADD };

// Instruction class
class Instruction {
public:

    // Must send string to create
    Instruction(string inst_str);

    // Returns the index of the source
    int Source();

    // Returns the index of the destination
    int Dest();

    // Returns the index of the second source (or value of immediate)
    int SecondSource();

    // Returns if operation uses two sources
    bool twoSources();

    // Setting complete
    void setComplete(bool comp);

    // Getting data from private variables
    string Name();
    int Time();
    OP_TYPE OpType();
    bool isOperating();
    bool isComplete();


    string instString();

private:
    string inst_name;
    string source_loc;
    string second_source;
    string dest_loc;
    int cycle_time;
    OP_TYPE operation;
    bool in_operation;
    bool complete;
    string inst_string;
};
