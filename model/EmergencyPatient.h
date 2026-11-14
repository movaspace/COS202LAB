#ifndef EMERGENCYPATIENT_H
#define EMERGENCYPATIENT_H

#include "PatientRecord.h"

namespace scapms {

class EmergencyPatient : public PatientRecord {
private:
    int severityLevel;
    std::string triageNotes;

public:
    EmergencyPatient(const std::string& id, const std::string& name, int age, const std::string& contact, int severity, const std::string& notes);

    int getSeverityLevel() const;
    void setSeverityLevel(int level);
    std::string getTriageNotes() const;
    void setTriageNotes(const std::string& notes);

    std::string getPatientType() const override;
    std::string getDetails() const override;
    double calculateConsultationFee() const override;
};

}

#endif
