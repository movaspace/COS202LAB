#include "PatientRecord.h"
#include <algorithm>

namespace scapms {

PatientRecord::PatientRecord(const std::string& id, const std::string& name, int age, const std::string& contact)
    : patientId(id), name(name), age(age), contactInfo(contact) {}

std::string PatientRecord::getId() const {
    return patientId;
}

std::string PatientRecord::getName() const {
    return name;
}

int PatientRecord::getAge() const {
    return age;
}

std::string PatientRecord::getContactInfo() const {
    return contactInfo;
}

const std::vector<std::string>& PatientRecord::getAppointmentIds() const {
    return appointmentIds;
}

void PatientRecord::setName(const std::string& newName) {
    name = newName;
}

void PatientRecord::setAge(int newAge) {
    age = newAge;
}

void PatientRecord::setContactInfo(const std::string& newContact) {
    contactInfo = newContact;
}

void PatientRecord::addAppointment(const std::string& apptId) {
    appointmentIds.push_back(apptId);
}

void PatientRecord::removeAppointment(const std::string& apptId) {
    for (size_t i = 0; i < appointmentIds.size(); ++i) {
        if (appointmentIds[i] == apptId) {
            appointmentIds.erase(appointmentIds.begin() + i);
            break;
        }
    }
}

}
