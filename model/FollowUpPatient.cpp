#include "FollowUpPatient.h"

namespace scapms {

FollowUpPatient::FollowUpPatient(const std::string& id, const std::string& name, int age, const std::string& contact, const std::string& doctor, const std::string& diagnosis)
    : PatientRecord(id, name, age, contact), previousDoctor(doctor), originalDiagnosis(diagnosis) {}

std::string FollowUpPatient::getPreviousDoctor() const {
    return previousDoctor;
}

void FollowUpPatient::setPreviousDoctor(const std::string& doctor) {
    previousDoctor = doctor;
}

std::string FollowUpPatient::getOriginalDiagnosis() const {
    return originalDiagnosis;
}

void FollowUpPatient::setOriginalDiagnosis(const std::string& diagnosis) {
    originalDiagnosis = diagnosis;
}

std::string FollowUpPatient::getPatientType() const {
    return "FollowUp";
}

std::string FollowUpPatient::getDetails() const {
    return "Doctor: " + previousDoctor + " | Diagnosis: " + originalDiagnosis;
}

double FollowUpPatient::calculateConsultationFee() const {
    return 30.0;
}

bool FollowUpPatient::scheduleAppointment(const std::string& apptId) {
    addAppointment(apptId);
    return true;
}

bool FollowUpPatient::cancelAppointment(const std::string& apptId) {
    removeAppointment(apptId);
    return true;
}

}
