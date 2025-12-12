# 🖥️ Batch Operating System Simulator

A comprehensive C++ implementation of a batch operating system simulator with virtual memory management, page table management, and process control.

---

## 📋 Project Overview

This project simulates a **Batch Operating System** with advanced memory management features including:

- **Process Control Block (PCB)** - Manages job execution with time and line limits
- **Virtual Memory Management** - Virtual-to-physical address translation
- **Page Table Management** - Dynamic page allocation and page fault handling
- **Master Operating System (MOS)** - Handles interrupts and system operations
- **Instruction Execution** - Supports multiple instruction types (LR, SR, CR, BT, GD, PD, H)

---

## 🎯 Key Features

### ✨ Core Features
- ✅ Process loading and initialization
- ✅ Virtual memory address mapping
- ✅ Page fault detection and handling
- ✅ Dynamic frame allocation
- ✅ Multi-interrupt handling (Timer, System, Program)
- ✅ Instruction validation and execution
- ✅ Memory protection and limit enforcement
- ✅ Detailed error reporting

### 📊 Supported Instructions
| Instruction | Operation | Description |
|---|---|---|
| **LR** | Load Register | Load data from memory to register |
| **SR** | Store Register | Store register data to memory |
| **CR** | Compare Register | Compare register with memory |
| **BT** | Branch on Toggle | Conditional branch based on flag |
| **GD** | Get Data | Read input data |
| **PD** | Put Data | Write output data |
| **H** | Halt | Terminate execution |

---

## 🛠️ Tools & Tech Stack

<div align="center">

### **Programming Language**
![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)

### **Development & Compilation**
![G++](https://img.shields.io/badge/G%2B%2B-Compiler-blue?style=for-the-badge)
![Visual Studio Code](https://img.shields.io/badge/Visual%20Studio%20Code-0078D4?style=for-the-badge&logo=visual-studio-code&logoColor=white)

### **Version Control**
![Git](https://img.shields.io/badge/Git-F05032?style=for-the-badge&logo=git&logoColor=white)
![GitHub](https://img.shields.io/badge/GitHub-181717?style=for-the-badge&logo=github&logoColor=white)

### **Operating System**
![Windows](https://img.shields.io/badge/Windows-0078D4?style=for-the-badge&logo=windows&logoColor=white)

</div>

---

## 📁 Project Structure

```
Phase03/
├── phase2.cpp                    # Main OS Simulator Implementation
├── input.txt                     # Input data for job execution
├── output.txt                    # Output results
├── input2.txt                    # Alternative input file
├── output2.txt                   # Alternative output file
├── Job.txt                       # Job specifications
├── Operating_System_Simulator.cpp # Additional implementation
├── README.md                     # Project documentation
└── .git/                         # Git repository
```

---

## 🏗️ System Architecture

### Memory Management
```
┌─────────────────────────────┐
│   Main Memory (300 words)   │
│   [4 bytes per word]        │
├─────────────────────────────┤
│  Page Table (10 entries)    │
│  ├─ Maps Virtual → Frame    │
│  └─ Handles Page Faults     │
├─────────────────────────────┤
│  Instruction Memory         │
│  ├─ User Program Code       │
│  └─ Data Storage            │
└─────────────────────────────┘
```

### Process Control Block (PCB)
```cpp
struct PCB {
    int job_id;        // Job identifier
    int TTL;           // Total Time Limit
    int TLL;           // Total Line Limit
    int TTC;           // Total Time Counter
    int LLC;           // Line Limit Counter
};
```

### Interrupt Handling
| Interrupt Type | Code | Description |
|---|---|---|
| **Timer Interrupt (TI)** | 2 | Time limit exceeded |
| **System Interrupt (SI)** | 1-3 | Read/Write/Halt operations |
| **Program Interrupt (PI)** | 1-3 | Opcode/Operand error or Page fault |

---

## 🚀 How to Compile & Run

### Prerequisites
- **G++ Compiler** (v7.0 or higher)
- **Windows PowerShell** or Command Prompt

### Compilation
```powershell
cd "c:\Users\alpha\OneDrive\Desktop\Phase03"
g++ phase2.cpp -o phase2
```

### Execution
```powershell
.\phase2
```

### Expected Output
```
Job 1001 Loaded. PTR: 120
Valid Page Fault. Allocated Frame: 15 for VA: 5
...
Terminated Normally. No Error
```

---

## 📝 Input File Format

The input file should follow this structure:

```
$AMJ <JobID> <TTL> <TLL>        # Start new job
<instruction><operand>          # User program instructions
...
$DTA                            # Start data input
<input_data>                    # Data lines
...
$END                            # End of input
```

### Example Input
```
$AMJ000100100010
LR05
SR10
GD20
CR15
PD30
H
$DTA
Sample input data
More test data
$END
```

---

## 🔍 Sample Output

```
Job 1001 Loaded. PTR: 120
Valid Page Fault. Allocated Frame: 15 for VA: 5
Valid Page Fault. Allocated Frame: 8 for VA: 1

Sample input data
More test data

Terminated Normally. No Error
IC=6, IR=H   , C=0, R=    , TTL=100, TTC=15, TLL=10, LLC=2
```

---

## 🧮 Technical Specifications

| Component | Value |
|---|---|
| **Memory Size** | 300 words |
| **Word Size** | 4 bytes |
| **Page Size** | 10 words |
| **Memory Pages** | 30 total |
| **Time Quantum** | 10 time units |
| **Page Table Entries** | 10 per job |

---

## ⚙️ Algorithm Overview

### Address Translation (Virtual → Physical)
```
VA (Virtual Address) 
  ↓
[VA / PAGE_SIZE] → Page Number
  ↓
Page Table[Page Number] → Frame Number
  ↓
PA = (Frame Number * PAGE_SIZE) + (VA % PAGE_SIZE)
```

### Page Fault Handling
1. Detect missing page (marked with '*' in PT)
2. Allocate free frame from memory
3. Update page table entry
4. Resume instruction execution

### Instruction Execution Cycle
1. **Fetch** - Load instruction from memory
2. **Validate** - Check opcode and operands
3. **Decode** - Identify operation type
4. **Execute** - Perform operation
5. **Handle Interrupts** - Process any interrupts

---

## 📚 Key Classes & Methods

### OS Class
```cpp
class OS {
private:
    char M[MEMORY_SIZE][WORD_SIZE];     // Main Memory
    PCB pcb;                             // Process Control Block
    
    // Core Methods
    void load();                         // Load jobs
    void executeUserProgram();           // Execute instructions
    void masterMode();                   // Handle interrupts
    void handlePageFault();              // Page fault handler
    
    // Utilities
    int addressMap(int VA);              // Address translation
    int allocateFrame();                 // Frame allocation
};
```

---

## 🐛 Error Handling

| Error Code | Description | Action |
|---|---|---|
| 0 | No Error | Normal termination |
| 1 | Out of Data | End of input reached |
| 2 | Line Limit Exceeded | Output line limit violated |
| 3 | Time Limit Exceeded | Execution time limit exceeded |
| 4 | Operation Code Error | Invalid instruction opcode |
| 5 | Operand Error | Invalid operand value |
| 6 | Invalid Page Fault | Page fault in non-I/O instruction |

---

## 📊 Performance Characteristics

- **Time Complexity** - O(1) for address mapping
- **Space Complexity** - O(n) where n = memory size
- **Interrupt Handling** - O(1) per interrupt
- **Page Allocation** - O(1) random allocation

---

## 🔐 Memory Safety Features

- ✅ Bounds checking on memory access
- ✅ Page table validation
- ✅ Instruction operand verification
- ✅ Time and line limit enforcement
- ✅ Error messages for debugging

---

## 📖 Learning Objectives

This project demonstrates understanding of:
- Operating system fundamentals
- Virtual memory and paging
- Process control and scheduling
- Interrupt handling mechanisms
- Memory management strategies
- System software design patterns

---

## 👨‍💻 Author

**Pranav Bhawari**
- GitHub: [@Pranav6197](https://github.com/Pranav6197)
- Email: pranav.bhawari22@vit.edu

---

## 📄 License

This project is open source and available under the MIT License.

---

## 🤝 Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

---

## 📞 Support

For questions or issues, please open an issue on the [GitHub repository](https://github.com/Pranav6197/Batch-Operating-System-Simulation-).

---

<div align="center">

**Built with ❤️ for Operating Systems Education**

⭐ If you found this helpful, please consider giving it a star!

</div>
