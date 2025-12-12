#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <vector>

using namespace std;

// Constants
constexpr int MEMORY_SIZE = 300;
constexpr int WORD_SIZE = 4;
constexpr int TIME_QUANTUM = 10;
constexpr int PAGE_SIZE = 10;

// Process Control Block
struct PCB {
    int input2_id;
    int TTL; // Total Time Limit
    int TLL; // Total Line Limit
    int TTC; // Total Time Counter
    int LLC; // Line Limit Counter

    void init(int id, int ttl, int tll) {
        input2_id = id;
        TTL = ttl;
        TLL = tll;
        TTC = 0;
        LLC = 0;
    }
};

class OS {
private:
    char M[MEMORY_SIZE][WORD_SIZE]; // Main Memory
    char IR[WORD_SIZE];             // Instruction Register
    char R[WORD_SIZE];              // General Purpose Register
    bool C;                         // Toggle Flag
    
    int IC;  // Instruction Counter
    int SI;  // System Interrupt
    int PI;  // Program Interrupt
    int TI;  // Timer Interrupt
    int PTR; // Page Table Register
    
    bool breakFlag;
    PCB pcb;

    ifstream inputFile;
    ofstream outputFile;

    // Error Messages
    const string errorMessages[7] = {
        "No Error", 
        "Out of Data", 
        "Line Limit Exceeded", 
        "Time Limit Exceeded",
        "Operation Code Error", 
        "Operand Error", 
        "Invalid Page Fault"
    };

public:
    OS();
    ~OS();
    void load();

private:
    void initialize();
    int allocateFrame();
    int addressMap(int VA);
    void executeUserProgram();
    void masterMode(); // MOS
    void terminate(int errorType, int subError = -1);
    
    // Instructions / Operations
    void read(int RA);
    void write(int RA);
    void handlePageFault();
};

OS::OS() {
    inputFile.open("input2.txt");
    outputFile.open("output2.txt");
    initialize();
    srand(time(0));
}

OS::~OS() {
    if (inputFile.is_open()) inputFile.close();
    if (outputFile.is_open()) outputFile.close();
}

void OS::initialize() {
    // Clear Memory
    for (int i = 0; i < MEMORY_SIZE; i++) {
        for (int j = 0; j < WORD_SIZE; j++) {
            M[i][j] = '\0';
        }
    }
    
    // Reset Registers
    memset(IR, '\0', WORD_SIZE);
    memset(R, '\0', WORD_SIZE);
    C = false;
    SI = 0;
    PI = 0;
    TI = 0;
    breakFlag = false;
}

int OS::allocateFrame() {
    return rand() % 30;
}

void OS::terminate(int errorType, int subError) {
    outputFile << endl << endl;
    if (errorType == 0) {
        outputFile << "Terminated Normally. " << errorMessages[errorType];
    } else {
        outputFile << errorType << " - " << errorMessages[errorType];
        if (subError != -1) {
            outputFile << ". " << errorMessages[subError];
        }
        outputFile << endl;
        outputFile << "IC=" << IC << ", IR=" << string(IR, 4) 
                   << ", C=" << C << ", R=" << string(R, 4) 
                   << ", TTL=" << pcb.TTL << ", TTC=" << pcb.TTC 
                   << ", TLL=" << pcb.TLL << ", LLC=" << pcb.LLC << endl;
    }
}

void OS::read(int RA) {
    string line;
    if (getline(inputFile, line)) {
        if (line.find("$END") != string::npos) {
            terminate(1);
            breakFlag = true;
        } else {
            for (int i = 0; i < line.length() && i < 40; i++) {
                int row = RA + (i / 4);
                int col = i % 4;
                M[row][col] = line[i];
            }
        }
    }
}

void OS::write(int RA) {
    pcb.LLC++;
    if (pcb.LLC > pcb.TLL) {
        terminate(2);
        breakFlag = true;
        return;
    }

    string output = "";
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 4; j++) {
            if (M[RA + i][j] != '\0')
                output += M[RA + i][j];
        }
    }
    outputFile << output << endl;
}

int OS::addressMap(int VA) {
    if (VA >= 0 && VA < 100) {
        int pteIndex = PTR + (VA / 10);
        
        if (M[pteIndex][0] == '*') {
            PI = 3;
            return -1;
        }
        
        string frameStr = "";
        for (int i = 0; i < 4; i++) frameStr += M[pteIndex][i];
        
        try {
            int frame = stoi(frameStr);
            return frame * 10 + (VA % 10);
        } catch (...) {
            PI = 2;
            return -1;
        }
    }
    PI = 2;
    return -1;
}

void OS::handlePageFault() {
    string opcode = string(1, IR[0]) + string(1, IR[1]);
    
    if (opcode == "GD" || opcode == "SR") {
        int frame;
        do {
            frame = allocateFrame();
        } while (M[frame * 10][0] != '\0');

        string operandStr = string(1, IR[2]) + string(1, IR[3]);
        int VA = stoi(operandStr);
        int pteIndex = PTR + (VA / 10);
        
        sprintf(M[pteIndex], "%d", frame);
        
        cout << "Valid Page Fault. Allocated Frame: " << frame << " for VA: " << VA << endl;
        pcb.TTC++;
    } else {
        terminate(6);
        breakFlag = true;
    }
}

void OS::masterMode() {
    if (TI == 2) {
        terminate(3);
        breakFlag = true;
        return;
    }

    if (SI != 0) {
        switch (SI) {
            case 1:
                break;
            case 2:
                terminate(3);
                break;
            case 3:
                terminate(0);
                breakFlag = true;
                break;
        }
        SI = 0;
    }

    if (PI != 0) {
        switch (PI) {
            case 1: terminate(4); breakFlag = true; break;
            case 2: terminate(5); breakFlag = true; break;
            case 3: handlePageFault(); PI = 0; break;
        }
    }
}

// Helper to execute MOS with RA context if needed
void OS::executeUserProgram() {
    int RA;
    while (!breakFlag) {
        RA = addressMap(IC);
        if (PI != 0) {
            masterMode();
            if (breakFlag) break;
            continue;
        }
        
        memcpy(IR, M[RA], 4);
        IC++;

        if (IR[0] != 'H') {
             if (!isdigit(IR[2]) && !isdigit(IR[3])) {
                 PI = 2;
                 masterMode();
                 if (breakFlag) break;
             }
        }

        string opcode = string(1, IR[0]) + string(1, IR[1]);
        
        if (pcb.TTC >= pcb.TTL) {
            TI = 2;
            masterMode();
            break;
        }

        if (opcode == "H ") {
            SI = 3;
            masterMode();
            break;
        }

        int operandVA = stoi(string(1, IR[2]) + string(1, IR[3]));
        int operandRA = addressMap(operandVA);
        
        if (PI != 0) {
            masterMode();
            if (breakFlag) break;
            if (opcode == "GD" || opcode == "SR") {
                operandRA = addressMap(operandVA);
            } else {
                continue;
            }
        }

        if (opcode == "LR") {
            memcpy(R, M[operandRA], 4);
            pcb.TTC++;
        } else if (opcode == "SR") {
            memcpy(M[operandRA], R, 4);
            pcb.TTC++;
        } else if (opcode == "CR") {
            if (strncmp(R, M[operandRA], 4) == 0) C = true;
            else C = false;
            pcb.TTC++;
        } else if (opcode == "BT") {
            if (C) IC = operandVA;
            pcb.TTC++;
        } else if (opcode == "GD") {
            SI = 1;
            read(operandRA);
            pcb.TTC++;
        } else if (opcode == "PD") {
            SI = 2;
            write(operandRA);
            pcb.TTC++;
        } else {
            PI = 1; // Opcode Error
            masterMode();
        }
    }
}

void OS::load() {
    string line;
    while (getline(inputFile, line)) {
        if (line.substr(0, 4) == "$AMJ") {
            initialize();
            int id = stoi(line.substr(4, 4));
            int ttl = stoi(line.substr(8, 4));
            int tll = stoi(line.substr(12, 4));
            pcb.init(id, ttl, tll);
            
            PTR = allocateFrame() * 10;
            for (int i = 0; i < 10; i++) {
                M[PTR + i][0] = '*';
            }
            cout << "input2 " << id << " Loaded. PTR: " << PTR << endl;
        } else if (line.substr(0, 4) == "$DTA") {
            IC = 0;
            executeUserProgram();
        } else if (line.substr(0, 4) == "$END") {
            continue;
        } else {
            // Load Program Code
            int frame = allocateFrame();
            int ptIndex = PTR;
            while (M[ptIndex][0] != '*') ptIndex++; // Find free PT slot
            
            // Update Page Table
            sprintf(M[ptIndex], "%d", frame);
            
            // Copy code to frame
            for (int i = 0; i < line.length(); i++) {
                M[frame * 10 + (i / 4)][i % 4] = line[i];
            }
        }
    }
}

int main() {
    OS os;
    os.load();
    return 0;
}
