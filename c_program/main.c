/*
 * ============================================================
 *  EMERGENCY RESOURCE ALLOCATION SYSTEM
 *  BCA 2nd Semester - Minor Project (Data Structures Using C)
 * ============================================================
 *
 *  Description:
 *      A simulation of an Emergency Resource Allocation System
 *      that uses a Max-Heap Priority Queue to allocate limited
 *      emergency resources to disaster-affected locations based
 *      on urgency level.
 *
 *  Data Structure Used: Priority Queue (Max-Heap)
 *
 *  Compiled with: gcc main.c -o resource_alloc
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- Constants ---- */
#define MAX_QUEUE_SIZE 100
#define MAX_HISTORY    100

/* ---- Data Structures ---- */

/* Represents a single emergency request */
typedef struct {
    int    id;
    char   location[50];
    char   disaster_type[30];   /* flood, earthquake, fire, accident */
    int    people_affected;
    int    urgency;             /* 1 (low) to 10 (critical)          */
    char   resources_needed[50]; /* food, medical, rescue             */
} EmergencyRequest;

/* Max-Heap Priority Queue */
typedef struct {
    EmergencyRequest data[MAX_QUEUE_SIZE];
    int size;
} PriorityQueue;

/* Allocation history record */
typedef struct {
    EmergencyRequest request;
    int allocation_order;
} AllocationRecord;

/* ---- Global Variables ---- */
PriorityQueue     queue;
AllocationRecord  history[MAX_HISTORY];
int               history_count  = 0;
int               next_id        = 1;
int               allocation_num = 0;

/* ===========================================================
 *  PRIORITY QUEUE OPERATIONS (Max-Heap)
 * =========================================================== */

/* Initialize the priority queue */
void initQueue(PriorityQueue *pq) {
    pq->size = 0;
}

/* Swap two emergency requests in the heap */
void swap(EmergencyRequest *a, EmergencyRequest *b) {
    EmergencyRequest temp = *a;
    *a = *b;
    *b = temp;
}

/* Restore heap property upward after insertion */
void heapifyUp(PriorityQueue *pq, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (pq->data[index].urgency > pq->data[parent].urgency) {
            swap(&pq->data[index], &pq->data[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

/* Restore heap property downward after extraction */
void heapifyDown(PriorityQueue *pq, int index) {
    int largest = index;
    int left    = 2 * index + 1;
    int right   = 2 * index + 2;

    if (left < pq->size && pq->data[left].urgency > pq->data[largest].urgency)
        largest = left;
    if (right < pq->size && pq->data[right].urgency > pq->data[largest].urgency)
        largest = right;

    if (largest != index) {
        swap(&pq->data[index], &pq->data[largest]);
        heapifyDown(pq, largest);
    }
}

/* Insert a new emergency request into the priority queue */
int insert(PriorityQueue *pq, EmergencyRequest req) {
    if (pq->size >= MAX_QUEUE_SIZE) {
        printf("\n  [ERROR] Priority Queue is full! Cannot add more requests.\n");
        return 0;
    }
    pq->data[pq->size] = req;
    heapifyUp(pq, pq->size);
    pq->size++;
    return 1;
}

/* Extract the highest-priority (max urgency) request */
int extractMax(PriorityQueue *pq, EmergencyRequest *result) {
    if (pq->size <= 0) {
        printf("\n  [INFO] No pending requests in the queue.\n");
        return 0;
    }
    *result = pq->data[0];
    pq->data[0] = pq->data[pq->size - 1];
    pq->size--;
    if (pq->size > 0)
        heapifyDown(pq, 0);
    return 1;
}

/* Peek at the highest-priority request without removing it */
int peek(PriorityQueue *pq, EmergencyRequest *result) {
    if (pq->size <= 0) {
        return 0;
    }
    *result = pq->data[0];
    return 1;
}

/* ===========================================================
 *  DISPLAY HELPERS
 * =========================================================== */

/* Print a horizontal separator line */
void printSeparator(void) {
    printf("  +-----+----------------------+----------------+----------+---------+----------------+\n");
}

/* Print the table header for requests */
void printHeader(void) {
    printSeparator();
    printf("  | %-3s | %-20s | %-14s | %-8s | %-7s | %-14s |\n",
           "ID", "Location", "Disaster Type", "Affected", "Urgency", "Resources");
    printSeparator();
}

/* Print a single request row */
void printRequest(EmergencyRequest *req) {
    printf("  | %-3d | %-20s | %-14s | %-8d | %-7d | %-14s |\n",
           req->id,
           req->location,
           req->disaster_type,
           req->people_affected,
           req->urgency,
           req->resources_needed);
}

/* Get urgency label */
const char* getUrgencyLabel(int urgency) {
    if (urgency >= 8) return "CRITICAL";
    if (urgency >= 5) return "HIGH";
    if (urgency >= 3) return "MODERATE";
    return "LOW";
}

/* Print a visual urgency bar */
void printUrgencyBar(int urgency) {
    printf("  Urgency: [");
    int i;
    for (i = 0; i < 10; i++) {
        if (i < urgency) printf("#");
        else printf("-");
    }
    printf("] %d/10 (%s)\n", urgency, getUrgencyLabel(urgency));
}

/* ===========================================================
 *  MENU FUNCTIONS
 * =========================================================== */

/* 1. Add Emergency Request */
void addRequest(void) {
    EmergencyRequest req;
    int choice;

    printf("\n  ========================================\n");
    printf("         ADD EMERGENCY REQUEST\n");
    printf("  ========================================\n\n");

    req.id = next_id++;

    printf("  Enter Location Name: ");
    scanf(" %[^\n]", req.location);

    printf("\n  Select Disaster Type:\n");
    printf("    1. Flood\n");
    printf("    2. Earthquake\n");
    printf("    3. Fire\n");
    printf("    4. Accident\n");
    printf("    5. Cyclone\n");
    printf("    6. Landslide\n");
    printf("  Choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: strcpy(req.disaster_type, "Flood");      break;
        case 2: strcpy(req.disaster_type, "Earthquake");  break;
        case 3: strcpy(req.disaster_type, "Fire");        break;
        case 4: strcpy(req.disaster_type, "Accident");    break;
        case 5: strcpy(req.disaster_type, "Cyclone");     break;
        case 6: strcpy(req.disaster_type, "Landslide");   break;
        default: strcpy(req.disaster_type, "Unknown");    break;
    }

    printf("\n  Number of People Affected: ");
    scanf("%d", &req.people_affected);

    do {
        printf("  Urgency Level (1-10): ");
        scanf("%d", &req.urgency);
        if (req.urgency < 1 || req.urgency > 10)
            printf("  [ERROR] Please enter a value between 1 and 10.\n");
    } while (req.urgency < 1 || req.urgency > 10);

    printf("\n  Select Resources Needed:\n");
    printf("    1. Food & Water\n");
    printf("    2. Medical Supplies\n");
    printf("    3. Rescue Team\n");
    printf("    4. Shelter\n");
    printf("    5. All Resources\n");
    printf("  Choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: strcpy(req.resources_needed, "Food & Water");     break;
        case 2: strcpy(req.resources_needed, "Medical");          break;
        case 3: strcpy(req.resources_needed, "Rescue Team");      break;
        case 4: strcpy(req.resources_needed, "Shelter");          break;
        case 5: strcpy(req.resources_needed, "All Resources");    break;
        default: strcpy(req.resources_needed, "General");         break;
    }

    if (insert(&queue, req)) {
        printf("\n  [SUCCESS] Emergency Request #%d added successfully!\n", req.id);
        printf("  Location : %s\n", req.location);
        printf("  Disaster : %s\n", req.disaster_type);
        printUrgencyBar(req.urgency);
        printf("  Pending Requests: %d\n", queue.size);
    }
}

/* 2. Allocate Resources (extract max priority) */
void allocateResources(void) {
    EmergencyRequest req;

    printf("\n  ========================================\n");
    printf("         ALLOCATE RESOURCES\n");
    printf("  ========================================\n");

    if (extractMax(&queue, &req)) {
        allocation_num++;
        history[history_count].request = req;
        history[history_count].allocation_order = allocation_num;
        history_count++;

        printf("\n  >> RESOURCES ALLOCATED (Priority #%d) <<\n\n", allocation_num);
        printHeader();
        printRequest(&req);
        printSeparator();
        printUrgencyBar(req.urgency);
        printf("\n  Resources Dispatched: %s\n", req.resources_needed);
        printf("  Remaining Requests : %d\n", queue.size);
    }
}

/* 3. View All Pending Requests */
void viewAllRequests(void) {
    int i, j;
    EmergencyRequest sorted[MAX_QUEUE_SIZE];
    int count = queue.size;

    printf("\n  ========================================\n");
    printf("       ALL PENDING REQUESTS (%d)\n", count);
    printf("  ========================================\n");

    if (count == 0) {
        printf("\n  [INFO] No pending emergency requests.\n");
        return;
    }

    /* Copy and sort by urgency (descending) for display */
    for (i = 0; i < count; i++)
        sorted[i] = queue.data[i];

    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (sorted[j].urgency < sorted[j + 1].urgency) {
                EmergencyRequest temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }

    printf("\n");
    printHeader();
    for (i = 0; i < count; i++) {
        printRequest(&sorted[i]);
    }
    printSeparator();
    printf("\n  Total Pending: %d request(s)\n", count);
}

/* 4. View Next Priority Request (peek) */
void viewNextPriority(void) {
    EmergencyRequest req;

    printf("\n  ========================================\n");
    printf("       NEXT PRIORITY REQUEST\n");
    printf("  ========================================\n");

    if (peek(&queue, &req)) {
        printf("\n  >> HIGHEST PRIORITY IN QUEUE <<\n\n");
        printHeader();
        printRequest(&req);
        printSeparator();
        printUrgencyBar(req.urgency);
        printf("\n  This request will be served next.\n");
    } else {
        printf("\n  [INFO] No pending emergency requests.\n");
    }
}

/* 5. View Allocation History */
void viewHistory(void) {
    int i;

    printf("\n  ========================================\n");
    printf("       ALLOCATION HISTORY (%d)\n", history_count);
    printf("  ========================================\n");

    if (history_count == 0) {
        printf("\n  [INFO] No resources have been allocated yet.\n");
        return;
    }

    printf("\n");
    printf("  +-------+-----+----------------------+----------------+---------+----------------+\n");
    printf("  | %-5s | %-3s | %-20s | %-14s | %-7s | %-14s |\n",
           "Order", "ID", "Location", "Disaster Type", "Urgency", "Resources");
    printf("  +-------+-----+----------------------+----------------+---------+----------------+\n");

    for (i = 0; i < history_count; i++) {
        printf("  | %-5d | %-3d | %-20s | %-14s | %-7d | %-14s |\n",
               history[i].allocation_order,
               history[i].request.id,
               history[i].request.location,
               history[i].request.disaster_type,
               history[i].request.urgency,
               history[i].request.resources_needed);
    }
    printf("  +-------+-----+----------------------+----------------+---------+----------------+\n");
    printf("\n  Total Allocations: %d\n", history_count);
}

/* Display main menu */
void displayMenu(void) {
    printf("\n");
    printf("  ================================================\n");
    printf("     EMERGENCY RESOURCE ALLOCATION SYSTEM\n");
    printf("     Data Structures Project | Priority Queue\n");
    printf("  ================================================\n");
    printf("                                                    \n");
    printf("    1.  Add Emergency Request                      \n");
    printf("    2.  Allocate Resources (Highest Priority)      \n");
    printf("    3.  View All Pending Requests                  \n");
    printf("    4.  View Next Priority Request                 \n");
    printf("    5.  View Allocation History                    \n");
    printf("    6.  Exit                                       \n");
    printf("                                                    \n");
    printf("  ================================================\n");
    printf("    Pending: %d  |  Allocated: %d                  \n", queue.size, history_count);
    printf("  ================================================\n");
    printf("\n  Enter your choice (1-6): ");
}

/* ===========================================================
 *  MAIN FUNCTION
 * =========================================================== */
int main() {
    int choice;

    /* Initialize the priority queue */
    initQueue(&queue);

    printf("\n");
    printf("  ************************************************\n");
    printf("  *                                              *\n");
    printf("  *   EMERGENCY RESOURCE ALLOCATION SYSTEM       *\n");
    printf("  *                                              *\n");
    printf("  *   BCA 2nd Semester - Minor Project           *\n");
    printf("  *   Data Structures Using C                    *\n");
    printf("  *   Priority Queue (Max-Heap) Implementation   *\n");
    printf("  *                                              *\n");
    printf("  ************************************************\n");

    do {
        displayMenu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addRequest();
                break;
            case 2:
                allocateResources();
                break;
            case 3:
                viewAllRequests();
                break;
            case 4:
                viewNextPriority();
                break;
            case 5:
                viewHistory();
                break;
            case 6:
                printf("\n  ================================================\n");
                printf("    Thank you for using the Emergency Resource\n");
                printf("    Allocation System. Stay safe!\n");
                printf("  ================================================\n\n");
                break;
            default:
                printf("\n  [ERROR] Invalid choice. Please enter 1-6.\n");
        }

    } while (choice != 6);

    return 0;
}
