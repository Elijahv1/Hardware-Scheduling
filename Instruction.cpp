#include "Instruction.h"
#include <iostream>
#include <string>
#include <sstream>

// Constructor
Instruction::Instruction(string inst_str) {

    // Separating name out
    stringstream in_string = stringstream(inst_str);
    getline(in_string, inst_name, ' ');
    
    // Loading or storing into floating point register
    cycle_time = 1;
    if (inst_name == LOAD || inst_name == STORE) {
        getline(in_string, dest_loc, ',');
        if (in_string.peek() == ' ') { in_string.ignore(); }
        getline(in_string, source_loc);
        second_source = "";
        cycle_time = INT_UNIT_DUR;
        operation = IN_ADD;
    } 

    // Other Operations, loading in variables
    else {
        getline(in_string, dest_loc, ',');
        if (in_string.peek() == ' ') { in_string.ignore(); }
        getline(in_string, source_loc, ',');
        if (in_string.peek() == ' ') { in_string.ignore(); }
        getline(in_string, second_source);
        if (inst_name == INT_ADD || inst_name == INT_SUB || inst_name == IMM_ADD) { 
            cycle_time = INT_UNIT_DUR;
            operation = IN_ADD;
        }
        else if (inst_name == FLT_ADD || inst_name == FLT_SUB) { 
            cycle_time = FLOAT_ADD_DUR; 
            operation = FP_ADD;
        }
        else if (inst_name == FLT_MUL) { 
            cycle_time = FLOAT_MUL_DUR;
            operation = FP_MUL;
        }
        else if (inst_name == FLT_DIV) { 
            cycle_time = FLOAT_DIV_DUR;
            operation = FP_DIV;
        }
    }
    in_operation = false;
    complete = false;
    inst_string = inst_str;
}

// Returns the index of the source
int Instruction::Source() {

    // Load and store have special ways of getting values
    if (inst_name == LOAD || inst_name == STORE) {

        stringstream num_from_string(source_loc);
        string offset, addr;

        getline(num_from_string, offset, '(');
        getline(num_from_string, addr, ')');
        return (stoi(offset) + stoi(addr));
    }

    // Converting ascii
    if (source_loc.size() == 2) { return source_loc[1] - 48; }
    if (source_loc.size() == 3) { return ((source_loc[1] - 48) * 10) + (source_loc[2] - 48); }
    return 0;
}

// Returns the index of the destination
int Instruction::Dest() { 

    // Converting ascii
    if (dest_loc.size() == 2) { return dest_loc[1] - 48; }
    if (dest_loc.size() == 3) { return (dest_loc[1] - 48) * 10 + dest_loc[2] - 48; }
    return 0;
}

// Returns the index of the second source (or value of immediate)
int Instruction::SecondSource() {

    // Load and store have no second source
    if (inst_name == LOAD || inst_name == STORE) {
        return -1;
    }

    // Second source treated as the immediate value
    if (inst_name == IMM_ADD) {
        return (stoi(second_source));
    }

    // Converting ascii
    else {
        if (second_source.size() == 2) { return second_source[1]-48; }
        if (second_source.size() == 3) { return (second_source[1]-48)*10 + second_source[2]-48; }
    }
    return 0;
}

// Returns if operation uses two sources
bool Instruction::twoSources() {

    // Only load, store and load immediate have less than 2 sources
    if (inst_name == LOAD || inst_name == STORE || inst_name == IMM_ADD) {
        return false;
    }
    return true;
}

// Setting if the instruction is complete or not
void Instruction::setComplete(bool comp) {
    complete = comp;
}


// Returning private variables
string Instruction::Name() { return inst_name; }
int Instruction::Time() { return cycle_time; }
OP_TYPE Instruction::OpType() { return operation;  }
bool Instruction::isOperating() { return in_operation; }
bool Instruction::isComplete() { return complete; }
string Instruction::instString() { return inst_string; }