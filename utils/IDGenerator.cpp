#include "IDGenerator.h"
#include <sstream>
#include <iomanip>

namespace scapms {

int IDGenerator::nextPatientNumber = 1001;
int IDGenerator::nextAppointmentNumber = 5001;

std::string IDGenerator::generatePatientId() {
    std::ostringstream oss;
    oss << "P" << std::setw(4) << std::setfill('0') << nextPatientNumber++;
    return oss.str();
}

std::string IDGenerator::generateAppointmentId() {
    std::ostringstream oss;
    oss << "A" << std::setw(4) << std::setfill('0') << nextAppointmentNumber++;
    return oss.str();
}

void IDGenerator::syncFromExistingIds(const std::string& patId, const std::string& apptId) {
    if (patId.size() > 1 && (patId[0] == 'P' || patId[0] == 'p')) {
        try {
            int num = std::stoi(patId.substr(1));
            if (num >= nextPatientNumber) {
                nextPatientNumber = num + 1;
            }
        } catch (...) {}
    }
    if (apptId.size() > 1 && (apptId[0] == 'A' || apptId[0] == 'a')) {
        try {
            int num = std::stoi(apptId.substr(1));
            if (num >= nextAppointmentNumber) {
                nextAppointmentNumber = num + 1;
            }
        } catch (...) {}
    }
}

}
