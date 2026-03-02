/*
 * ============================================================
 *  Smart Task Manager  —  C++ CLI App
 *  Concepts: OOP, STL (vector/map), File I/O, Clean Architecture
 * ============================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>
#include <ctime>
#include <limits>

// ─────────────────────────────────────────────
//  ENUMS & HELPERS
// ─────────────────────────────────────────────
enum class Priority { LOW = 1, MEDIUM = 2, HIGH = 3 };

std::string priorityToStr(Priority p) {
    switch (p) {
        case Priority::LOW:    return "LOW";
        case Priority::MEDIUM: return "MEDIUM";
        case Priority::HIGH:   return "HIGH";
    }
    return "UNKNOWN";
}

Priority strToPriority(const std::string& s) {
    if (s == "HIGH")   return Priority::HIGH;
    if (s == "MEDIUM") return Priority::MEDIUM;
    return Priority::LOW;
}

std::string colorPriority(Priority p) {
    switch (p) {
        case Priority::HIGH:   return "\033[31m" + priorityToStr(p) + "\033[0m"; // red
        case Priority::MEDIUM: return "\033[33m" + priorityToStr(p) + "\033[0m"; // yellow
        case Priority::LOW:    return "\033[32m" + priorityToStr(p) + "\033[0m"; // green
    }
    return priorityToStr(p);
}

// ─────────────────────────────────────────────
//  TASK CLASS
// ─────────────────────────────────────────────
class Task {
public:
    static int nextId;

    int         id;
    std::string title;
    std::string deadline;   // "YYYY-MM-DD" or "none"
    Priority    priority;
    bool        completed;

    Task() = default;

    Task(const std::string& title, const std::string& deadline, Priority priority)
        : id(nextId++), title(title), deadline(deadline),
          priority(priority), completed(false) {}

    // Serialize to CSV line
    std::string serialize() const {
        return std::to_string(id) + "|" + title + "|" + deadline + "|" +
               priorityToStr(priority) + "|" + (completed ? "1" : "0");
    }

    // Deserialize from CSV line
    static Task deserialize(const std::string& line) {
        std::istringstream ss(line);
        std::string token;
        Task t;
        std::getline(ss, token, '|'); t.id       = std::stoi(token);
        std::getline(ss, token, '|'); t.title     = token;
        std::getline(ss, token, '|'); t.deadline  = token;
        std::getline(ss, token, '|'); t.priority  = strToPriority(token);
        std::getline(ss, token, '|'); t.completed = (token == "1");
        if (t.id >= nextId) nextId = t.id + 1;
        return t;
    }

    void print(bool showIndex = false, int index = 0) const {
        std::string status = completed ? "\033[32m[✓]\033[0m" : "\033[90m[ ]\033[0m";
        if (showIndex)
            std::cout << std::setw(3) << index << ". ";
        else
            std::cout << "    ";
        std::cout << status << " [ID:" << std::setw(3) << id << "] "
                  << std::left << std::setw(30) << title
                  << "  Priority: " << std::setw(8) << colorPriority(priority)
                  << "  Deadline: " << deadline << "\n";
    }
};

int Task::nextId = 1;

// ─────────────────────────────────────────────
//  TASK MANAGER  (core logic)
// ─────────────────────────────────────────────
class TaskManager {
    std::vector<Task> tasks;
    const std::string SAVE_FILE = "tasks.dat";
    bool dirty = false;   // unsaved changes?

    void autoSave() {
        saveToFile();
        dirty = false;
    }

public:
    TaskManager() { loadFromFile(); }
    ~TaskManager() { if (dirty) autoSave(); }

    // ── CRUD ──────────────────────────────────
    void addTask(const std::string& title, const std::string& deadline, Priority p) {
        tasks.emplace_back(title, deadline, p);
        dirty = true;
        autoSave();
        std::cout << "\033[32m✓ Task added (ID=" << tasks.back().id << ")\033[0m\n";
    }

    bool completeTask(int id) {
        for (auto& t : tasks) {
            if (t.id == id && !t.completed) {
                t.completed = true;
                dirty = true;
                autoSave();
                return true;
            }
        }
        return false;
    }

    bool deleteTask(int id) {
        auto it = std::remove_if(tasks.begin(), tasks.end(),
                                 [id](const Task& t){ return t.id == id; });
        if (it == tasks.end()) return false;
        tasks.erase(it, tasks.end());
        dirty = true;
        autoSave();
        return true;
    }

    // ── DISPLAY ───────────────────────────────
    void showAll(bool pendingOnly = false) const {
        std::cout << "\n\033[1;34m";
        std::cout << (pendingOnly ? "  📋 PENDING TASKS" : "  📋 ALL TASKS");
        std::cout << "\033[0m\n";
        std::cout << std::string(72, '-') << "\n";

        // Sort by priority (HIGH first), then by id
        std::vector<Task> sorted = tasks;
        std::sort(sorted.begin(), sorted.end(), [](const Task& a, const Task& b){
            if (a.completed != b.completed) return !a.completed; // pending first
            if (a.priority  != b.priority)  return a.priority > b.priority;
            return a.id < b.id;
        });

        int count = 0;
        for (const auto& t : sorted) {
            if (pendingOnly && t.completed) continue;
            t.print();
            ++count;
        }
        if (count == 0)
            std::cout << "    \033[90m(no tasks)\033[0m\n";
        std::cout << std::string(72, '-') << "\n";
        std::cout << "  Total shown: " << count << "\n\n";
    }

    void showStats() const {
        int total = tasks.size();
        int done  = std::count_if(tasks.begin(), tasks.end(), [](const Task& t){ return t.completed; });
        std::map<Priority, int> pCount;
        for (const auto& t : tasks) if (!t.completed) pCount[t.priority]++;

        std::cout << "\n\033[1;36m  📊 STATISTICS\033[0m\n";
        std::cout << std::string(40, '-') << "\n";
        std::cout << "  Total tasks   : " << total << "\n";
        std::cout << "  Completed     : " << done  << "\n";
        std::cout << "  Pending       : " << (total - done) << "\n";
        std::cout << "  Pending HIGH  : " << pCount[Priority::HIGH]   << "\n";
        std::cout << "  Pending MEDIUM: " << pCount[Priority::MEDIUM] << "\n";
        std::cout << "  Pending LOW   : " << pCount[Priority::LOW]    << "\n";
        std::cout << std::string(40, '-') << "\n\n";
    }

    // ── FILE I/O ──────────────────────────────
    void saveToFile() const {
        std::ofstream ofs(SAVE_FILE);
        if (!ofs) { std::cerr << "⚠  Could not save to " << SAVE_FILE << "\n"; return; }
        for (const auto& t : tasks) ofs << t.serialize() << "\n";
    }

    void loadFromFile() {
        std::ifstream ifs(SAVE_FILE);
        if (!ifs) return;   // first run — no file yet
        std::string line;
        while (std::getline(ifs, line))
            if (!line.empty()) tasks.push_back(Task::deserialize(line));
    }
};

// ─────────────────────────────────────────────
//  UI HELPERS
// ─────────────────────────────────────────────
void clearInput() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void printMenu() {
    std::cout << "\n\033[1;35m╔══════════════════════════════════╗\033[0m\n";
    std::cout << "\033[1;35m║    🗂  SMART TASK MANAGER         ║\033[0m\n";
    std::cout << "\033[1;35m╚══════════════════════════════════╝\033[0m\n";
    std::cout << "  1. ➕  Add Task\n";
    std::cout << "  2. ✅  Complete Task\n";
    std::cout << "  3. 🗑   Delete Task\n";
    std::cout << "  4. 📋  Show Pending Tasks\n";
    std::cout << "  5. 📋  Show All Tasks\n";
    std::cout << "  6. 📊  Statistics\n";
    std::cout << "  7. 💾  Save & Exit\n";
    std::cout << "  Choice: ";
}

Priority choosePriority() {
    std::cout << "  Priority (1=LOW, 2=MEDIUM, 3=HIGH): ";
    int p; std::cin >> p; clearInput();
    switch (p) {
        case 3: return Priority::HIGH;
        case 2: return Priority::MEDIUM;
        default: return Priority::LOW;
    }
}

// ─────────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────────
int main() {
    TaskManager mgr;
    int choice = 0;

    while (true) {
        printMenu();
        std::cin >> choice;
        clearInput();

        switch (choice) {
            case 1: {
                std::cout << "  Task title   : "; std::string title; std::getline(std::cin, title);
                std::cout << "  Deadline (YYYY-MM-DD or 'none'): "; std::string dl; std::getline(std::cin, dl);
                Priority p = choosePriority();
                mgr.addTask(title, dl.empty() ? "none" : dl, p);
                break;
            }
            case 2: {
                std::cout << "  Enter Task ID to complete: "; int id; std::cin >> id; clearInput();
                std::cout << (mgr.completeTask(id) ? "\033[32m✓ Marked complete!\033[0m\n"
                                                   : "\033[31m✗ ID not found or already done.\033[0m\n");
                break;
            }
            case 3: {
                std::cout << "  Enter Task ID to delete: "; int id; std::cin >> id; clearInput();
                std::cout << (mgr.deleteTask(id) ? "\033[32m✓ Task deleted.\033[0m\n"
                                                 : "\033[31m✗ ID not found.\033[0m\n");
                break;
            }
            case 4: mgr.showAll(true);  break;
            case 5: mgr.showAll(false); break;
            case 6: mgr.showStats();    break;
            case 7:
                mgr.saveToFile();
                std::cout << "\033[32m💾 Saved. Goodbye!\033[0m\n";
                return 0;
            default:
                std::cout << "\033[31m  Invalid choice.\033[0m\n";
        }
    }
}
