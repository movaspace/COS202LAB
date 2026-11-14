#include "GeneralPatient.h"

namespace scapms {

GeneralPatient::GeneralPatient(const std::string& id, const std::string& name, int age, const std::string& contact, const std::string& dept)
    : PatientRecord(id, name, age, contact), department(dept) {}

std::string GeneralPatient::getDepartment() const {
    return department;
}

void GeneralPatient::setDepartment(const std::string& dept) {
    department = dept;
}

std::string GeneralPatient::getPatientType() const {
    return "General";
}

std::string GeneralPatient::getDetails() const {
    return "Department: " + department;
}

double GeneralPatient::calculateConsultationFee() const {
    return 50.0;
}

bool GeneralPatient::scheduleAppointment(const std::string& apptId) {
    addAppointment(apptId);
    return true;
}

bool GeneralPatient::cancelAppointment(const std::string& apptId) {
    removeAppointment(apptId);
    return true;
}

}
