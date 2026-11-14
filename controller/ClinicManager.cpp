#include "ClinicManager.h"
#include "../utils/IDGenerator.h"
#include "../utils/FileHandler.h"
#include "../utils/Exceptions.h"

namespace scapms {

ClinicManager::ClinicManager() : appointmentCtrl(&database) {}

void ClinicManager::reset() {
    database = ClinicDatabase();
    appointmentCtrl.setDatabase(&database);
}

ClinicDatabase& ClinicManager::getDatabase() {
    return database;
}

AppointmentController& ClinicManager::getAppointmentController() {
    return appointmentCtrl;
}

bool ClinicManager::registerGeneralPatient(const std::string& name, int age, const std::string& contact, const std::string& dept, std::string& outId, std::string& errorMsg) {
    if (name.empty()) {
        errorMsg = "Patient name cannot be empty";
        return false;
    }
    if (age < 0 || age > 120) {
        errorMsg = "Patient age must be between 0 and 120";
        return false;
    }

    outId = IDGenerator::generatePatientId();
    std::string safeDept = dept.empty() ? "General Practice" : dept;
    auto patient = std::make_shared<GeneralPatient>(outId, name, age, contact, safeDept);

    if (!database.addPatient(patient)) {
        errorMsg = "Failed to add patient to database";
        return false;
    }
    return true;
}

bool ClinicManager::registerEmergencyPatient(const std::string& name, int age, const std::string& contact, int severity, const std::string& notes, std::string& outId, std::string& errorMsg) {
    if (name.empty()) {
        errorMsg = "Patient name cannot be empty";
        return false;
    }
    if (age < 0 || age > 120) {
        errorMsg = "Patient age must be between 0 and 120";
        return false;
    }
    if (severity < 1 || severity > 5) {
        severity = 1;
    }

    outId = IDGenerator::generatePatientId();
    std::string safeNotes = notes.empty() ? "Emergency Room Admission" : notes;
    auto patient = std::make_shared<EmergencyPatient>(outId, name, age, contact, severity, safeNotes);

    if (!database.addPatient(patient)) {
        errorMsg = "Failed to add patient to database";
        return false;
    }
    return true;
}

bool ClinicManager::registerFollowUpPatient(const std::string& name, int age, const std::string& contact, const std::string& doctor, const std::string& diag, std::string& outId, std::string& errorMsg) {
    if (name.empty()) {
        errorMsg = "Patient name cannot be empty";
        return false;
    }
    if (age < 0 || age > 120) {
        errorMsg = "Patient age must be between 0 and 120";
        return false;
    }

    outId = IDGenerator::generatePatientId();
    std::string safeDoc = doctor.empty() ? "Attending Physician" : doctor;
    std::string safeDiag = diag.empty() ? "Routine Check" : diag;
    auto patient = std::make_shared<FollowUpPatient>(outId, name, age, contact, safeDoc, safeDiag);

    if (!database.addPatient(patient)) {
        errorMsg = "Failed to add patient to database";
        return false;
    }
    return true;
}

bool ClinicManager::updatePatient(const std::string& id, const std::string& name, int age, const std::string& contact, const std::string& extra1, const std::string& extra2, std::string& errorMsg) {
    auto current = database.findPatientById(id);
    if (!current) {
        errorMsg = "Patient not found";
        return false;
    }
    if (name.empty()) {
        errorMsg = "Patient name cannot be empty";
        return false;
    }
    if (age < 0 || age > 120) {
        errorMsg = "Patient age must be between 0 and 120";
        return false;
    }

    std::string type = current->getPatientType();
    std::shared_ptr<PatientRecord> updated;

    if (type == "General") {
        std::string dept = extra1.empty() ? "General Practice" : extra1;
        updated = std::make_shared<GeneralPatient>(id, name, age, contact, dept);
    } else if (type == "Emergency") {
        int sev = 1;
        try { sev = std::stoi(extra1); } catch (...) { sev = 1; }
        updated = std::make_shared<EmergencyPatient>(id, name, age, contact, sev, extra2);
    } else if (type == "FollowUp") {
        updated = std::make_shared<FollowUpPatient>(id, name, age, contact, extra1, extra2);
    } else {
        errorMsg = "Unknown patient category";
        return false;
    }

    // Preserve existing appointment links
    for (const auto& aId : current->getAppointmentIds()) {
        updated->addAppointment(aId);
    }

    if (!database.updatePatient(updated)) {
        errorMsg = "Failed to update patient record";
        return false;
    }
    return true;
}

bool ClinicManager::deletePatient(const std::string& id, std::string& errorMsg) {
    if (!database.deletePatient(id)) {
        errorMsg = "Patient not found or could not be removed";
        return false;
    }
    return true;
}

bool ClinicManager::enqueuePatient(const std::string& id, std::string& errorMsg) {
    auto patient = database.findPatientById(id);
    if (!patient) {
        errorMsg = "Patient ID not registered";
        return false;
    }
    database.addToQueue(id);
    return true;
}

std::string ClinicManager::callNextPatient() {
    return database.callNextPatient();
}

std::vector<std::string> ClinicManager::getQueueList() const {
    return database.getQueueList();
}

std::shared_ptr<PatientRecord> ClinicManager::searchPatient(const std::string& query, const std::string& method, bool isSorted) {
    const auto& list = database.getAllPatients();
    if (list.empty()) return nullptr;

    if (method == "Binary Search") {
        std::vector<std::shared_ptr<PatientRecord>> sortedList = list;
        SearchEngine::selectionSort(sortedList, "ID");
        return SearchEngine::binarySearchById(sortedList, query);
    }
    if (method == "Recursive Search") {
        return SearchEngine::recursiveSearchByName(list, query, 0);
    }
    // Default: Linear Search
    return SearchEngine::linearSearchById(list, query);
}

std::vector<std::shared_ptr<PatientRecord>> ClinicManager::getSortedPatients(const std::string& algorithm, const std::string& field) {
    std::vector<std::shared_ptr<PatientRecord>> result = database.getAllPatients();
    if (result.empty()) return result;

    if (algorithm == "Selection Sort") {
        SearchEngine::selectionSort(result, field);
    } else if (algorithm == "Insertion Sort") {
        SearchEngine::insertionSort(result, field);
    } else if (algorithm == "Quick Sort") {
        SearchEngine::quickSort(result, field);
    } else {
        // Fallback default
        SearchEngine::selectionSort(result, field);
    }
    return result;
}

CategoryDistribution ClinicManager::getCategoryDistribution() const {
    CategoryDistribution dist = {0, 0, 0};
    const auto& list = database.getAllPatients();
    for (const auto& p : list) {
        if (p->getPatientType() == "General") dist.generalCount++;
        else if (p->getPatientType() == "Emergency") dist.emergencyCount++;
        else if (p->getPatientType() == "FollowUp") dist.followUpCount++;
    }
    return dist;
}

std::vector<PatientRecord*> ClinicManager::getRecentlyAccessedPatients() const {
    return database.getRecentPatients();
}

bool ClinicManager::undoLastAction(std::string& statusMsg) {
    return database.undoLastOperation(statusMsg);
}

bool ClinicManager::saveToFiles(const std::string& patFile, const std::string& apptFile, std::string& errorMsg) {
    try {
        FileHandler::saveDatabase(database, patFile, apptFile);
        return true;
    } catch (const std::exception& e) {
        errorMsg = e.what();
        return false;
    }
}

bool ClinicManager::loadFromFiles(const std::string& patFile, const std::string& apptFile, std::string& errorMsg) {
    try {
        FileHandler::loadDatabase(database, patFile, apptFile);
        return true;
    } catch (const std::exception& e) {
        errorMsg = e.what();
        return false;
    }
}

std::string ClinicManager::formatPolymorphicSummary(const PatientRecord& record) const {
    return ClinicDatabase::describePatientPolymorphic(record);
}

void ClinicManager::loadSampleData() {
    std::string id, err;
    registerGeneralPatient("Adebayo Adeleke", 34, "08031234567", "Cardiology", id, err);
    registerGeneralPatient("Ngozi Okonjo", 45, "08029876543", "Internal Medicine", id, err);
    registerEmergencyPatient("Aminu Kano", 62, "08145550101", 1, "Acute chest distress", id, err);
    registerGeneralPatient("Chukwuma Eze", 29, "08051239876", "Orthopedics", id, err);
    registerFollowUpPatient("Folake Balogun", 38, "08076543210", "Dr Olawale", "Post surgery recovery", id, err);
    registerEmergencyPatient("Ibrahim Musa", 51, "08182345678", 2, "Severe asthma attack", id, err);
    registerGeneralPatient("Chinwe Achebe", 27, "08093456789", "Pediatrics", id, err);
    registerFollowUpPatient("Olumide Bakare", 42, "08064567890", "Dr Nnamdi", "Hypertension monitoring", id, err);
    registerGeneralPatient("Zainab Aliyu", 19, "08135678901", "Dermatology", id, err);
    registerGeneralPatient("Emeka Okafor", 58, "08026789012", "Ophthalmology", id, err);
    registerEmergencyPatient("Damilola Ojo", 23, "08157890123", 3, "Road accident laceration", id, err);
    registerGeneralPatient("Aisha Bello", 31, "08088901234", "Obstetrics", id, err);
    registerFollowUpPatient("Kelechi Iheanacho", 26, "08129012345", "Dr Okeke", "Ligament rehabilitation", id, err);
    registerGeneralPatient("Funke Akindele", 48, "08030123456", "General Practice", id, err);
    registerEmergencyPatient("Usman Danfodio", 70, "08061234567", 1, "Diabetic emergency", id, err);
    registerFollowUpPatient("Chiamaka Nnamani", 33, "08172345678", "Dr Abubakar", "Post cesarean review", id, err);
    registerGeneralPatient("Segun Arinze", 55, "08053456789", "Neurology", id, err);
    registerGeneralPatient("Halima Abubakar", 40, "08164567890", "ENT Clinic", id, err);
    registerEmergencyPatient("Ifeanyi Ubah", 49, "08095678901", 2, "Burn injury", id, err);
    registerFollowUpPatient("Bamidele Johnson", 64, "08106789012", "Dr Danjuma", "Pacemaker monitoring", id, err);

    std::string apptId;
    appointmentCtrl.scheduleAppointment("P1001", "2026/11/16", "09:00", "Cardiac Checkup", apptId, err);
    appointmentCtrl.scheduleAppointment("P1002", "2026/11/16", "10:30", "Medical Consultation", apptId, err);
    appointmentCtrl.scheduleAppointment("P1004", "2026/11/16", "11:45", "Bone Evaluation", apptId, err);
    appointmentCtrl.scheduleAppointment("P1005", "2026/11/17", "09:15", "Wound Inspection", apptId, err);
    appointmentCtrl.scheduleAppointment("P1007", "2026/11/17", "10:00", "Child Wellness", apptId, err);
    appointmentCtrl.scheduleAppointment("P1008", "2026/11/17", "11:30", "Pressure Test", apptId, err);
    appointmentCtrl.scheduleAppointment("P1010", "2026/11/18", "09:30", "Eye Screening", apptId, err);
    appointmentCtrl.scheduleAppointment("P1012", "2026/11/18", "10:45", "Antenatal Visit", apptId, err);

    appointmentCtrl.completeAppointment("A5001", err);
    appointmentCtrl.completeAppointment("A5002", err);

    enqueuePatient("P1001", err);
    enqueuePatient("P1003", err);
    enqueuePatient("P1006", err);
    enqueuePatient("P1011", err);
}

}
