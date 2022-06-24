#ifndef FORMULA_CLOUD_TCCWRAPPER_INCLUDED
#define FORMULA_CLOUD_TCCWRAPPER_INCLUDED

#include <boost/lexical_cast.hpp>

#include <compiler/CompilerWrapper.hpp>

namespace formula::compiler{
	class TccWrapper : public CompilerWrapper {
	public:
		explicit TccWrapper(const std::shared_ptr<formula::events::EventHub>& eventHubRef);

	protected:
		void sanitizeErrorString(std::string& errStr, bool isMono) override;
		std::string getCompilerPath() override;
		std::vector<std::string> getCompilerArgs(std::string sourcePath, std::string outPath, bool isMono) override;
	};
}

#endif // FORMULA_CLOUD_TCCWRAPPER_INCLUDED