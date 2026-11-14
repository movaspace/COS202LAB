#include "AppointmentController.h"
#include "../utils/IDGenerator.h"
#include "../model/Appointable.h"
#include <map>
#include <algorithm>

namespace scapms {

AppointmentController::AppointmentController() : database(nullptr) {}

AppointmentController::AppointmentController(ClinicDatabase* db) : database(db) {}

void AppointmentController::setDatabase(ClinicDatabase* db) {
    database = db;
}

bool AppointmentController::scheduleAppointment(const std::string& patientId, const std::string& date, const std::string& time, const std::string& reason, std::string& outApptId, std::string& errorMsg) {
    if (!database) {
        errorMsg = "Database not initialized";
        return false;
    }
    auto patient = database->findPatientById(patientId);
    if (!patient) {
        errorMsg = "Patient ID not found";
        return false;
    }

    auto appointable = std::dynamic_pointer_cast<Appointable>(patient);
    if (!appointable) {
        errorMsg = "Emergency patients cannot be scheduled for regular appointments";
        return false;
    }

    if (date.empty() || time.empty()) {
        errorMsg = "Date and time cannot be empty";
        return false;
    }

    // Check for duplicate appointment at the same date and time for this patient
    const auto& existingAppts = database->getAllAppointments();
    for (const auto& a : existingAppts) {
        if (a.getPatientId() == patientId && a.getDate() == date && a.getTime() == time && a.getStatus() != "Cancelled") {
            errorMsg = "Patient already has an active appointment scheduled on " + date + " at " + time;
            return false;
        }
    }

    outApptId = IDGenerator::generateAppointmentId();
    Appointment appt(outApptId, patientId, date, time, reason, "Pending");
    
    if (database->addAppointment(appt)) {
        appointable->scheduleAppointment(outApptId);
        return true;
    }

    errorMsg = "Failed to add appointment to database";
    return false;
}

bool AppointmentController::cancelAppointment(const std::string& apptId, std::string& errorMsg) {
    if (!database) return false;
    Appointment* appt = database->findAppointmentById(apptId);
    if (!appt) {
        errorMsg = "Appointment not found";
        return false;
    }

    if (appt->getStatus() == "Cancelled") {
        errorMsg = "Appointment is already cancelled";
        return false;
    }

    database->updateAppointmentStatus(apptId, "Cancelled");
    auto patient = database->findPatientById(appt->getPatientId());
    if (patient) {
        auto appointable = std::dynamic_pointer_cast<Appointable>(patient);
        if (appointable) {
            appointable->cancelAppointment(apptId);
        }
    }
    return true;
}

bool AppointmentController::completeAppointment(const std::string& apptId, std::string& errorMsg) {
    if (!database) return false;
    Appointment* appt = database->findAppointmentById(apptId);
    if (!appt) {
        errorMsg = "Appointment not found";
        return false;
    }
    database->updateAppointmentStatus(apptId, "Completed");
    return true;
}

DailySummary AppointmentController::getDailySummary(const std::string& targetDate) const {
    DailySummary summary = {0, 0, 0, 0};
    if (!database) return summary;
    const auto& list = database->getAllAppointments();

    for (const auto& a : list) {
        if (targetDate.empty() || a.getDate() == targetDate) {
            summary.total++;
            if (a.getStatus() == "Completed") summary.completed++;
            else if (a.getStatus() == "Pending") summary.pending++;
            else if (a.getStatus() == "Cancelled") summary.cancelled++;
        }
    }
    return summary;
}

std::vector<std::string> AppointmentController::getPendingPatientsReport() const {
    std::vector<std::string> pendingPatIds;
    if (!database) return pendingPatIds;
    const auto& list = database->getAllAppointments();

    for (const auto& a : list) {
        if (a.getStatus() == "Pending") {
            bool exists = false;
            for (const auto& id : pendingPatIds) {
                if (id == a.getPatientId()) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                pendingPatIds.push_back(a.getPatientId());
            }
        }
    }
    return pendingPatIds;
}

std::vector<std::pair<std::string, int>> AppointmentController::getMostFrequentlyAttendedPatients() const {
    std::map<std::string, int> counts;
    if (!database) return {};
    const auto& list = database->getAllAppointments();

    for (const auto& a : list) {
        if (a.getStatus() == "Completed") {
            counts[a.getPatientId()]++;
        }
    }

    std::vector<std::pair<std::string, int>> result;
    for (const auto& pair : counts) {
        result.push_back(pair);
    }

    // Sort by count descending using simple selection sort
    for (size_t i = 0; i < result.size(); ++i) {
        size_t maxIdx = i;
        for (size_t j = i + 1; j < result.size(); ++j) {
            if (result[j].second > result[maxIdx].second) {
                maxIdx = j;
            }
        }
        if (maxIdx != i) {
            std::swap(result[i], result[maxIdx]);
        }
    }

    return result;
}

}
