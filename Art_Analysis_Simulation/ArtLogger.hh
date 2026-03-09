#ifndef ArtLogger_h
#define ArtLogger_h 1

#include <G4String.hh>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    FATAL = 4
};

class ArtLogger
{
public:
    static ArtLogger* Instance();
    
    void SetLogLevel(LogLevel level) { fLogLevel = level; }
    void SetLogFile(const G4String& filename);
    
    void Log(LogLevel level, const G4String& message);
    void Debug(const G4String& message) { Log(LogLevel::DEBUG, message); }
    void Info(const G4String& message) { Log(LogLevel::INFO, message); }
    void Warning(const G4String& message) { Log(LogLevel::WARNING, message); }
    void Error(const G4String& message) { Log(LogLevel::ERROR, message); }
    void Fatal(const G4String& message) { Log(LogLevel::FATAL, message); }
    
    void Flush();

private:
    static ArtLogger* fInstance;
    ArtLogger() : fLogLevel(LogLevel::INFO), fUseFile(false) {}
    
    LogLevel fLogLevel;
    bool fUseFile;
    std::ofstream fLogFile;
    
    G4String GetTimestamp();
    G4String LevelToString(LogLevel level);
};

// Macros for easy logging
#define ART_DEBUG(msg) ArtLogger::Instance()->Debug(msg)
#define ART_INFO(msg) ArtLogger::Instance()->Info(msg)
#define ART_WARNING(msg) ArtLogger::Instance()->Warning(msg)
#define ART_ERROR(msg) ArtLogger::Instance()->Error(msg)
#define ART_FATAL(msg) ArtLogger::Instance()->Fatal(msg)

#endif
