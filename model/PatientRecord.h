#ifndef PATIENTRECORD_H
#define PATIENTRECORD_H

#include <string>
#include <vector>

namespace scapms {

class PatientRecord {
protected:
    std::string patientId;
    std::string name;
    int age;
    std::string contactInfo;
    std::vector<std::string> appointmentIds;

public:
    PatientRecord(const std::string& id, const std::string& name, int age, const std::string& contact);
    virtual ~PatientRecord() = default;

    std::string getId() const;
    std::string getName() const;
    int getAge() const;
    std::string getContactInfo() const;
    const std::vector<std::string>& getAppointmentIds() const;

    void setName(const std::string& newName);
    void setAge(int newAge);
    void setContactInfo(const std::string& newContact);
    void addAppointment(const std::string& apptId);
    void removeAppointment(const std::string& apptId);

    // Pure virtual functions
    virtual std::string getPatientType() const = 0;
    virtual std::string getDetails() const = 0;
    virtual double calculateConsultationFee() const = 0;
};

}

#endif
