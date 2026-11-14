#include "EmergencyPatient.h"

namespace scapms {

EmergencyPatient::EmergencyPatient(const std::string& id, const std::string& name, int age, const std::string& contact, int severity, const std::string& notes)
    : PatientRecord(id, name, age, contact), severityLevel(severity), triageNotes(notes) {}

int EmergencyPatient::getSeverityLevel() const {
    return severityLevel;
}

void EmergencyPatient::setSeverityLevel(int level) {
    severityLevel = level;
}

std::string EmergencyPatient::getTriageNotes() const {
    return triageNotes;
}

void EmergencyPatient::setTriageNotes(const std::string& notes) {
    triageNotes = notes;
}

std::string EmergencyPatient::getPatientType() const {
    return "Emergency";
}

std::string EmergencyPatient::getDetails() const {
    return "Severity: " + std::to_string(severityLevel) + " | Notes: " + triageNotes;
}

double EmergencyPatient::calculateConsultationFee() const {
    return 150.0;
}

}
