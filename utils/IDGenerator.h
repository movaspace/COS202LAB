#ifndef IDGENERATOR_H
#define IDGENERATOR_H

#include <string>

namespace scapms {

class IDGenerator {
private:
    static int nextPatientNumber;
    static int nextAppointmentNumber;

public:
    static std::string generatePatientId();
    static std::string generateAppointmentId();
    static void syncFromExistingIds(const std::string& patId, const std::string& apptId);
};

}

#endif
