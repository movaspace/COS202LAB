#ifndef FOLLOWUPPATIENT_H
#define FOLLOWUPPATIENT_H

#include "PatientRecord.h"
#include "Appointable.h"

namespace scapms {

class FollowUpPatient : public PatientRecord, public Appointable {
private:
    std::string previousDoctor;
    std::string originalDiagnosis;

public:
    FollowUpPatient(const std::string& id, const std::string& name, int age, const std::string& contact, const std::string& doctor, const std::string& diagnosis);

    std::string getPreviousDoctor() const;
    void setPreviousDoctor(const std::string& doctor);
    std::string getOriginalDiagnosis() const;
    void setOriginalDiagnosis(const std::string& diagnosis);

    std::string getPatientType() const override;
    std::string getDetails() const override;
    double calculateConsultationFee() const override;

    bool scheduleAppointment(const std::string& apptId) override;
    bool cancelAppointment(const std::string& apptId) override;
};

}

#endif
