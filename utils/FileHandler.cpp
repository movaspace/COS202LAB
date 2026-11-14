#include "FileHandler.h"
#include "Exceptions.h"
#include "IDGenerator.h"
#include "../model/GeneralPatient.h"
#include "../model/EmergencyPatient.h"
#include "../model/FollowUpPatient.h"
#include <fstream>
#include <sstream>
#include <vector>

namespace scapms {

static std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(line);
    while (std::getline(stream, token, ',')) {
        tokens.push_back(token);
    }
    return tokens;
}

bool FileHandler::saveDatabase(const ClinicDatabase& db, const std::string& patientsFile, const std::string& appointmentsFile) {
    std::ofstream pOut(patientsFile);
    if (!pOut.is_open()) {
        throw FileIOException("Unable to open patients file for writing: " + patientsFile);
    }

    const auto& patients = db.getAllPatients();
    for (const auto& p : patients) {
        pOut << p->getPatientType() << ","
             << p->getId() << ","
             << p->getName() << ","
             << p->getAge() << ","
             << p->getContactInfo() << ",";

        if (p->getPatientType() == "General") {
            auto gen = std::dynamic_pointer_cast<GeneralPatient>(p);
            pOut << (gen ? gen->getDepartment() : "") << ",";
        } else if (p->getPatientType() == "Emergency") {
            auto em = std::dynamic_pointer_cast<EmergencyPatient>(p);
            if (em) {
                pOut << em->getSeverityLevel() << "," << em->getTriageNotes();
            }
        } else if (p->getPatientType() == "FollowUp") {
            auto fu = std::dynamic_pointer_cast<FollowUpPatient>(p);
            if (fu) {
                pOut << fu->getPreviousDoctor() << "," << fu->getOriginalDiagnosis();
            }
        }
        pOut << "\n";
    }
    pOut.close();

    std::ofstream aOut(appointmentsFile);
    if (!aOut.is_open()) {
        throw FileIOException("Unable to open appointments file for writing: " + appointmentsFile);
    }

    const auto& appointments = db.getAllAppointments();
    for (const auto& a : appointments) {
        aOut << a.getAppointmentId() << ","
             << a.getPatientId() << ","
             << a.getDate() << ","
             << a.getTime() << ","
             << a.getReason() << ","
             << a.getStatus() << "\n";
    }
    aOut.close();

    return true;
}

bool FileHandler::loadDatabase(ClinicDatabase& db, const std::string& patientsFile, const std::string& appointmentsFile) {
    std::ifstream pIn(patientsFile);
    if (!pIn.is_open()) {
        throw FileIOException("Unable to open patients file for reading: " + patientsFile);
    }

    std::string line;
    while (std::getline(pIn, line)) {
        if (line.empty()) continue;
        auto tokens = splitCsvLine(line);
        if (tokens.size() < 5) continue;

        std::string type = tokens[0];
        std::string id = tokens[1];
        std::string name = tokens[2];
        int age = 0;
        try {
            age = std::stoi(tokens[3]);
        } catch (...) {
            continue;
        }
        std::string contact = tokens[4];

        std::shared_ptr<PatientRecord> patient;
        if (type == "General") {
            std::string dept = tokens.size() > 5 ? tokens[5] : "General Practice";
            patient = std::make_shared<GeneralPatient>(id, name, age, contact, dept);
        } else if (type == "Emergency") {
            int severity = tokens.size() > 5 ? std::stoi(tokens[5]) : 1;
            std::string notes = tokens.size() > 6 ? tokens[6] : "Urgent";
            patient = std::make_shared<EmergencyPatient>(id, name, age, contact, severity, notes);
        } else if (type == "FollowUp") {
            std::string doctor = tokens.size() > 5 ? tokens[5] : "Doctor";
            std::string diag = tokens.size() > 6 ? tokens[6] : "Follow-up";
            patient = std::make_shared<FollowUpPatient>(id, name, age, contact, doctor, diag);
        }

        if (patient) {
            db.addPatient(patient, false);
            IDGenerator::syncFromExistingIds(id, "");
        }
    }
    pIn.close();

    std::ifstream aIn(appointmentsFile);
    if (aIn.is_open()) {
        while (std::getline(aIn, line)) {
            if (line.empty()) continue;
            auto tokens = splitCsvLine(line);
            if (tokens.size() < 6) continue;

            std::string apptId = tokens[0];
            std::string patId = tokens[1];
            std::string date = tokens[2];
            std::string time = tokens[3];
            std::string reason = tokens[4];
            std::string status = tokens[5];

            Appointment appt(apptId, patId, date, time, reason, status);
            db.addAppointment(appt, false);
            IDGenerator::syncFromExistingIds("", apptId);
        }
        aIn.close();
    }

    return true;
}

}
