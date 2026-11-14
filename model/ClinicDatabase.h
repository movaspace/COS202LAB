#ifndef CLINICDATABASE_H
#define CLINICDATABASE_H

#include "PatientRecord.h"
#include "Appointment.h"
#include <vector>
#include <queue>
#include <stack>
#include <memory>
#include <string>

namespace scapms {

enum class AdminActionType {
    AddPatient,
    DeletePatient,
    ModifyPatient,
    AddAppointment,
    DeleteAppointment,
    ModifyAppointment
};

struct AdminOperation {
    AdminActionType type;
    std::shared_ptr<PatientRecord> patientData;
    Appointment appointmentData;
    std::string description;

    AdminOperation(AdminActionType t, std::shared_ptr<PatientRecord> p, const Appointment& a, const std::string& desc)
        : type(t), patientData(p), appointmentData(a), description(desc) {}
};

class ClinicDatabase {
public:
    static const int RECENT_CACHE_SIZE = 10;

private:
    std::vector<std::shared_ptr<PatientRecord>> patients;
    std::vector<Appointment> appointments;
    std::queue<std::string> waitingQueue;
    std::stack<AdminOperation> undoStack;
    PatientRecord* recentPatients[RECENT_CACHE_SIZE];
    int recentCount;

public:
    ClinicDatabase();
    ~ClinicDatabase() = default;

    // Patient operations
    bool addPatient(std::shared_ptr<PatientRecord> patient, bool recordUndo = true);
    bool deletePatient(const std::string& patientId, bool recordUndo = true);
    bool updatePatient(std::shared_ptr<PatientRecord> updatedPatient, bool recordUndo = true);
    std::shared_ptr<PatientRecord> findPatientById(const std::string& id);
    const std::vector<std::shared_ptr<PatientRecord>>& getAllPatients() const;

    // Appointment operations
    bool addAppointment(const Appointment& appt, bool recordUndo = true);
    bool deleteAppointment(const std::string& apptId, bool recordUndo = true);
    bool updateAppointmentStatus(const std::string& apptId, const std::string& newStatus, bool recordUndo = true);
    Appointment* findAppointmentById(const std::string& apptId);
    const std::vector<Appointment>& getAllAppointments() const;
    std::vector<Appointment> getAppointmentsForPatient(const std::string& patientId) const;

    // Queue operations
    void addToQueue(const std::string& patientId);
    std::string callNextPatient();
    bool isQueueEmpty() const;
    std::vector<std::string> getQueueList() const;
    int getQueueSize() const;

    // Cache operations
    void recordAccess(PatientRecord* record);
    std::vector<PatientRecord*> getRecentPatients() const;

    // Undo operation
    bool undoLastOperation(std::string& outMessage);
    bool hasUndo() const;

    // Polymorphism demo
    static std::string describePatientPolymorphic(const PatientRecord& record);
};

}

#endif
