#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>

namespace scapms {

class Appointment {
private:
    std::string appointmentId;
    std::string patientId;
    std::string date;
    std::string time;
    std::string reason;
    std::string status;

public:
    Appointment(const std::string& apptId, const std::string& patId, const std::string& date, const std::string& time, const std::string& reason, const std::string& status = "Pending");

    std::string getAppointmentId() const;
    std::string getPatientId() const;
    std::string getDate() const;
    std::string getTime() const;
    std::string getReason() const;
    std::string getStatus() const;

    void setDate(const std::string& newDate);
    void setTime(const std::string& newTime);
    void setReason(const std::string& newReason);
    void setStatus(const std::string& newStatus);
};

}

#endif
