/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_EVENTS_EVENTTYPE_INLUDED
#define FORMULA_EVENTS_EVENTTYPE_INLUDED

/**
 * Different events types used across the application
 */
enum class EventType {
    // Formula management
    noCompilerFound,
	compilationRequest,
	compilationSuccess,
	compilationFail,
	loadFormulaRequest,
    searchFormulaRequest,
    // GUI
    scaleUp,
    scaleDown,
    windowSizeRequested,
	codeSearch,
    // Web client events
    newVersionReleased,
    webRequestSent,
    webRequestFinished,
    // Miscellaneous
    unexpectedError,
	undefined
};

#endif // FORMULA_EVENTS_EVENTTYPE_INLUDED