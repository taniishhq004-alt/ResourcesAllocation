# 🚨 Emergency Resource Allocation System

**BCA 2nd Semester · Minor Project · Data Structures Using C**

---

## About

A simulation of an **Emergency Resource Allocation System** that demonstrates how multiple data structures can support decision-making during disaster situations. The system uses **4 core data structures** — Priority Queue, Linked List, Hash Table, and Stack — to manage emergency requests, allocate resources efficiently, enable fast search, and support undo operations.

## Data Structures Used

| # | Data Structure | Purpose | Implementation |
|---|---|---|---|
| 1 | **Priority Queue (Max-Heap)** | Request prioritization by urgency | Array-based Max-Heap |
| 2 | **Singly Linked List** | Dynamic allocation history | Pointer-based linked nodes |
| 3 | **Hash Table (Chaining)** | Fast location-based search | djb2 hash + linked list chains |
| 4 | **Stack (LIFO)** | Undo last allocation | Array-based stack |

## Features

- Accept emergency requests from multiple locations
- Assign urgency levels (1–10) for each request
- Allocate resources to the highest-priority request first (Max-Heap)
- Dynamic allocation history using Linked List
- Search by location using Hash Table (O(1) average)
- Search by disaster type (Flood, Earthquake, Fire, etc.)
- Undo last allocation using Stack
- Sort requests by urgency, people affected, or ID
- Statistics report with disaster & resource breakdowns
- File I/O — auto-save on exit, auto-load on startup
- Web dashboard with real-time simulation

## Project Structure

```
RESORCES_ALLOCATION/
├── c_program/
│   ├── main.c              # C program (794 lines, 4 data structures)
│   ├── resource_alloc.exe   # Compiled executable
│   ├── emergency_data.txt   # Auto-generated save file
│   └── README.md            # Compile & run instructions
├── web/
│   ├── index.html           # Web dashboard (simulation)
│   ├── about.html           # Project introduction
│   ├── how-it-works.html    # Data structure explanation
│   ├── style.css            # Styling
│   ├── script.js            # JS Priority Queue + UI logic
│   └── cursor.js            # Cursor animation
└── README.md                # This file
```

## How to Run

### C Program
```bash
cd c_program
gcc main.c -o resource_alloc
./resource_alloc
```

### Web Version
Open `web/index.html` in any modern web browser (Chrome, Firefox, Edge).

## C Program — Menu Options

```
 1.  Add Emergency Request
 2.  Allocate Resources (Highest Priority)
 3.  View All Pending Requests
 4.  View Next Priority Request (Peek)
 5.  View Allocation History (Linked List)
 6.  Search by Location (Hash Table)
 7.  Search by Disaster Type
 8.  Undo Last Allocation (Stack)
 9.  Sort & Display Requests
10.  Statistics Report
11.  Save Data to File
12.  Load Data from File
13.  Exit
```

## Time Complexities

| Operation | Data Structure | Complexity |
|---|---|---|
| `pqInsert()` | Max-Heap | O(log n) |
| `pqExtractMax()` | Max-Heap | O(log n) |
| `pqPeek()` | Max-Heap | O(1) |
| `historyAdd()` | Linked List | O(n) |
| `historyDisplay()` | Linked List | O(n) |
| `htInsert()` | Hash Table | O(1) avg |
| `htSearch()` | Hash Table | O(1) avg |
| `stackPush()` | Stack | O(1) |
| `stackPop()` | Stack | O(1) |
| `sortAndDisplay()` | Bubble Sort | O(n²) |

## Tools Used

- **Language:** C (GCC Compiler)
- **Frontend:** HTML, CSS, JavaScript
- **OS:** Windows
- **Editor:** VS Code
