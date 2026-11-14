@echo off
set CXX=w64devkit\bin\g++.exe

echo Compiling SCAPMS GUI Application...
%CXX% -std=c++17 -Wall -O2 -mwindows ^
    model\PatientRecord.cpp ^
    model\GeneralPatient.cpp ^
    model\EmergencyPatient.cpp ^
    model\FollowUpPatient.cpp ^
    model\Appointment.cpp ^
    model\ClinicDatabase.cpp ^
    controller\SearchEngine.cpp ^
    controller\AppointmentController.cpp ^
    controller\ClinicManager.cpp ^
    utils\IDGenerator.cpp ^
    utils\FileHandler.cpp ^
    gui\LayoutManager.cpp ^
    gui\PatientPanel.cpp ^
    gui\AppointmentPanel.cpp ^
    gui\SearchPanel.cpp ^
    gui\AdminPanel.cpp ^
    gui\MainWindow.cpp ^
    main.cpp ^
    -lcomctl32 -lcomdlg32 -lgdi32 -luser32 -lkernel32 ^
    -o scapms.exe

if errorlevel 1 (
    echo Compilation of SCAPMS failed!
    exit /b 1
)

echo Build completed successfully!
echo scapms.exe created.
