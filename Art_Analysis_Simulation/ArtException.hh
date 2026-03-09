#ifndef ArtException_h
#define ArtException_h 1

#include <G4String.hh>
#include <exception>

class ArtException : public std::exception
{
public:
    ArtException(const G4String& message) : fMessage(message) {}
    virtual ~ArtException() throw() {}
    
    virtual const char* what() const throw() override {
        return fMessage.c_str();
    }
    
    const G4String& GetMessage() const { return fMessage; }

private:
    G4String fMessage;
};

class MaterialNotFoundException : public ArtException
{
public:
    MaterialNotFoundException(const G4String& material) 
        : ArtException("Material not found: " + material) {}
};

class InvalidParameterException : public ArtException
{
public:
    InvalidParameterException(const G4String& parameter, const G4String& value)
        : ArtException("Invalid parameter value for " + parameter + ": " + value) {}
};

class AnalysisException : public ArtException
{
public:
    AnalysisException(const G4String& operation)
        : ArtException("Analysis operation failed: " + operation) {}
};

#endif
