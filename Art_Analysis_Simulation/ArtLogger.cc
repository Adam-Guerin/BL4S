#include "ArtLogger.hh"
#include <sstream>

ArtLogger* ArtLogger::fInstance = nullptr;

ArtLogger* ArtLogger::Instance()
{
    if (!fInstance) {
        fInstance = new ArtLogger();
    }
    return fInstance;
}

void ArtLogger::SetLogFile(const G4String& filename)
{
    if (fLogFile.is_open()) {
        fLogFile.close();
    }
    
    fLogFile.open(filename, std::ios::app);
    if (fLogFile.is_open()) {
        fUseFile = true;
        Info("Log file opened: " + filename);
    } else {
        fUseFile = false;
        Error("Failed to open log file: " + filename);
    }
}

void ArtLogger::Log(LogLevel level, const G4String& message)
{
    if (level < fLogLevel) {
        return;
    }
    
    G4String timestamp = GetTimestamp();
    G4String levelStr = LevelToString(level);
    G4String logMessage = "[" + timestamp + "] [" + levelStr + "] " + message;
    
    // Always log to console
    std::cout << logMessage << std::endl;
    
    // Also log to file if enabled
    if (fUseFile && fLogFile.is_open()) {
        fLogFile << logMessage << std::endl;
        fLogFile.flush();
    }
}

void ArtLogger::Flush()
{
    std::cout.flush();
    if (fLogFile.is_open()) {
        fLogFile.flush();
    }
}

G4String ArtLogger::GetTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return G4String(ss.str());
}

G4String ArtLogger::LevelToString(LogLevel level)
{
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO ";
        case LogLevel::WARNING: return "WARN ";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::FATAL:   return "FATAL";
        default:                return "UNKN ";
    }
}
