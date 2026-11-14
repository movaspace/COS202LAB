# Smart Clinic Appointment & Patient Management System (SCAPMS)

**Course**: COS202 Computer Programming II    
**Language**: C++   

---

## 1. Project Overview

The **Smart Clinic Appointment & Patient Management System (SCAPMS)** is a desktop clinical administration system developed in native C++ using the Windows Win32 API. It automates outpatient clinic operations including patient registration, categorization, appointment booking, consultation queue management, undo history, algorithmic searching and sorting, and persistent CSV storage.

---

## 2. Key Features

* **Object Oriented Patient Hierarchy**:
  * Abstract base class `PatientRecord` defining shared clinical properties and pure virtual methods.
  * Derived `GeneralPatient`: standard outpatient appointments and department routing.
  * Derived `EmergencyPatient`: acute arrivals with triage severity (1 to 5) and emergency observations.
  * Derived `FollowUpPatient`: recurring visits tracking prior physician and diagnostic history.
  * `Appointable` Interface: contract for booking and cancelling scheduled appointments.
* **Appointment Scheduling & Conflict Prevention**:
  * Book, cancel, and mark appointments as completed.
  * Real-time duplicate collision check prevents scheduling the same patient twice at the identical date and time slot.
* **Consultation Waiting Queue (FIFO)**:
  * Strict First-In First-Out waiting line using `std::queue`.
  * Sequential admission for consultations via the "Call Next Patient" control.
* **Administrative Undo System (LIFO)**:
  * Stack-based transaction rollback using `std::stack`.
  * Reverts additions, deletions, updates, and appointment modifications seamlessly.
* **Recency Access Cache**:
  * Fixed-size 10-slot memory array caching the most recently registered and inspected records.
* **Self Implemented Search & Sort Engine**:
  * Searching: Linear Search (by ID), Binary Search (by ID on sorted data), Recursive Search (by Name).
  * Sorting: Selection Sort, Insertion Sort, Quick Sort (by Name, ID, or Age).
  * Built manually without using `std::sort` or standard search shortcuts.
* **Native Win32 Graphical User Interface**:
  * Tabbed multi-panel design: *Patient Records*, *Appointments*, *Search & Sort*, *Admin Panel*.
  * Responsive layout recalculation on window resize (`WM_SIZE`).
  * Built-in native tooltip system providing real-time hover guidance on buttons and controls.
  * Background dashboard timer updating real-time clinic statistics.

---

## 3. Repository File Structure

```text
COS202LAB/
├── model/
│   ├── PatientRecord.h / .cpp       # Abstract base class
│   ├── GeneralPatient.h / .cpp      # General outpatient class
│   ├── EmergencyPatient.h / .cpp    # Emergency patient class
│   ├── FollowUpPatient.h / .cpp     # Follow-up patient class
│   ├── Appointable.h                # Scheduling interface
│   ├── Appointment.h / .cpp         # Appointment entity class
│   └── ClinicDatabase.h / .cpp      # Database, queue, undo stack, cache
├── controller/
│   ├── SearchEngine.h / .cpp        # Manual search and sort algorithms
│   ├── AppointmentController.h/.cpp # Scheduling business logic
│   └── ClinicManager.h / .cpp       # System facade coordinator
├── gui/
│   ├── LayoutManager.h / .cpp       # Dynamic layout and tooltip engine
│   ├── PatientPanel.h / .cpp        # Patient management UI tab
│   ├── AppointmentPanel.h / .cpp    # Appointment booking UI tab
│   ├── SearchPanel.h / .cpp         # Search and sort UI tab
│   ├── AdminPanel.h / .cpp          # Queue, undo, and reports UI tab
│   └── MainWindow.h / .cpp          # Top window, tabs, and menu dispatch
├── utils/
│   ├── IDGenerator.h / .cpp         # Auto-incrementing unique IDs
│   ├── FileHandler.h / .cpp         # CSV serialization and ingestion
│   └── Exceptions.h                 # Domain-specific exception classes
├── main.cpp                         # WinMain application entry point
├── build.bat                        # Batch compilation script
├── Makefile                         # GNU Make build configuration
├── patients.csv                     # Sample patients data
├── patients.csv.appts.csv           # Sample appointments data
└── README.md                        # Documentation
```

---

## 4. Compilation & Execution

### Prerequisites
* Windows 10 or Windows 11
* MinGW-w64 GCC with C++17 support (e.g., w64devkit)

### Building via Batch Script
Open Command Prompt in the project folder and run:
```cmd
./build.bat
```

### Running the Application
```cmd
./scapms.exe
```

---

## 5. Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl + S` | Save current database to CSV files |
| `Ctrl + L` | Load database from CSV file |
| `Ctrl + F` | Jump directly to Search & Sort tab |
| `Ctrl + Z` | Undo last administrative operation |
| `Ctrl + Q` | Close application |
| `F1` | Open User Guide & Tooltips documentation |

---
