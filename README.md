# 🚨 Emergency Resource Allocation System

**BCA 2nd Semester · Minor Project · Data Structures Using C**

---

## About

A simulation of an **Emergency Resource Allocation System** that demonstrates how data structures can support decision-making during disaster situations. The system uses a **Priority Queue (Max-Heap)** to ensure areas with higher urgency receive resources first.

## Features

- Accept emergency requests from multiple locations
- Assign urgency levels (1-10) for each request
- Store requests using a **Priority Queue (Max-Heap)**
- Allocate resources to the highest-priority request first
- Display allocation history and pending requests
- Visual web dashboard with real-time updates

## Project Structure

```
RESORCES_ALLOCATION/
├── c_program/
│   ├── main.c          # C console program (Priority Queue)
│   └── README.md       # Compile & run instructions
├── web/
│   ├── index.html      # Web dashboard
│   ├── style.css       # Styling
│   └── script.js       # JS Priority Queue + UI logic
└── README.md           # This file
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

## Data Structure

**Priority Queue** implemented as a **Max-Heap** using arrays:

| Operation    | Description                          | Time Complexity |
|-------------|--------------------------------------|-----------------|
| insert()     | Add new emergency request            | O(log n)        |
| extractMax() | Remove highest-urgency request       | O(log n)        |
| heapifyUp()  | Maintain heap after insertion        | O(log n)        |
| heapifyDown()| Maintain heap after extraction       | O(log n)        |
| peek()       | View highest-priority request        | O(1)            |

## Tools Used

- **Language:** C (GCC Compiler)
- **Frontend:** HTML, CSS, JavaScript
- **OS:** Windows
- **Editor:** VS Code
