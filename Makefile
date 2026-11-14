CXX = w64devkit/bin/g++.exe
CXXFLAGS = -std=c++17 -Wall -O2
LDFLAGS = -mwindows -lcomctl32 -lcomdlg32 -lgdi32 -luser32 -lkernel32

CORE_SRCS = model/PatientRecord.cpp \
            model/GeneralPatient.cpp \
            model/EmergencyPatient.cpp \
            model/FollowUpPatient.cpp \
            model/Appointment.cpp \
            model/ClinicDatabase.cpp \
            controller/SearchEngine.cpp \
            controller/AppointmentController.cpp \
            controller/ClinicManager.cpp \
            utils/IDGenerator.cpp \
            utils/FileHandler.cpp

GUI_SRCS = gui/LayoutManager.cpp \
           gui/PatientPanel.cpp \
           gui/AppointmentPanel.cpp \
           gui/SearchPanel.cpp \
           gui/AdminPanel.cpp \
           gui/MainWindow.cpp \
           main.cpp

all: scapms.exe

scapms.exe: $(CORE_SRCS) $(GUI_SRCS)
	$(CXX) $(CXXFLAGS) $(CORE_SRCS) $(GUI_SRCS) $(LDFLAGS) -o $@

clean:
	del /Q scapms.exe *.o 2>nul || exit 0
