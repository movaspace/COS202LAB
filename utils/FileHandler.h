#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "../model/ClinicDatabase.h"
#include <string>

namespace scapms {

class FileHandler {
public:
    static bool saveDatabase(const ClinicDatabase& db, const std::string& patientsFile, const std::string& appointmentsFile);
    static bool loadDatabase(ClinicDatabase& db, const std::string& patientsFile, const std::string& appointmentsFile);
};

}

#endif
