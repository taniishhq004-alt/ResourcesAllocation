# Emergency Resource Allocation System — C Program

## About
Console-based implementation using **4 Data Structures** in C:
1. **Priority Queue (Max-Heap)** — Request prioritization
2. **Singly Linked List** — Dynamic allocation history
3. **Hash Table (Chaining)** — Location-based search
4. **Stack** — Undo last allocation

Additional features: File I/O, Sorting, Statistics, Search

## Compile & Run

### Linux / macOS
```bash
gcc main.c -o resource_alloc
./resource_alloc
```

### Windows
```cmd
gcc main.c -o resource_alloc.exe
resource_alloc.exe
```

## Menu Options
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

## Data Structures & Complexities

| Operation | Data Structure | Time |
|---|---|---|
| Insert request | Max-Heap | O(log n) |
| Extract max | Max-Heap | O(log n) |
| Peek | Max-Heap | O(1) |
| Add to history | Linked List | O(n) |
| Search location | Hash Table | O(1) avg |
| Undo allocation | Stack | O(1) |
| Sort requests | Bubble Sort | O(n²) |

## File I/O
- Data auto-saves to `emergency_data.txt` on exit
- Data auto-loads from file on startup
- Format: pipe-separated values (`|`)
