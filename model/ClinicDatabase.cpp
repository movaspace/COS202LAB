#include "ClinicDatabase.h"
#include "GeneralPatient.h"
#include "EmergencyPatient.h"
#include "FollowUpPatient.h"
#include <sstream>
#include <iomanip>

namespace scapms {

ClinicDatabase::ClinicDatabase() : recentCount(0) {
    for (int i = 0; i < RECENT_CACHE_SIZE; ++i) {
        recentPatients[i] = nullptr;
    }
}

bool ClinicDatabase::addPatient(std::shared_ptr<PatientRecord> patient, bool recordUndo) {
    if (!patient) return false;
    for (const auto& p : patients) {
        if (p->getId() == patient->getId()) {
            return false;
        }
    }
    patients.push_back(patient);
    recordAccess(patient.get());
    if (recordUndo) {
        undoStack.push(AdminOperation(AdminActionType::AddPatient, patient, Appointment("", "", "", "", ""), "Added patient " + patient->getId()));
    }
    return true;
}

bool ClinicDatabase::deletePatient(const std::string& patientId, bool recordUndo) {
    for (auto it = patients.begin(); it != patients.end(); ++it) {
        if ((*it)->getId() == patientId) {
            std::shared_ptr<PatientRecord> removed = *it;
            patients.erase(it);
            // Remove from cache if present
            for (int i = 0; i < recentCount; ++i) {
                if (recentPatients[i] && recentPatients[i]->getId() == patientId) {
                    for (int j = i; j < recentCount - 1; ++j) {
                        recentPatients[j] = recentPatients[j + 1];
                    }
                    recentPatients[recentCount - 1] = nullptr;
                    recentCount--;
                    break;
                }
            }
            if (recordUndo) {
                undoStack.push(AdminOperation(AdminActionType::DeletePatient, removed, Appointment("", "", "", "", ""), "Deleted patient " + patientId));
            }
            return true;
        }
    }
    return false;
}

bool ClinicDatabase::updatePatient(std::shared_ptr<PatientRecord> updatedPatient, bool recordUndo) {
    if (!updatedPatient) return false;
    for (size_t i = 0; i < patients.size(); ++i) {
        if (patients[i]->getId() == updatedPatient->getId()) {
            std::shared_ptr<PatientRecord> oldPatient = patients[i];
            if (recordUndo) {
                undoStack.push(AdminOperation(AdminActionType::ModifyPatient, oldPatient, Appointment("", "", "", "", ""), "Modified patient " + updatedPatient->getId()));
            }
            patients[i] = updatedPatient;
            recordAccess(updatedPatient.get());
            return true;
        }
    }
    return false;
}

std::shared_ptr<PatientRecord> ClinicDatabase::findPatientById(const std::string& id) {
    for (auto& p : patients) {
        if (p->getId() == id) {
            recordAccess(p.get());
            return p;
        }
    }
    return nullptr;
}

const std::vector<std::shared_ptr<PatientRecord>>& ClinicDatabase::getAllPatients() const {
    return patients;
}

bool ClinicDatabase::addAppointment(const Appointment& appt, bool recordUndo) {
    for (const auto& a : appointments) {
        if (a.getAppointmentId() == appt.getAppointmentId()) {
            return false;
        }
    }
    appointments.push_back(appt);
    auto p = findPatientById(appt.getPatientId());
    if (p) {
        p->addAppointment(appt.getAppointmentId());
    }
    if (recordUndo) {
        undoStack.push(AdminOperation(AdminActionType::AddAppointment, nullptr, appt, "Added appointment " + appt.getAppointmentId()));
    }
    return true;
}

bool ClinicDatabase::deleteAppointment(const std::string& apptId, bool recordUndo) {
    for (auto it = appointments.begin(); it != appointments.end(); ++it) {
        if (it->getAppointmentId() == apptId) {
            Appointment removed = *it;
            auto p = findPatientById(removed.getPatientId());
            if (p) {
                p->removeAppointment(apptId);
            }
            appointments.erase(it);
            if (recordUndo) {
                undoStack.push(AdminOperation(AdminActionType::DeleteAppointment, nullptr, removed, "Removed appointment " + apptId));
            }
            return true;
        }
    }
    return false;
}

bool ClinicDatabase::updateAppointmentStatus(const std::string& apptId, const std::string& newStatus, bool recordUndo) {
    for (auto& a : appointments) {
        if (a.getAppointmentId() == apptId) {
            Appointment oldAppt = a;
            a.setStatus(newStatus);
            if (recordUndo) {
                undoStack.push(AdminOperation(AdminActionType::ModifyAppointment, nullptr, oldAppt, "Changed status for " + apptId));
            }
            return true;
        }
    }
    return false;
}

Appointment* ClinicDatabase::findAppointmentById(const std::string& apptId) {
    for (auto& a : appointments) {
        if (a.getAppointmentId() == apptId) {
            return &a;
        }
    }
    return nullptr;
}

const std::vector<Appointment>& ClinicDatabase::getAllAppointments() const {
    return appointments;
}

std::vector<Appointment> ClinicDatabase::getAppointmentsForPatient(const std::string& patientId) const {
    std::vector<Appointment> result;
    for (const auto& a : appointments) {
        if (a.getPatientId() == patientId) {
            result.push_back(a);
        }
    }
    return result;
}

void ClinicDatabase::addToQueue(const std::string& patientId) {
    waitingQueue.push(patientId);
}

std::string ClinicDatabase::callNextPatient() {
    if (waitingQueue.empty()) {
        return "";
    }
    std::string nextId = waitingQueue.front();
    waitingQueue.pop();
    return nextId;
}

bool ClinicDatabase::isQueueEmpty() const {
    return waitingQueue.empty();
}

std::vector<std::string> ClinicDatabase::getQueueList() const {
    std::vector<std::string> list;
    std::queue<std::string> copy = waitingQueue;
    while (!copy.empty()) {
        list.push_back(copy.front());
        copy.pop();
    }
    return list;
}

int ClinicDatabase::getQueueSize() const {
    return static_cast<int>(waitingQueue.size());
}

void ClinicDatabase::recordAccess(PatientRecord* record) {
    if (!record) return;
    int existingIndex = -1;
    for (int i = 0; i < recentCount; ++i) {
        if (recentPatients[i] == record) {
            existingIndex = i;
            break;
        }
    }
    if (existingIndex != -1) {
        for (int i = existingIndex; i > 0; --i) {
            recentPatients[i] = recentPatients[i - 1];
        }
        recentPatients[0] = record;
    } else {
        int limit = recentCount < RECENT_CACHE_SIZE ? recentCount : RECENT_CACHE_SIZE - 1;
        for (int i = limit; i > 0; --i) {
            recentPatients[i] = recentPatients[i - 1];
        }
        recentPatients[0] = record;
        if (recentCount < RECENT_CACHE_SIZE) {
            recentCount++;
        }
    }
}

std::vector<PatientRecord*> ClinicDatabase::getRecentPatients() const {
    std::vector<PatientRecord*> list;
    for (int i = 0; i < recentCount; ++i) {
        if (recentPatients[i]) {
            list.push_back(recentPatients[i]);
        }
    }
    return list;
}

bool ClinicDatabase::hasUndo() const {
    return !undoStack.empty();
}

bool ClinicDatabase::undoLastOperation(std::string& outMessage) {
    if (undoStack.empty()) {
        outMessage = "No administrative operations to undo";
        return false;
    }
    AdminOperation op = undoStack.top();
    undoStack.pop();

    switch (op.type) {
    case AdminActionType::AddPatient:
        if (op.patientData) {
            deletePatient(op.patientData->getId(), false);
            outMessage = "Reversed: Added patient " + op.patientData->getId();
        }
        break;
    case AdminActionType::DeletePatient:
        if (op.patientData) {
            addPatient(op.patientData, false);
            outMessage = "Reversed: Deleted patient " + op.patientData->getId();
        }
        break;
    case AdminActionType::ModifyPatient:
        if (op.patientData) {
            updatePatient(op.patientData, false);
            outMessage = "Reversed: Modified patient " + op.patientData->getId();
        }
        break;
    case AdminActionType::AddAppointment:
        deleteAppointment(op.appointmentData.getAppointmentId(), false);
        outMessage = "Reversed: Added appointment " + op.appointmentData.getAppointmentId();
        break;
    case AdminActionType::DeleteAppointment:
        addAppointment(op.appointmentData, false);
        outMessage = "Reversed: Deleted appointment " + op.appointmentData.getAppointmentId();
        break;
    case AdminActionType::ModifyAppointment:
        for (auto& a : appointments) {
            if (a.getAppointmentId() == op.appointmentData.getAppointmentId()) {
                a.setStatus(op.appointmentData.getStatus());
                break;
            }
        }
        outMessage = "Reversed: Modified appointment " + op.appointmentData.getAppointmentId();
        break;
    }
    return true;
}

std::string ClinicDatabase::describePatientPolymorphic(const PatientRecord& record) {
    std::ostringstream oss;
    oss << "ID: " << record.getId()
        << ", Name: " << record.getName()
        << ", Type: " << record.getPatientType()
        << ", Fee: $" << std::fixed << std::setprecision(2) << record.calculateConsultationFee()
        << ", Details: " << record.getDetails();
    return oss.str();
}

}
