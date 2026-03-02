# Smart Task Manager

A terminal-based task management application written in C++. Built to demonstrate core systems programming concepts including OOP, STL containers, file I/O, and clean architecture.

![C++](https://img.shields.io/badge/C++-17-00599C?style=flat&logo=cplusplus&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-macOS%20%7C%20Linux%20%7C%20Windows-lightgrey?style=flat)
![License](https://img.shields.io/badge/License-MIT-green?style=flat)

---

## Features

- Add tasks with a title, deadline, and priority level (HIGH / MEDIUM / LOW)
- Mark tasks as complete
- Delete tasks by ID
- View pending or all tasks, sorted by priority
- Statistics dashboard showing task breakdown
- Auto-save to `tasks.dat` after every change — data persists across sessions
- Color-coded priority output in the terminal

---

## Demo
```
╔══════════════════════════════════╗
║    🗂  SMART TASK MANAGER         ║
╚══════════════════════════════════╝
  1. ➕  Add Task
  2. ✅  Complete Task
  3. 🗑   Delete Task
  4. 📋  Show Pending Tasks
  5. 📋  Show All Tasks
  6. 📊  Statistics
  7. 💾  Save & Exit
  Choice:
```
```
    [ ] [ID:  1] Write resume          Priority: HIGH    Deadline: 2026-04-01
    [ ] [ID:  2] Study DSA             Priority: MEDIUM  Deadline: 2026-03-15
    [✓] [ID:  3] Setup GitHub          Priority: LOW     Deadline: none
```

---

## Concepts Demonstrated

| Concept | Implementation |
|---|---|
| OOP | `Task` class with serialize / deserialize methods |
| STL | `vector<Task>`, `map<Priority, int>` |
| File I/O | Auto-save and load from `tasks.dat` |
| Sorting | Priority and completion-aware task ordering |
| Clean Architecture | Model, logic, and UI layers separated |

---

## Getting Started

### Prerequisites

A C++17 compatible compiler.

- **macOS** — `xcode-select --install`
- **Linux** — `sudo apt install g++`
- **Windows** — Install [MinGW-w64](https://www.mingw-w64.org/)

### Compile and Run
```bash
g++ -std=c++17 -o task_manager task_manager.cpp
./task_manager
```

On Windows:
```bash
g++ -std=c++17 -o task_manager task_manager.cpp
task_manager.exe
```

---

## File Persistence

Tasks are automatically saved to `tasks.dat` in the same directory after every operation. The file is loaded on startup, so your tasks are preserved between sessions. No manual saving required.

---

## Author

**Fahed Waheed** — [GitHub](https://github.com/FahedWaheed8)
