#ifndef FORMULA_TCC_WRAPPER_INCLUDED
#define FORMULA_TCC_WRAPPER_INCLUDED

#include <boost/lexical_cast.hpp>

#include <compiler/CompilerWrapper.hpp>

namespace formula::compiler{
	class TccWrapper : public CompilerWrapper {
	public:
		explicit TccWrapper(const std::shared_ptr<formula::events::EventHub>& eventHub);

	protected:
		void sanitizeErrorString(std::string& errStr, bool isMono) override;
		std::string getCompilerPath();
		std::vector<std::string> getCompilerArgs(std::string sourcePath, std::string outPath, bool isMono);
	};
}

#endif // FORMULA_TCC_WRAPPER_INCLUDED