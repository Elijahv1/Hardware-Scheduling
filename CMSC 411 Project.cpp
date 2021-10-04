// CMSC 411 Project.cpp
// Author: Elijah Vaughnn
// Date: 12/5/20


#include "Instruction.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

// Initializing the memory and registers
vector<int> memory{ 45, 12, 0, 0, 10, 135, 254, 127, 18, 4, 55, 8, 2, 98, 13, 5, 233, 158, 167 };
vector<int> int_reg(32, 0);
vector<float> flt_reg(32, 0);
vector<Instruction> instructions;

// List of available registers
enum REG_ABLE { READY, U_SOURCE, U_DEST };
vector<int> able_int_reg(32, READY);
vector<int> able_flt_reg(32, READY);


// Operator class definition
enum PIPELINE { ISSUE, READ, EXE, WRITE };
class Operator {
public:

    // Must send operator type to initialize
    Operator(OP_TYPE operation_type);

    // Makes the operator tick a clock cycle
    void Tick(int clock_value);

    string inst_name;
    int source_loc;
    int second_source;
    int dest_loc;
    int cycle_time;
    OP_TYPE operation;
    bool in_operation;
    bool complete;
    PIPELINE cur_state;
    vector<int> stage_comp_values{ 0, 0, 0, 0 };
    bool two_sources;
    int inst_number;
};


// Main program
int main()
{

    // Take in input file
    string input_name;
    cout << "Type input file and push enter: ";
    cin >> input_name;

    // Load file
    ifstream file;
    file.open(input_name);
    if (!file) { 
        cout << "Could not read file" << endl;
        return 0;
    }
    
    // Load each instruction as instruction class
    string instruction;
    string inst_name;
    while (getline(file, instruction)) {

        // Loading instruction name and checking if valid
        getline(stringstream(instruction), inst_name, ' ');
        if (inst_name == LOAD || inst_name == STORE || inst_name == INT_ADD || 
            inst_name == IMM_ADD || inst_name == FLT_ADD || inst_name == FLT_SUB || 
            inst_name == INT_SUB || inst_name == FLT_MUL || inst_name == FLT_DIV) {
            Instruction new_inst(instruction);
            instructions.push_back(new_inst);
        }
        else {
            cout << "Invalid instruction: " << instruction << endl;
        }
    }
    file.close();

    if (instructions.size() == 0) {
        cout << "No instructions Read" << endl;
        return 0;
    }

    // Used for output
    vector<vector<int>> out_values;
    out_values.resize(instructions.size());

    // Creating operators
    Operator int_adder(IN_ADD);
    Operator fp_mult(FP_MUL);
    Operator fp_divide(FP_DIV);
    Operator fp_adder(FP_ADD);

    // Setup variables
    int clock = 1;
    bool inst_complete = false;
    int issue_inst_ind = 1; // Keep track if which instructions have been issued
    bool issued_during_cycle = false;

    // Executing each Instruction
    do {

        // Operating on issued instructions
        issued_during_cycle = false;
        for (int i = 0; i < issue_inst_ind; i++) {

            // Can the integer unit operation be issued?
            if (instructions.at(i).OpType() == IN_ADD && !int_adder.in_operation &&
                !instructions.at(i).isComplete() && !issued_during_cycle) {

                // Integer Add/Subtract
                if (instructions.at(i).Name() == INT_ADD || 
                    instructions.at(i).Name() == INT_SUB) {
                    if (able_int_reg.at(instructions.at(i).Source()) != U_DEST &&
                        able_int_reg.at(instructions.at(i).SecondSource()) != U_DEST &&
                        able_flt_reg.at(instructions.at(i).Dest()) != U_DEST) {

                        // Issuing integer operation
                        int_adder.in_operation = true;
                        int_adder.cur_state = ISSUE;
                        int_adder.source_loc = instructions.at(i).Source();
                        int_adder.second_source = instructions.at(i).SecondSource();
                        int_adder.dest_loc = instructions.at(i).Dest();
                        int_adder.cycle_time = instructions.at(i).Time();
                        int_adder.complete = false;
                        int_adder.two_sources = true;
                        int_adder.inst_name = instructions.at(i).Name();
                        int_adder.inst_number = i;
                        int_adder.stage_comp_values.at(0) = clock;
                        if (issue_inst_ind < instructions.size()) { issue_inst_ind++; }
                        issued_during_cycle = true;
                    }
                }

                // Floating Point Load/Store
                else if (instructions.at(i).Name() == LOAD ||
                         instructions.at(i).Name() == STORE) {
                    if (able_int_reg.at(instructions.at(i).Dest()) == READY) {
                            
                        // Issuing
                        int_adder.in_operation = true;
                        int_adder.cur_state = ISSUE;
                        int_adder.source_loc = instructions.at(i).Source();
                        int_adder.second_source = instructions.at(i).SecondSource();
                        int_adder.dest_loc = instructions.at(i).Dest();
                        int_adder.cycle_time = instructions.at(i).Time();
                        int_adder.complete = false;
                        int_adder.two_sources = false;
                        int_adder.inst_name = instructions.at(i).Name();
                        int_adder.inst_number = i;
                        int_adder.stage_comp_values.at(0) = clock;
                        if (issue_inst_ind < instructions.size()) { issue_inst_ind++; }
                        issued_during_cycle = true;
                    }
                }

                // Integer Add immediate
                else if (instructions.at(i).Name() == IMM_ADD) {
                    if (able_int_reg.at(instructions.at(i).Source()) != U_DEST &&
                        able_flt_reg.at(instructions.at(i).Dest()) != U_DEST) {

                        // Issuing
                        int_adder.in_operation = true;
                        int_adder.cur_state = ISSUE;
                        int_adder.source_loc = instructions.at(i).Source();
                        int_adder.second_source = instructions.at(i).SecondSource();
                        int_adder.dest_loc = instructions.at(i).Dest();
                        int_adder.cycle_time = instructions.at(i).Time();
                        int_adder.complete = false;
                        int_adder.two_sources = false;
                        int_adder.inst_name = instructions.at(i).Name();
                        int_adder.inst_number = i;
                        int_adder.stage_comp_values.at(0) = clock;
                        if (issue_inst_ind < instructions.size()) { issue_inst_ind++; }
                        issued_during_cycle = true;
                    }
                }
            }

            // Floating Point ADD, check reg if can issue
            else if (instructions.at(i).OpType() == FP_ADD && !fp_adder.in_operation &&
                     !instructions.at(i).isComplete() && !issued_during_cycle){
                if (able_flt_reg.at(instructions.at(i).Source()) != U_DEST &&
                    able_flt_reg.at(instructions.at(i).SecondSource()) != U_DEST &&
                    able_flt_reg.at(instructions.at(i).Dest()) != U_DEST) {

                    // Issuing
                    fp_adder.in_operation = true;
                    fp_adder.cur_state = ISSUE;
                    fp_adder.source_loc = instructions.at(i).Source();
                    fp_adder.second_source = instructions.at(i).SecondSource();
                    fp_adder.dest_loc = instructions.at(i).Dest();
                    fp_adder.cycle_time = instructions.at(i).Time();
                    fp_adder.complete = false;
                    fp_adder.two_sources = true;
                    fp_adder.inst_name = instructions.at(i).Name();
                    fp_adder.inst_number = i;
                    fp_adder.stage_comp_values.at(0) = clock;
                    if (issue_inst_ind < instructions.size()) { issue_inst_ind++; }
                    issued_during_cycle = true;
                }

            }

            // Floating Point MULTIPLY, check reg if can issue
            else if (instructions.at(i).OpType() == FP_MUL && !fp_mult.in_operation &&
                     !instructions.at(i).isComplete() && !issued_during_cycle) {
                if (able_flt_reg.at(instructions.at(i).Source()) != U_DEST &&
                    able_flt_reg.at(instructions.at(i).SecondSource()) != U_DEST &&
                    able_flt_reg.at(instructions.at(i).Dest()) != U_DEST) {

                    // Issuing
                    fp_mult.in_operation = true;
                    fp_mult.cur_state = ISSUE;
                    fp_mult.source_loc = instructions.at(i).Source();
                    fp_mult.second_source = instructions.at(i).SecondSource();
                    fp_mult.dest_loc = instructions.at(i).Dest();
                    fp_mult.cycle_time = instructions.at(i).Time();
                    fp_mult.complete = false;
                    fp_mult.two_sources = true;
                    fp_mult.inst_name = instructions.at(i).Name();
                    fp_mult.inst_number = i;
                    fp_mult.stage_comp_values.at(0) = clock;
                    if (issue_inst_ind < instructions.size()) { issue_inst_ind++; }
                    issued_during_cycle = true;
                }

            }

            // Floating Point DIVIDE, check reg if can issue
            else if (instructions.at(i).OpType() == FP_DIV && !fp_divide.in_operation &&
                     !instructions.at(i).isComplete() && !issued_during_cycle) {
                if (able_flt_reg.at(instructions.at(i).Source()) != U_DEST &&
                    able_flt_reg.at(instructions.at(i).SecondSource()) != U_DEST &&
                    able_flt_reg.at(instructions.at(i).Dest()) != U_DEST) {

                    // Issuing
                    fp_divide.in_operation = true;
                    fp_divide.cur_state = ISSUE;
                    fp_divide.source_loc = instructions.at(i).Source();
                    fp_divide.second_source = instructions.at(i).SecondSource();
                    fp_divide.dest_loc = instructions.at(i).Dest();
                    fp_divide.cycle_time = instructions.at(i).Time();
                    fp_divide.complete = false;
                    fp_divide.two_sources = true;
                    fp_divide.inst_name = instructions.at(i).Name();
                    fp_divide.inst_number = i;
                    fp_divide.stage_comp_values.at(0) = clock;
                    if (issue_inst_ind < instructions.size()) { issue_inst_ind++; }
                    issued_during_cycle = true;
                }

            }
        }

        // Progressing operations
        int_adder.Tick(clock);
        fp_mult.Tick(clock);
        fp_divide.Tick(clock);
        fp_adder.Tick(clock);

        clock += 1;

        // Checking if the operators finished, record values
        if (int_adder.complete) {
            int_adder.complete = false;
            out_values.at(int_adder.inst_number) = int_adder.stage_comp_values;
            instructions.at(int_adder.inst_number).setComplete(true);
        }
        if (fp_mult.complete) {
            fp_mult.complete = false;
            out_values.at(fp_mult.inst_number) = fp_mult.stage_comp_values;
            instructions.at(fp_mult.inst_number).setComplete(true);
        }
        if (fp_divide.complete) {
            fp_divide.complete = false;
            out_values.at(fp_divide.inst_number) = fp_divide.stage_comp_values;
            instructions.at(fp_divide.inst_number).setComplete(true);
        }
        if (fp_adder.complete) {
            fp_adder.complete = false;
            out_values.at(fp_adder.inst_number) = fp_adder.stage_comp_values;
            instructions.at(fp_adder.inst_number).setComplete(true);
        }

        // Checking if all instructions are complete
        inst_complete = true;
        for (int i = 0; i < instructions.size(); i++) {
            if (!instructions.at(i).isComplete()) { inst_complete = false;  }
        }

    } while (!inst_complete);


    // Outputting result
    cout << "Instruction\t\tIssue\tRead\tExecute\tWrite" << endl;
    for (int i = 0; i < instructions.size(); i++) {
        cout << setw(20) << left << instructions.at(i).instString() << "\t" << out_values.at(i).at(0) 
            << "\t" << out_values.at(i).at(1) << "\t" << out_values.at(i).at(2) 
            << "\t" << out_values.at(i).at(3) << endl;
    }
    cout << endl;

    // Outputting Integer Registers
    cout << "---Integer Registers---" << endl;
    for (int i = 0; i < int_reg.size(); i++) {
        cout << "Index: " << i << ", Value: " << int_reg.at(i) << endl;
    }
    cout << endl;

    // Outputting Floating Point Registers
    cout << "---Floating Point Registers---" << endl;
    for (int i = 0; i < flt_reg.size(); i++) {
        cout << "Index: " << i << ", Value: " << flt_reg.at(i) << endl;
    }
}


// Constructor
Operator::Operator(OP_TYPE operation_type) {
    operation = operation_type;
    source_loc = 0;
    dest_loc = 0;
    second_source = 0;
    cycle_time = 0;
    in_operation = false;
    complete = false;
    two_sources = false;
}

void Operator::Tick(int clock_value) {

    // Check if the stages can be progressed, store and load use fp mem/reg
    if (in_operation && operation == IN_ADD && 
        inst_name != LOAD && inst_name != STORE) {

        // If there are RAW hazards, for 1 source
        if (cur_state == ISSUE && !two_sources &&
            able_int_reg.at(source_loc) != U_DEST) {

            // Move on to next state
            cur_state = READ;

            // Flagging registers
            able_int_reg.at(source_loc) = U_SOURCE;
            able_int_reg.at(dest_loc) = U_DEST;
        }

        // If there are RAW hazards, for 2 sources
        else if (cur_state == ISSUE && two_sources &&
            able_int_reg.at(source_loc) != U_DEST) {
            if (able_int_reg.at(second_source) != U_DEST) {

                // Move on to next state
                cur_state = READ;

                // Flagging the registers are being used
                able_int_reg.at(source_loc) = U_SOURCE;
                able_int_reg.at(second_source) = U_SOURCE;
                able_int_reg.at(dest_loc) = U_DEST;
            }
        }

        // Can go from read directly to execute
        else if (cur_state == READ) {

            // Flagging the registers are being used
            cur_state = EXE;
            stage_comp_values.at(1) = clock_value;
        }

        // WAR hazard moving to WRITE stage
        else if (cur_state == EXE) {

            // Is execute finished?
            if (clock_value >= stage_comp_values.at(1) + cycle_time) {

                // WAR hazard?
                if (able_int_reg.at(dest_loc) == U_DEST) {
                    cur_state = WRITE;
                    stage_comp_values.at(2) = clock_value;
                }
            }
        }

        else if (cur_state == WRITE) {

            // Integer addition
            if (inst_name == INT_ADD) {
                int_reg.at(dest_loc) = int_reg.at(source_loc) + int_reg.at(second_source);
            }

            // Integer subtract
            else if (inst_name == INT_SUB) {
                int_reg.at(dest_loc) = int_reg.at(source_loc) - int_reg.at(second_source);
            }

            // Immediate add
            else {
                int_reg.at(dest_loc) = int_reg.at(source_loc) + second_source;
            }
            stage_comp_values.at(3) = clock_value;
            in_operation = false;
            able_int_reg.at(source_loc) = READY;
            able_int_reg.at(dest_loc) = READY;
            if (two_sources) { able_int_reg.at(second_source) = READY; }
            complete = true;
        }
    }

    // Check if the stages can be progressed, Load and store use fp mem/reg
    if (in_operation && (operation != IN_ADD || inst_name == LOAD || 
        inst_name == STORE)) {

        // If there are RAW hazards, for 1 source
        if (cur_state == ISSUE && !two_sources &&
            able_flt_reg.at(source_loc) != U_DEST) {

            // Move on to next state
            cur_state = READ;

            // Flagging registers
            able_flt_reg.at(source_loc) = U_SOURCE;
            able_flt_reg.at(dest_loc) = U_DEST;
        }

        // If there are RAW hazards, for 2 sources
        else if (cur_state == ISSUE && two_sources &&
            able_flt_reg.at(source_loc) != U_DEST) {
            if (able_flt_reg.at(second_source) != U_DEST) {

                // Move on to next state
                cur_state = READ;

                // Flagging the registers are being used
                able_flt_reg.at(source_loc) = U_SOURCE;
                able_flt_reg.at(second_source) = U_SOURCE;
                able_flt_reg.at(dest_loc) = U_DEST;
            }
        }

        // Can go from read directly to execute
        else if (cur_state == READ) {

            // Flagging the registers are being used
            cur_state = EXE;
            stage_comp_values.at(1) = clock_value;
        }

        // WAR hazard moving to WRITE stage
        else if (cur_state == EXE) {

            // Is execute finished?
            if (clock_value >= stage_comp_values.at(1) + cycle_time) {

                // WAR hazard?
                if (able_flt_reg.at(dest_loc) == U_DEST) {
                    cur_state = WRITE;
                    stage_comp_values.at(2) = clock_value;
                }
            }
        }

        else if (cur_state == WRITE) {

            // Load from memory location
            if (inst_name == LOAD) {
                flt_reg.at(dest_loc) = memory.at(source_loc);
            }

            // Store into memory location
            else if (inst_name == STORE) {
                memory.at(source_loc) = flt_reg.at(dest_loc);
            }

            // Float add
            else if (inst_name == FLT_ADD) {
                flt_reg.at(dest_loc) = flt_reg.at(source_loc) + flt_reg.at(second_source);
            }

            // Float Subtract
            else if (inst_name == FLT_SUB) {
                flt_reg.at(dest_loc) = flt_reg.at(source_loc) - flt_reg.at(second_source);
            }

            // Float Multiply
            else if (inst_name == FLT_MUL) {
                flt_reg.at(dest_loc) = flt_reg.at(source_loc) * flt_reg.at(second_source);
            }

            // Float Divide
            else {
                flt_reg.at(dest_loc) = flt_reg.at(source_loc) / flt_reg.at(second_source);
            }
            stage_comp_values.at(3) = clock_value;
            in_operation = false;
            able_flt_reg.at(dest_loc) = READY;
            if (two_sources) { 
                able_flt_reg.at(second_source) = READY; 
                able_flt_reg.at(source_loc) = READY;
            }
            complete = true;
        }
    }
}
