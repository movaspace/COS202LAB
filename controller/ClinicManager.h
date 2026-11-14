#ifndef CLINICMANAGER_H
#define CLINICMANAGER_H

#include "../model/ClinicDatabase.h"
#include "../model/GeneralPatient.h"
#include "../model/EmergencyPatient.h"
#include "../model/FollowUpPatient.h"
#include "SearchEngine.h"
#include "AppointmentController.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace scapms {

struct CategoryDistribution {
    int generalCount;
    int emergencyCount;
    int followUpCount;
};

class ClinicManager {
private:
    ClinicDatabase database;
    AppointmentController appointmentCtrl;

public:
    ClinicManager();
    void reset();

    ClinicDatabase& getDatabase();
    AppointmentController& getAppointmentController();

    // Patient management with validation
    bool registerGeneralPatient(const std::string& name, int age, const std::string& contact, const std::string& dept, std::string& outId, std::string& errorMsg);
    bool registerEmergencyPatient(const std::string& name, int age, const std::string& contact, int severity, const std::string& notes, std::string& outId, std::string& errorMsg);
    bool registerFollowUpPatient(const std::string& name, int age, const std::string& contact, const std::string& doctor, const std::string& diag, std::string& outId, std::string& errorMsg);

    bool updatePatient(const std::string& id, const std::string& name, int age, const std::string& contact, const std::string& extra1, const std::string& extra2, std::string& errorMsg);
    bool deletePatient(const std::string& id, std::string& errorMsg);

    // Waiting queue
    bool enqueuePatient(const std::string& id, std::string& errorMsg);
    std::string callNextPatient();
    std::vector<std::string> getQueueList() const;

    // Search and Sort
    std::shared_ptr<PatientRecord> searchPatient(const std::string& query, const std::string& method, bool isSorted = false);
    std::vector<std::shared_ptr<PatientRecord>> getSortedPatients(const std::string& algorithm, const std::string& field);

    // Reports
    CategoryDistribution getCategoryDistribution() const;

    // Cache
    std::vector<PatientRecord*> getRecentlyAccessedPatients() const;

    // Undo
    bool undoLastAction(std::string& statusMsg);

    // Persistence
    bool saveToFiles(const std::string& patFile, const std::string& apptFile, std::string& errorMsg);
    bool loadFromFiles(const std::string& patFile, const std::string& apptFile, std::string& errorMsg);

    // Polymorphism
    std::string formatPolymorphicSummary(const PatientRecord& record) const;

    // Sample data loader for quick testing
    void loadSampleData();
};

}

#endif
