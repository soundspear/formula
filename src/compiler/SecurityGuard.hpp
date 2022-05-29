#ifndef FORMULA_COMPILER_SECURITYGUARD_INCLUDED
#define FORMULA_COMPILER_SECURITYGUARD_INCLUDED

#include <JuceHeader.h>

#include <utility>

namespace formula::compiler {
    class SecurityGuard {
    public:
        virtual ~SecurityGuard() = default;
        virtual bool checkLine(std::string& line) = 0;
        std::string& getErrorMessage() { return errorMessage; }

    protected:
        void setErrorMessage(std::string newErrorMessage) {
            errorMessage = std::move(newErrorMessage);
            errorMessage += "\r\n\tYou can ask for a corporate Formula build to bypass this guard in your private cloud tenant.";
        }

    private:
        std::string errorMessage;
    };
}

#endif //FORMULA_COMPILER_SECURITYGUARD_INCLUDED
