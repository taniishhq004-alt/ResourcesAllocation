# Emergency Resource Allocation System — C Program

## About
Console-based implementation using **Priority Queue (Max-Heap)** in C.

## Compile & Run
```bash
gcc main.c -o resource_alloc
./resource_alloc
```

## On Windows
```cmd
gcc main.c -o resource_alloc.exe
resource_alloc.exe
```

## Menu Options
1. Add Emergency Request
2. Allocate Resources (Highest Priority)
3. View All Pending Requests
4. View Next Priority Request
5. View Allocation History
6. Exit

## Data Structure
Priority Queue (Max-Heap) — insert O(log n), extractMax O(log n), peek O(1)
