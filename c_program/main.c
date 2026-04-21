/*
 * ============================================================
 *  EMERGENCY RESOURCE ALLOCATION SYSTEM
 *  BCA 2nd Semester - Minor Project (Data Structures Using C)
 * ============================================================
 *
 *  Data Structures Used:
 *    1. Priority Queue (Max-Heap) - Request prioritization
 *    2. Singly Linked List        - Allocation history
 *    3. Hash Table (Chaining)     - Search by location
 *    4. Stack                     - Undo last allocation
 *
 *  Features: File I/O, Search, Sort, Statistics, Undo
 *  Compiled with: gcc main.c -o resource_alloc
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ---- Constants ---- */
#define MAX_QUEUE    100
#define HASH_SIZE    53
#define MAX_STACK    50
#define DATA_FILE    "emergency_data.txt"

/* ============================================================
 *  DATA STRUCTURE 1: Emergency Request (Base Structure)
 * ============================================================ */
typedef struct {
    int    id;
    char   location[50];
    char   disaster_type[30];
    int    people_affected;
    int    urgency;
    char   resources_needed[50];
} EmergencyRequest;

/* ============================================================
 *  DATA STRUCTURE 2: Max-Heap Priority Queue
 * ============================================================ */
typedef struct {
    EmergencyRequest data[MAX_QUEUE];
    int size;
} PriorityQueue;

/* ============================================================
 *  DATA STRUCTURE 3: Singly Linked List (Allocation History)
 * ============================================================ */
typedef struct HistoryNode {
    EmergencyRequest    request;
    int                 allocation_order;
    struct HistoryNode *next;
} HistoryNode;

typedef struct {
    HistoryNode *head;
    int          count;
} HistoryList;

/* ============================================================
 *  DATA STRUCTURE 4: Hash Table with Chaining (Location Search)
 * ============================================================ */
typedef struct HashNode {
    EmergencyRequest   request;
    int                is_pending;
    struct HashNode   *next;
} HashNode;

typedef struct {
    HashNode *buckets[HASH_SIZE];
} HashTable;

/* ============================================================
 *  DATA STRUCTURE 5: Stack (Undo Last Allocation)
 * ============================================================ */
typedef struct {
    EmergencyRequest data[MAX_STACK];
    int top;
} UndoStack;

/* ---- Global Variables ---- */
PriorityQueue  queue;
HistoryList    history;
HashTable      locationMap;
UndoStack      undoStack;
int            next_id        = 1;
int            allocation_num = 0;
/* ============================================================
 *  INITIALIZATION FUNCTIONS
 * ============================================================ */
void initQueue(PriorityQueue *pq) { pq->size = 0; }
void initHistory(HistoryList *h) { h->head = NULL; h->count = 0; }
void initStack(UndoStack *s) { s->top = -1; }
void initHashTable(HashTable *ht) {
    int i;
    for (i = 0; i < HASH_SIZE; i++) ht->buckets[i] = NULL;
}

/* ============================================================
 *  PRIORITY QUEUE (MAX-HEAP) OPERATIONS
 * ============================================================ */
void swap(EmergencyRequest *a, EmergencyRequest *b) {
    EmergencyRequest temp = *a; *a = *b; *b = temp;
}

void heapifyUp(PriorityQueue *pq, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (pq->data[index].urgency > pq->data[parent].urgency) {
            swap(&pq->data[index], &pq->data[parent]);
            index = parent;
        } else break;
    }
}

void heapifyDown(PriorityQueue *pq, int index) {
    int largest = index, left = 2*index+1, right = 2*index+2;
    if (left < pq->size && pq->data[left].urgency > pq->data[largest].urgency)
        largest = left;
    if (right < pq->size && pq->data[right].urgency > pq->data[largest].urgency)
        largest = right;
    if (largest != index) {
        swap(&pq->data[index], &pq->data[largest]);
        heapifyDown(pq, largest);
    }
}

int pqInsert(PriorityQueue *pq, EmergencyRequest req) {
    if (pq->size >= MAX_QUEUE) {
        printf("\n  [ERROR] Queue full!\n");
        return 0;
    }
    pq->data[pq->size] = req;
    heapifyUp(pq, pq->size);
    pq->size++;
    return 1;
}

int pqExtractMax(PriorityQueue *pq, EmergencyRequest *result) {
    if (pq->size <= 0) return 0;
    *result = pq->data[0];
    pq->data[0] = pq->data[pq->size - 1];
    pq->size--;
    if (pq->size > 0) heapifyDown(pq, 0);
    return 1;
}

int pqPeek(PriorityQueue *pq, EmergencyRequest *result) {
    if (pq->size <= 0) return 0;
    *result = pq->data[0];
    return 1;
}

/* ============================================================
 *  LINKED LIST OPERATIONS (Allocation History)
 * ============================================================ */
void historyAdd(HistoryList *h, EmergencyRequest req, int order) {
    HistoryNode *node = (HistoryNode*)malloc(sizeof(HistoryNode));
    if (!node) { printf("\n  [ERROR] Memory allocation failed!\n"); return; }
    node->request = req;
    node->allocation_order = order;
    node->next = NULL;
    if (h->head == NULL) {
        h->head = node;
    } else {
        HistoryNode *cur = h->head;
        while (cur->next) cur = cur->next;
        cur->next = node;
    }
    h->count++;
}

void historyDisplay(HistoryList *h) {
    HistoryNode *cur = h->head;
    if (!cur) { printf("\n  [INFO] No allocation history yet.\n"); return; }
    printf("\n  +-------+-----+----------------------+----------------+---------+----------------+\n");
    printf("  | %-5s | %-3s | %-20s | %-14s | %-7s | %-14s |\n",
           "Order","ID","Location","Disaster","Urgency","Resources");
    printf("  +-------+-----+----------------------+----------------+---------+----------------+\n");
    while (cur) {
        printf("  | %-5d | %-3d | %-20s | %-14s | %-7d | %-14s |\n",
               cur->allocation_order, cur->request.id, cur->request.location,
               cur->request.disaster_type, cur->request.urgency, cur->request.resources_needed);
        cur = cur->next;
    }
    printf("  +-------+-----+----------------------+----------------+---------+----------------+\n");
    printf("  Total Allocations: %d\n", h->count);
}

void historyFree(HistoryList *h) {
    HistoryNode *cur = h->head, *tmp;
    while (cur) { tmp = cur; cur = cur->next; free(tmp); }
    h->head = NULL; h->count = 0;
}

/* ============================================================
 *  HASH TABLE OPERATIONS (Location-Based Search)
 * ============================================================ */
unsigned int hashFunction(const char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + tolower(c);
    }
    return hash % HASH_SIZE;
}

void htInsert(HashTable *ht, EmergencyRequest req, int pending) {
    unsigned int idx = hashFunction(req.location);
    HashNode *node = (HashNode*)malloc(sizeof(HashNode));
    if (!node) return;
    node->request = req;
    node->is_pending = pending;
    node->next = ht->buckets[idx];
    ht->buckets[idx] = node;
}

void htSearch(HashTable *ht, const char *location) {
    unsigned int idx = hashFunction(location);
    HashNode *cur = ht->buckets[idx];
    int found = 0;
    printf("\n  Search Results for \"%s\":\n", location);
    printf("  --------------------------------------------------------\n");
    while (cur) {
        if (strcmpi(cur->request.location, location) == 0) {
            printf("  ID: %d | %s | %s | Urgency: %d | %s\n",
                   cur->request.id, cur->request.disaster_type,
                   cur->is_pending ? "PENDING" : "ALLOCATED",
                   cur->request.urgency, cur->request.resources_needed);
            found++;
        }
        cur = cur->next;
    }
    if (!found) printf("  No records found for this location.\n");
    else printf("  --------------------------------------------------------\n  Found %d record(s).\n", found);
}

void htFree(HashTable *ht) {
    int i;
    for (i = 0; i < HASH_SIZE; i++) {
        HashNode *cur = ht->buckets[i], *tmp;
        while (cur) { tmp = cur; cur = cur->next; free(tmp); }
        ht->buckets[i] = NULL;
    }
}

/* ============================================================
 *  STACK OPERATIONS (Undo Last Allocation)
 * ============================================================ */
int stackPush(UndoStack *s, EmergencyRequest req) {
    if (s->top >= MAX_STACK - 1) {
        printf("\n  [ERROR] Undo stack is full!\n");
        return 0;
    }
    s->data[++(s->top)] = req;
    return 1;
}

int stackPop(UndoStack *s, EmergencyRequest *result) {
    if (s->top < 0) return 0;
    *result = s->data[(s->top)--];
    return 1;
}

int stackIsEmpty(UndoStack *s) { return s->top < 0; }
/* ============================================================
 *  DISPLAY HELPERS
 * ============================================================ */
void printSeparator(void) {
    printf("  +-----+----------------------+----------------+----------+---------+----------------+\n");
}
void printHeader(void) {
    printSeparator();
    printf("  | %-3s | %-20s | %-14s | %-8s | %-7s | %-14s |\n",
           "ID","Location","Disaster Type","Affected","Urgency","Resources");
    printSeparator();
}
void printRequest(EmergencyRequest *r) {
    printf("  | %-3d | %-20s | %-14s | %-8d | %-7d | %-14s |\n",
           r->id, r->location, r->disaster_type, r->people_affected,
           r->urgency, r->resources_needed);
}
const char* getUrgencyLabel(int u) {
    if (u >= 8) return "CRITICAL";
    if (u >= 5) return "HIGH";
    if (u >= 3) return "MODERATE";
    return "LOW";
}
void printUrgencyBar(int u) {
    int i;
    printf("  Urgency: [");
    for (i = 0; i < 10; i++) printf(i < u ? "#" : "-");
    printf("] %d/10 (%s)\n", u, getUrgencyLabel(u));
}

/* ============================================================
 *  FILE I/O OPERATIONS (Persistence)
 * ============================================================ */
void saveToFile(void) {
    FILE *fp = fopen(DATA_FILE, "w");
    int i;
    HistoryNode *cur;
    if (!fp) { printf("\n  [ERROR] Cannot open file for writing!\n"); return; }

    /* Save pending queue */
    fprintf(fp, "PENDING %d\n", queue.size);
    for (i = 0; i < queue.size; i++) {
        fprintf(fp, "%d|%s|%s|%d|%d|%s\n",
                queue.data[i].id, queue.data[i].location,
                queue.data[i].disaster_type, queue.data[i].people_affected,
                queue.data[i].urgency, queue.data[i].resources_needed);
    }

    /* Save allocation history */
    fprintf(fp, "HISTORY %d\n", history.count);
    cur = history.head;
    while (cur) {
        fprintf(fp, "%d|%s|%s|%d|%d|%s|%d\n",
                cur->request.id, cur->request.location,
                cur->request.disaster_type, cur->request.people_affected,
                cur->request.urgency, cur->request.resources_needed,
                cur->allocation_order);
        cur = cur->next;
    }

    fprintf(fp, "NEXTID %d\n", next_id);
    fprintf(fp, "ALLOCNUM %d\n", allocation_num);
    fclose(fp);
    printf("\n  [SUCCESS] Data saved to %s\n", DATA_FILE);
}

void loadFromFile(void) {
    FILE *fp = fopen(DATA_FILE, "r");
    char line[256], label[20];
    int count, i;
    EmergencyRequest req;
    int order;

    if (!fp) { printf("\n  [INFO] No saved data found.\n"); return; }

    /* Clear existing data */
    queue.size = 0;
    historyFree(&history);
    htFree(&locationMap);
    initHashTable(&locationMap);

    /* Load pending */
    if (fscanf(fp, "%s %d\n", label, &count) == 2 && strcmp(label, "PENDING") == 0) {
        for (i = 0; i < count; i++) {
            if (fgets(line, sizeof(line), fp)) {
                sscanf(line, "%d|%[^|]|%[^|]|%d|%d|%[^\n]",
                       &req.id, req.location, req.disaster_type,
                       &req.people_affected, &req.urgency, req.resources_needed);
                pqInsert(&queue, req);
                htInsert(&locationMap, req, 1);
            }
        }
    }

    /* Load history */
    if (fscanf(fp, "%s %d\n", label, &count) == 2 && strcmp(label, "HISTORY") == 0) {
        for (i = 0; i < count; i++) {
            if (fgets(line, sizeof(line), fp)) {
                sscanf(line, "%d|%[^|]|%[^|]|%d|%d|%[^|]|%d",
                       &req.id, req.location, req.disaster_type,
                       &req.people_affected, &req.urgency, req.resources_needed, &order);
                historyAdd(&history, req, order);
                htInsert(&locationMap, req, 0);
            }
        }
    }

    if (fscanf(fp, "%s %d", label, &next_id) != 2) next_id = 1;
    if (fscanf(fp, "%s %d", label, &allocation_num) != 2) allocation_num = 0;

    fclose(fp);
    printf("\n  [SUCCESS] Data loaded from %s\n", DATA_FILE);
    printf("  Loaded: %d pending, %d history records\n", queue.size, history.count);
}

/* ============================================================
 *  SORTING FUNCTIONS
 * ============================================================ */
void sortAndDisplay(void) {
    int i, j, choice, count = queue.size;
    EmergencyRequest sorted[MAX_QUEUE];

    if (count == 0) { printf("\n  [INFO] No pending requests to sort.\n"); return; }
    for (i = 0; i < count; i++) sorted[i] = queue.data[i];

    printf("\n  Sort By:\n    1. Urgency (High to Low)\n    2. People Affected (High to Low)\n    3. ID (Low to High)\n  Choice: ");
    scanf("%d", &choice);

    for (i = 0; i < count-1; i++) {
        for (j = 0; j < count-i-1; j++) {
            int doSwap = 0;
            if (choice == 1) doSwap = sorted[j].urgency < sorted[j+1].urgency;
            else if (choice == 2) doSwap = sorted[j].people_affected < sorted[j+1].people_affected;
            else doSwap = sorted[j].id > sorted[j+1].id;
            if (doSwap) { EmergencyRequest t = sorted[j]; sorted[j] = sorted[j+1]; sorted[j+1] = t; }
        }
    }

    printf("\n"); printHeader();
    for (i = 0; i < count; i++) printRequest(&sorted[i]);
    printSeparator();
}

/* ============================================================
 *  STATISTICS REPORT
 * ============================================================ */
void showStatistics(void) {
    int total, i;
    int dCount[6] = {0}; /* flood,earthquake,fire,accident,cyclone,landslide */
    int rCount[5] = {0}; /* food,medical,rescue,shelter,all */
    int totalUrgency = 0, totalPeople = 0, maxUrg = 0;
    HistoryNode *cur;
    const char *dNames[] = {"Flood","Earthquake","Fire","Accident","Cyclone","Landslide"};
    const char *rNames[] = {"Food & Water","Medical","Rescue Team","Shelter","All Resources"};

    total = queue.size + history.count;
    printf("\n  ================================================\n");
    printf("           STATISTICS REPORT\n");
    printf("  ================================================\n\n");
    printf("  Total Requests:     %d\n", total);
    printf("  Pending Requests:   %d\n", queue.size);
    printf("  Allocated:          %d\n", history.count);

    /* Aggregate from pending queue */
    for (i = 0; i < queue.size; i++) {
        totalUrgency += queue.data[i].urgency;
        totalPeople += queue.data[i].people_affected;
        if (queue.data[i].urgency > maxUrg) maxUrg = queue.data[i].urgency;
        if (strcmp(queue.data[i].disaster_type,"Flood")==0) dCount[0]++;
        else if (strcmp(queue.data[i].disaster_type,"Earthquake")==0) dCount[1]++;
        else if (strcmp(queue.data[i].disaster_type,"Fire")==0) dCount[2]++;
        else if (strcmp(queue.data[i].disaster_type,"Accident")==0) dCount[3]++;
        else if (strcmp(queue.data[i].disaster_type,"Cyclone")==0) dCount[4]++;
        else if (strcmp(queue.data[i].disaster_type,"Landslide")==0) dCount[5]++;
        if (strstr(queue.data[i].resources_needed,"Food")) rCount[0]++;
        else if (strstr(queue.data[i].resources_needed,"Medical")) rCount[1]++;
        else if (strstr(queue.data[i].resources_needed,"Rescue")) rCount[2]++;
        else if (strstr(queue.data[i].resources_needed,"Shelter")) rCount[3]++;
        else if (strstr(queue.data[i].resources_needed,"All")) rCount[4]++;
    }
    /* Aggregate from history */
    cur = history.head;
    while (cur) {
        totalUrgency += cur->request.urgency;
        totalPeople += cur->request.people_affected;
        if (cur->request.urgency > maxUrg) maxUrg = cur->request.urgency;
        if (strcmp(cur->request.disaster_type,"Flood")==0) dCount[0]++;
        else if (strcmp(cur->request.disaster_type,"Earthquake")==0) dCount[1]++;
        else if (strcmp(cur->request.disaster_type,"Fire")==0) dCount[2]++;
        else if (strcmp(cur->request.disaster_type,"Accident")==0) dCount[3]++;
        else if (strcmp(cur->request.disaster_type,"Cyclone")==0) dCount[4]++;
        else if (strcmp(cur->request.disaster_type,"Landslide")==0) dCount[5]++;
        if (strstr(cur->request.resources_needed,"Food")) rCount[0]++;
        else if (strstr(cur->request.resources_needed,"Medical")) rCount[1]++;
        else if (strstr(cur->request.resources_needed,"Rescue")) rCount[2]++;
        else if (strstr(cur->request.resources_needed,"Shelter")) rCount[3]++;
        else if (strstr(cur->request.resources_needed,"All")) rCount[4]++;
        cur = cur->next;
    }

    if (total > 0) {
        printf("  Avg Urgency:        %.1f\n", (float)totalUrgency/total);
        printf("  Max Urgency:        %d (%s)\n", maxUrg, getUrgencyLabel(maxUrg));
        printf("  Total People:       %d\n", totalPeople);
    }

    printf("\n  --- Disaster Type Breakdown ---\n");
    for (i = 0; i < 6; i++)
        if (dCount[i] > 0) printf("    %-12s: %d\n", dNames[i], dCount[i]);

    printf("\n  --- Resource Usage Breakdown ---\n");
    for (i = 0; i < 5; i++)
        if (rCount[i] > 0) printf("    %-14s: %d\n", rNames[i], rCount[i]);
    printf("  ================================================\n");
}

/* ============================================================
 *  SEARCH BY DISASTER TYPE
 * ============================================================ */
void searchByDisasterType(void) {
    int i, choice, found = 0;
    char target[30];
    HistoryNode *cur;

    printf("\n  Select Disaster Type:\n");
    printf("    1. Flood  2. Earthquake  3. Fire\n");
    printf("    4. Accident  5. Cyclone  6. Landslide\n  Choice: ");
    scanf("%d", &choice);
    switch(choice) {
        case 1: strcpy(target,"Flood"); break;
        case 2: strcpy(target,"Earthquake"); break;
        case 3: strcpy(target,"Fire"); break;
        case 4: strcpy(target,"Accident"); break;
        case 5: strcpy(target,"Cyclone"); break;
        case 6: strcpy(target,"Landslide"); break;
        default: printf("  Invalid choice.\n"); return;
    }

    printf("\n  Results for \"%s\":\n", target);
    printHeader();
    for (i = 0; i < queue.size; i++) {
        if (strcmp(queue.data[i].disaster_type, target) == 0) {
            printRequest(&queue.data[i]);
            found++;
        }
    }
    cur = history.head;
    while (cur) {
        if (strcmp(cur->request.disaster_type, target) == 0) {
            printRequest(&cur->request);
            found++;
        }
        cur = cur->next;
    }
    printSeparator();
    printf("  Found %d record(s).\n", found);
}
/* ============================================================
 *  MENU FUNCTIONS
 * ============================================================ */

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
    printf("    1. Flood       2. Earthquake   3. Fire\n");
    printf("    4. Accident    5. Cyclone      6. Landslide\n");
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
    printf("    1. Food & Water   2. Medical Supplies  3. Rescue Team\n");
    printf("    4. Shelter        5. All Resources\n");
    printf("  Choice: ");
    scanf("%d", &choice);
    switch (choice) {
        case 1: strcpy(req.resources_needed, "Food & Water");  break;
        case 2: strcpy(req.resources_needed, "Medical");       break;
        case 3: strcpy(req.resources_needed, "Rescue Team");   break;
        case 4: strcpy(req.resources_needed, "Shelter");       break;
        case 5: strcpy(req.resources_needed, "All Resources"); break;
        default: strcpy(req.resources_needed, "General");      break;
    }

    if (pqInsert(&queue, req)) {
        htInsert(&locationMap, req, 1);
        printf("\n  [SUCCESS] Emergency Request #%d added!\n", req.id);
        printf("  Location : %s\n", req.location);
        printf("  Disaster : %s\n", req.disaster_type);
        printUrgencyBar(req.urgency);
        printf("  Pending  : %d\n", queue.size);
    }
}

/* 2. Allocate Resources */
void allocateResources(void) {
    EmergencyRequest req;

    printf("\n  ========================================\n");
    printf("         ALLOCATE RESOURCES\n");
    printf("  ========================================\n");

    if (pqExtractMax(&queue, &req)) {
        allocation_num++;
        historyAdd(&history, req, allocation_num);
        stackPush(&undoStack, req);

        printf("\n  >> RESOURCES ALLOCATED (Priority #%d) <<\n\n", allocation_num);
        printHeader();
        printRequest(&req);
        printSeparator();
        printUrgencyBar(req.urgency);
        printf("\n  Resources Dispatched: %s\n", req.resources_needed);
        printf("  Remaining Requests : %d\n", queue.size);
    } else {
        printf("\n  [INFO] No pending requests in the queue.\n");
    }
}

/* 3. View All Pending Requests */
void viewAllRequests(void) {
    int i, j, count = queue.size;
    EmergencyRequest sorted[MAX_QUEUE];

    printf("\n  ========================================\n");
    printf("       ALL PENDING REQUESTS (%d)\n", count);
    printf("  ========================================\n");

    if (count == 0) { printf("\n  [INFO] No pending requests.\n"); return; }

    for (i = 0; i < count; i++) sorted[i] = queue.data[i];
    for (i = 0; i < count-1; i++)
        for (j = 0; j < count-i-1; j++)
            if (sorted[j].urgency < sorted[j+1].urgency) {
                EmergencyRequest t = sorted[j]; sorted[j] = sorted[j+1]; sorted[j+1] = t;
            }

    printf("\n"); printHeader();
    for (i = 0; i < count; i++) printRequest(&sorted[i]);
    printSeparator();
    printf("\n  Total Pending: %d request(s)\n", count);
}

/* 4. View Next Priority */
void viewNextPriority(void) {
    EmergencyRequest req;
    printf("\n  ========================================\n");
    printf("       NEXT PRIORITY REQUEST\n");
    printf("  ========================================\n");
    if (pqPeek(&queue, &req)) {
        printf("\n  >> HIGHEST PRIORITY IN QUEUE <<\n\n");
        printHeader(); printRequest(&req); printSeparator();
        printUrgencyBar(req.urgency);
        printf("\n  This request will be served next.\n");
    } else {
        printf("\n  [INFO] No pending requests.\n");
    }
}

/* 6. Search by Location */
void searchByLocation(void) {
    char loc[50];
    printf("\n  Enter Location to Search: ");
    scanf(" %[^\n]", loc);
    htSearch(&locationMap, loc);
}

/* 8. Undo Last Allocation */
void undoLastAllocation(void) {
    EmergencyRequest req;
    HistoryNode *cur, *prev;

    printf("\n  ========================================\n");
    printf("         UNDO LAST ALLOCATION\n");
    printf("  ========================================\n");

    if (stackIsEmpty(&undoStack)) {
        printf("\n  [INFO] Nothing to undo.\n");
        return;
    }

    if (stackPop(&undoStack, &req)) {
        /* Remove last node from linked list history */
        if (history.head) {
            if (history.head->next == NULL) {
                free(history.head);
                history.head = NULL;
            } else {
                cur = history.head;
                while (cur->next && cur->next->next) cur = cur->next;
                free(cur->next);
                cur->next = NULL;
            }
            history.count--;
            allocation_num--;
        }

        /* Re-insert into priority queue */
        pqInsert(&queue, req);

        printf("\n  [SUCCESS] Undo complete! Request #%d restored to queue.\n", req.id);
        printf("  Location: %s | Urgency: %d\n", req.location, req.urgency);
        printf("  Pending: %d | History: %d\n", queue.size, history.count);
    }
}

/* ============================================================
 *  MAIN MENU & MAIN FUNCTION
 * ============================================================ */
void displayMenu(void) {
    printf("\n");
    printf("  ====================================================\n");
    printf("     EMERGENCY RESOURCE ALLOCATION SYSTEM\n");
    printf("     Data Structures Project | BCA 2nd Semester\n");
    printf("  ====================================================\n");
    printf("                                                       \n");
    printf("    1.  Add Emergency Request                          \n");
    printf("    2.  Allocate Resources (Highest Priority)          \n");
    printf("    3.  View All Pending Requests                      \n");
    printf("    4.  View Next Priority Request (Peek)              \n");
    printf("    5.  View Allocation History (Linked List)          \n");
    printf("    6.  Search by Location (Hash Table)                \n");
    printf("    7.  Search by Disaster Type                        \n");
    printf("    8.  Undo Last Allocation (Stack)                   \n");
    printf("    9.  Sort & Display Requests                        \n");
    printf("   10.  Statistics Report                              \n");
    printf("   11.  Save Data to File                              \n");
    printf("   12.  Load Data from File                            \n");
    printf("   13.  Exit                                           \n");
    printf("                                                       \n");
    printf("  ====================================================\n");
    printf("    Pending: %d  |  Allocated: %d  |  Undo: %s        \n",
           queue.size, history.count, stackIsEmpty(&undoStack) ? "No" : "Yes");
    printf("  ====================================================\n");
    printf("\n  Enter your choice (1-13): ");
}

int main() {
    int choice;

    /* Initialize all data structures */
    initQueue(&queue);
    initHistory(&history);
    initHashTable(&locationMap);
    initStack(&undoStack);

    printf("\n");
    printf("  ****************************************************\n");
    printf("  *                                                  *\n");
    printf("  *   EMERGENCY RESOURCE ALLOCATION SYSTEM           *\n");
    printf("  *                                                  *\n");
    printf("  *   BCA 2nd Semester - Minor Project               *\n");
    printf("  *   Data Structures Used:                          *\n");
    printf("  *     1. Priority Queue (Max-Heap)                 *\n");
    printf("  *     2. Linked List (History)                     *\n");
    printf("  *     3. Hash Table (Location Search)              *\n");
    printf("  *     4. Stack (Undo)                              *\n");
    printf("  *   + File I/O, Sorting, Statistics                *\n");
    printf("  *                                                  *\n");
    printf("  ****************************************************\n");

    /* Auto-load saved data */
    loadFromFile();

    do {
        displayMenu();
        scanf("%d", &choice);
        switch (choice) {
            case 1:  addRequest();            break;
            case 2:  allocateResources();     break;
            case 3:  viewAllRequests();       break;
            case 4:  viewNextPriority();      break;
            case 5:  historyDisplay(&history); break;
            case 6:  searchByLocation();      break;
            case 7:  searchByDisasterType();  break;
            case 8:  undoLastAllocation();    break;
            case 9:  sortAndDisplay();        break;
            case 10: showStatistics();        break;
            case 11: saveToFile();            break;
            case 12: loadFromFile();          break;
            case 13:
                saveToFile();
                historyFree(&history);
                htFree(&locationMap);
                printf("\n  ================================================\n");
                printf("    Thank you for using the Emergency Resource\n");
                printf("    Allocation System. Stay safe!\n");
                printf("  ================================================\n\n");
                break;
            default:
                printf("\n  [ERROR] Invalid choice. Please enter 1-13.\n");
        }
    } while (choice != 13);

    return 0;
}
