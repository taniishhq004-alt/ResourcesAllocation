/* ============================================================
   EMERGENCY RESOURCE ALLOCATION SYSTEM
   Priority Queue (Max-Heap) — JavaScript Implementation
   ============================================================ */

// ---- Priority Queue Class (Max-Heap) ----
class PriorityQueue {
    constructor() {
        this.heap = [];
    }

    // Get parent index
    parent(i) { return Math.floor((i - 1) / 2); }

    // Get left child index
    left(i) { return 2 * i + 1; }

    // Get right child index
    right(i) { return 2 * i + 2; }

    // Swap two elements
    swap(i, j) {
        [this.heap[i], this.heap[j]] = [this.heap[j], this.heap[i]];
    }

    // Bubble up to maintain max-heap property after insertion
    heapifyUp(index) {
        while (index > 0) {
            const parentIdx = this.parent(index);
            if (this.heap[index].urgency > this.heap[parentIdx].urgency) {
                this.swap(index, parentIdx);
                index = parentIdx;
            } else {
                break;
            }
        }
    }

    // Heapify down to maintain max-heap property after extraction
    heapifyDown(index) {
        const size = this.heap.length;
        let largest = index;
        const l = this.left(index);
        const r = this.right(index);

        if (l < size && this.heap[l].urgency > this.heap[largest].urgency)
            largest = l;
        if (r < size && this.heap[r].urgency > this.heap[largest].urgency)
            largest = r;

        if (largest !== index) {
            this.swap(index, largest);
            this.heapifyDown(largest);
        }
    }

    // Insert a new request into the queue
    insert(request) {
        this.heap.push(request);
        this.heapifyUp(this.heap.length - 1);
    }

    // Extract the highest priority request
    extractMax() {
        if (this.heap.length === 0) return null;
        if (this.heap.length === 1) return this.heap.pop();

        const max = this.heap[0];
        this.heap[0] = this.heap.pop();
        this.heapifyDown(0);
        return max;
    }

    // Peek at the highest priority request
    peek() {
        return this.heap.length > 0 ? this.heap[0] : null;
    }

    // Get queue size
    size() {
        return this.heap.length;
    }

    // Get all items sorted by urgency (for display)
    getAllSorted() {
        return [...this.heap].sort((a, b) => b.urgency - a.urgency);
    }
}

// ---- Application State ----
const queue = new PriorityQueue();
const allocationHistory = [];
let nextId = 1;
let totalRequests = 0;

// ---- Disaster Type Emojis ----
const disasterEmojis = {
    'Flood': '🌊',
    'Earthquake': '🏚️',
    'Fire': '🔥',
    'Accident': '🚨',
    'Cyclone': '🌀',
    'Landslide': '⛰️'
};

// ---- DOM Elements ----
const form = document.getElementById('requestForm');
const urgencySlider = document.getElementById('urgency');
const urgencyValueEl = document.getElementById('urgencyValue');
const urgencyLabelEl = document.getElementById('urgencyLabel');
const queueListEl = document.getElementById('queueList');
const historyListEl = document.getElementById('historyList');
const allocateBtn = document.getElementById('allocateBtn');
const statPending = document.getElementById('statPending');
const statAllocated = document.getElementById('statAllocated');
const statTotal = document.getElementById('statTotal');
const statAvgUrgency = document.getElementById('statAvgUrgency');

// ---- Urgency Slider Update ----
function updateUrgencyDisplay() {
    const val = parseInt(urgencySlider.value);
    urgencyValueEl.textContent = val;

    let label, cssClass, color;
    if (val >= 8) {
        label = 'CRITICAL';
        cssClass = 'urgency-label--critical';
        color = '#ff2d78';
    } else if (val >= 5) {
        label = 'HIGH';
        cssClass = 'urgency-label--high';
        color = '#a855f7';
    } else if (val >= 3) {
        label = 'MODERATE';
        cssClass = 'urgency-label--moderate';
        color = '#e9c46a';
    } else {
        label = 'LOW';
        cssClass = 'urgency-label--low';
        color = '#06d6f2';
    }

    urgencyLabelEl.textContent = label;
    urgencyLabelEl.className = 'urgency-label ' + cssClass;
    urgencyValueEl.style.color = color;
}

urgencySlider.addEventListener('input', updateUrgencyDisplay);

// ---- Form Submission ----
form.addEventListener('submit', function(e) {
    e.preventDefault();

    const request = {
        id: nextId++,
        location: document.getElementById('location').value.trim(),
        disasterType: document.getElementById('disasterType').value,
        peopleAffected: parseInt(document.getElementById('peopleAffected').value),
        urgency: parseInt(urgencySlider.value),
        resourcesNeeded: document.getElementById('resourcesNeeded').value
    };

    if (!request.location) {
        showToast('Please enter a location name.', 'error');
        return;
    }

    queue.insert(request);
    totalRequests++;

    renderQueue();
    updateStats();
    showToast(`Request #${request.id} added — ${request.location}`, 'success');
    form.reset();
    urgencySlider.value = 5;
    updateUrgencyDisplay();
});

// ---- Allocate Resources ----
allocateBtn.addEventListener('click', function() {
    if (queue.size() === 0) return;

    const request = queue.extractMax();
    allocationHistory.unshift({
        ...request,
        allocationOrder: allocationHistory.length + 1,
        timestamp: new Date()
    });

    renderQueue();
    renderHistory();
    updateStats();
    showToast(`Resources dispatched to ${request.location} (Urgency: ${request.urgency})`, 'allocate');
});

// ---- Render Queue ----
function renderQueue() {
    allocateBtn.disabled = queue.size() === 0;

    if (queue.size() === 0) {
        queueListEl.innerHTML = `
            <div class="empty-state">
                <div class="empty-state__icon">📭</div>
                <div class="empty-state__text">No pending emergency requests.<br>Add a request using the form.</div>
            </div>
        `;
        return;
    }

    const sorted = queue.getAllSorted();
    queueListEl.innerHTML = sorted.map((req, i) => {
        const dotClass = getDotClass(req.urgency);
        const emoji = disasterEmojis[req.disasterType] || '⚠️';

        return `
            <li class="queue-item" id="queue-item-${req.id}">
                <span class="queue-item__rank">${i + 1}</span>
                <span class="queue-item__urgency-dot ${dotClass}"></span>
                <div class="queue-item__info">
                    <div class="queue-item__location">${emoji} ${escapeHtml(req.location)}</div>
                    <div class="queue-item__meta">${req.disasterType} · ${req.peopleAffected} affected · ${req.resourcesNeeded}</div>
                </div>
                <span class="queue-item__urgency-badge" style="color:${getUrgencyColor(req.urgency)}">${req.urgency}</span>
            </li>
        `;
    }).join('');
}

// ---- Render History ----
function renderHistory() {
    if (allocationHistory.length === 0) {
        historyListEl.innerHTML = `
            <div class="empty-state">
                <div class="empty-state__icon">📋</div>
                <div class="empty-state__text">No allocations yet.</div>
            </div>
        `;
        return;
    }

    historyListEl.innerHTML = allocationHistory.map((item, i) => {
        const emoji = disasterEmojis[item.disasterType] || '⚠️';
        const time = item.timestamp.toLocaleTimeString();

        return `
            <li class="history-item">
                <span class="history-item__order">${allocationHistory.length - i}</span>
                <div class="history-item__info">
                    <div class="history-item__location">${emoji} ${escapeHtml(item.location)}</div>
                    <div class="history-item__meta">Urgency ${item.urgency} · ${item.resourcesNeeded} · ${time}</div>
                </div>
                <span class="history-item__check">✓</span>
            </li>
        `;
    }).join('');
}

// ---- Update Stats ----
function updateStats() {
    const pending = queue.size();
    const allocated = allocationHistory.length;

    statPending.textContent = pending;
    statAllocated.textContent = allocated;
    statTotal.textContent = totalRequests;

    // Calculate average urgency of pending requests
    if (pending > 0) {
        const sorted = queue.getAllSorted();
        const avg = sorted.reduce((sum, r) => sum + r.urgency, 0) / pending;
        statAvgUrgency.textContent = avg.toFixed(1);
    } else {
        statAvgUrgency.textContent = '—';
    }
}

// ---- Helpers ----
function getDotClass(urgency) {
    if (urgency >= 8) return 'queue-item__urgency-dot--critical';
    if (urgency >= 5) return 'queue-item__urgency-dot--high';
    if (urgency >= 3) return 'queue-item__urgency-dot--moderate';
    return 'queue-item__urgency-dot--low';
}

function getUrgencyColor(urgency) {
    if (urgency >= 8) return '#ff2d78';
    if (urgency >= 5) return '#a855f7';
    if (urgency >= 3) return '#e9c46a';
    return '#06d6f2';
}

function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

// ---- Toast Notifications ----
function showToast(message, type = 'success') {
    // Remove existing toasts
    document.querySelectorAll('.toast').forEach(t => t.remove());

    const toast = document.createElement('div');
    toast.className = `toast toast--${type}`;
    toast.textContent = message;
    document.body.appendChild(toast);

    setTimeout(() => toast.remove(), 3200);
}

// ---- Initialize ----
renderQueue();
renderHistory();
updateStats();
updateUrgencyDisplay();
