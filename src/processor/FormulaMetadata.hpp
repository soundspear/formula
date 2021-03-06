/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_PROCESSOR_FORMULAMETADATA_INCLUDED
#define FORMULA_PROCESSOR_FORMULAMETADATA_INCLUDED

#include <string>

namespace formula::processor {
	using FormulaMetadata = std::unordered_map<std::string, std::string>;

    /**
     * Keys for key-value storage of a formula metadata
     */
	struct FormulaMetadataKeys {
		static inline const std::string name = "Name";
		static inline const std::string description = "Description";
		static inline const std::string source = "Source";
		static inline const std::string compilationId = "CompilationId";
		static inline const std::string created = "Created";
		static inline const std::string lastModified = "LastModified";

		static inline const std::string dryWet = "DryWet";
		static inline const std::string inGain = "InGain";
		static inline const std::string outGain = "OutGain";
		static inline std::string knobName(int knobId) { return std::string("KnobName") + std::to_string(knobId); }
		static inline std::string knobDefaultValue(int knobId) { return std::string("KnobDefaultValue") + std::to_string(knobId); }
		static inline std::string switchName(int switchId) { return std::string("SwitchName") + std::to_string(switchId); }
		static inline std::string switchDefaultValue(int switchId) { return std::string("SwitchDefaultValue") + std::to_string(switchId); }
	};
}

#endif // FORMULA_PROCESSOR_FORMULAMETADATA_INCLUDED