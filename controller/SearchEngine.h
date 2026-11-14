#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "../model/PatientRecord.h"
#include <vector>
#include <memory>
#include <string>

namespace scapms {

class SearchEngine {
public:
    // Searching algorithms implemented manually
    static std::shared_ptr<PatientRecord> linearSearchById(const std::vector<std::shared_ptr<PatientRecord>>& records, const std::string& targetId);
    static std::shared_ptr<PatientRecord> binarySearchById(const std::vector<std::shared_ptr<PatientRecord>>& sortedRecords, const std::string& targetId);
    static std::shared_ptr<PatientRecord> recursiveSearchByName(const std::vector<std::shared_ptr<PatientRecord>>& records, const std::string& targetName, size_t index = 0);

    // Sorting algorithms implemented manually
    static void selectionSort(std::vector<std::shared_ptr<PatientRecord>>& records, const std::string& field);
    static void quickSort(std::vector<std::shared_ptr<PatientRecord>>& records, const std::string& field);
    static void insertionSort(std::vector<std::shared_ptr<PatientRecord>>& records, const std::string& field);

private:
    static int compareRecords(const std::shared_ptr<PatientRecord>& a, const std::shared_ptr<PatientRecord>& b, const std::string& field);
    static void quickSortHelper(std::vector<std::shared_ptr<PatientRecord>>& records, int low, int high, const std::string& field);
    static int partition(std::vector<std::shared_ptr<PatientRecord>>& records, int low, int high, const std::string& field);
};

}

#endif
