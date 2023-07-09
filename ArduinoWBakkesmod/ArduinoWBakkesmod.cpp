#include "pch.h"
#include "ArduinoWBakkesmod.h"
#include <windows.h>
#include <iostream>

BAKKESMOD_PLUGIN(ArduinoWBakkesmod, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
HANDLE arduinoPort = INVALID_HANDLE_VALUE; // Global handle for Arduino port


//CreateFile
//https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
// 
//DCB
//https://learn.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-dcb

//WriteFile
//https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile

//GetCommState
//https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getcommstate


void setupArduinoPort()
{
    std::wstring portName = L"COM1"; // Might need to change this based on your Port
    arduinoPort = CreateFile(
        portName.c_str(),
        GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (arduinoPort == INVALID_HANDLE_VALUE)
    {
        LOG("Failed to connect to Arduino.");
        return;
    }

    // https://learn.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-dcb
    DCB serialParams{};
    serialParams.DCBlength = sizeof(serialParams);
    serialParams.BaudRate = 9600;
    serialParams.ByteSize = 8;
    serialParams.StopBits = 1;
    serialParams.Parity = 0;



    //https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getcommstate
    if (!GetCommState(arduinoPort, &serialParams))
    {
        LOG("Failed to retrieve serial port parameters.");
        CloseHandle(arduinoPort);
        arduinoPort = INVALID_HANDLE_VALUE;
        return;
    }


    if (!SetCommState(arduinoPort, &serialParams))
    {
        LOG("Failed to set serial port parameters.");
        CloseHandle(arduinoPort);
        arduinoPort = INVALID_HANDLE_VALUE;
    }
}

//General Bakkesmod Structs for StatEvents
struct StatTickerParams {
    uintptr_t Receiver;
    uintptr_t Victim;
    uintptr_t StatEvent;
};
struct StatEventParams {
    uintptr_t PRI;
    uintptr_t StatEvent;
};

void ArduinoWBakkesmod::onLoad()
{
    _globalCvarManager = cvarManager;

    setupArduinoPort();

    if (arduinoPort != INVALID_HANDLE_VALUE) {
        gameWrapper->HookEventWithCallerPost<CarWrapper>("Function TAGame.Car_TA.OnHitBall",
            [](CarWrapper caller, void* params, std::string eventname) {

                // writes 2 through serial port
                //https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile

                DWORD bytesWritten;
                if (WriteFile(arduinoPort, "2", strlen("2"), &bytesWritten, nullptr)) {
                    LOG("Sent data to Arduino.");
                }
                else {
                    LOG("Failed to send data to Arduino.");
                }
            });
        gameWrapper->HookEventWithCallerPost<ServerWrapper>("Function TAGame.GFxHUD_TA.HandleStatEvent",
            [](ServerWrapper caller, void* params, std::string eventname) {

                StatEventParams* pStruct = (StatEventParams*)params;
                PriWrapper playerPRI = PriWrapper(pStruct->PRI);
                StatEventWrapper statEvent = StatEventWrapper(pStruct->StatEvent);
                // if goal
                if (statEvent.GetEventName() == "Goal") {

                    // writes 4 to serial port

                    DWORD bytesWritten;
                    if (WriteFile(arduinoPort, "4", strlen("4"), &bytesWritten, nullptr)) {
                        LOG("Sent data to Arduino.");
                    }
                    else LOG("Failed to send data to Arduino.");

                }
                // if save
                if (statEvent.GetEventName() == "Save" || statEvent.GetEventName() == "EpicSave") {
                    DWORD bytesWritten;
                    //writes 1 to serial port
                    if (WriteFile(arduinoPort, "1", strlen("1"), &bytesWritten, nullptr)) {
                        LOG("Sent data to Arduino.");
                    }
                    else LOG("Failed to send data to Arduino.");
                }
                if (statEvent.GetEventName() == "Demolish" || statEvent.GetEventName() == "Demolition") {
                    DWORD bytesWritten;
                    // writes 3 to serial port
                    if (WriteFile(arduinoPort, "3", strlen("3"), &bytesWritten, nullptr)) {
                        LOG("Sent data to Arduino.");
                    }
                    else LOG("Failed to send data to Arduino.");
                }
            });
    }
}

void ArduinoWBakkesmod::onUnload()
{
    if (arduinoPort != INVALID_HANDLE_VALUE) {
        CloseHandle(arduinoPort);
        arduinoPort = INVALID_HANDLE_VALUE;
    }
}
