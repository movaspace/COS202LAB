#include "SearchEngine.h"
#include <algorithm>

namespace scapms {

int SearchEngine::compareRecords(const std::shared_ptr<PatientRecord>& a, const std::shared_ptr<PatientRecord>& b, const std::string& field) {
    if (!a || !b) return 0;
    if (field == "ID" || field == "Patient ID") {
        if (a->getId() < b->getId()) return -1;
        if (a->getId() > b->getId()) return 1;
        return 0;
    }
    if (field == "Age") {
        if (a->getAge() < b->getAge()) return -1;
        if (a->getAge() > b->getAge()) return 1;
        return 0;
    }
    // Default sort field is Name
    if (a->getName() < b->getName()) return -1;
    if (a->getName() > b->getName()) return 1;
    return 0;
}

std::shared_ptr<PatientRecord> SearchEngine::linearSearchById(const std::vector<std::shared_ptr<PatientRecord>>& records, const std::string& targetId) {
    // Linear search manually checks each element
    for (size_t i = 0; i < records.size(); ++i) {
        if (records[i] && records[i]->getId() == targetId) {
            return records[i];
        }
    }
    return nullptr;
}

std::shared_ptr<PatientRecord> SearchEngine::binarySearchById(const std::vector<std::shared_ptr<PatientRecord>>& sortedRecords, const std::string& targetId) {
    // Binary search on sorted records
    int left = 0;
    int right = static_cast<int>(sortedRecords.size()) - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (!sortedRecords[mid]) {
            break;
        }
        std::string midId = sortedRecords[mid]->getId();
        if (midId == targetId) {
            return sortedRecords[mid];
        }
        if (midId < targetId) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return nullptr;
}

std::shared_ptr<PatientRecord> SearchEngine::recursiveSearchByName(const std::vector<std::shared_ptr<PatientRecord>>& records, const std::string& targetName, size_t index) {
    // Base case when reaching end
    if (index >= records.size()) {
        return nullptr;
    }
    if (records[index] && records[index]->getName() == targetName) {
        return records[index];
    }
    // Recursive step
    return recursiveSearchByName(records, targetName, index + 1);
}

void SearchEngine::selectionSort(std::vector<std::shared_ptr<PatientRecord>>& records, const std::string& field) {
    int n = static_cast<int>(records.size());
    for (int i = 0; i < n - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < n; ++j) {
            if (compareRecords(records[j], records[minIdx], field) < 0) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            std::swap(records[i], records[minIdx]);
        }
    }
}

void SearchEngine::insertionSort(std::vector<std::shared_ptr<PatientRecord>>& records, const std::string& field) {
    int n = static_cast<int>(records.size());
    for (int i = 1; i < n; ++i) {
        std::shared_ptr<PatientRecord> key = records[i];
        int j = i - 1;
        while (j >= 0 && compareRecords(records[j], key, field) > 0) {
            records[j + 1] = records[j];
            j--;
        }
        records[j + 1] = key;
    }
}

int SearchEngine::partition(std::vector<std::shared_ptr<PatientRecord>>& records, int low, int high, const std::string& field) {
    std::shared_ptr<PatientRecord> pivot = records[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (compareRecords(records[j], pivot, field) <= 0) {
            i++;
            std::swap(records[i], records[j]);
        }
    }
    std::swap(records[i + 1], records[high]);
    return i + 1;
}

void SearchEngine::quickSortHelper(std::vector<std::shared_ptr<PatientRecord>>& records, int low, int high, const std::string& field) {
    if (low < high) {
        int pi = partition(records, low, high, field);
        quickSortHelper(records, low, pi - 1, field);
        quickSortHelper(records, pi + 1, high, field);
    }
}

void SearchEngine::quickSort(std::vector<std::shared_ptr<PatientRecord>>& records, const std::string& field) {
    if (records.empty()) return;
    quickSortHelper(records, 0, static_cast<int>(records.size()) - 1, field);
}

}
