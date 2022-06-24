/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_EVENTS_EVENTTYPE_INLUDED
#define FORMULA_EVENTS_EVENTTYPE_INLUDED

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
    // Web client events
    newVersionReleased,
    webRequestSent,
    webRequestFinished,
    loginSuccess,
    loginFail,
    needLogin,
    needSetUsername,
    userNameAlreadyExists,
    subscriptionExpired,
    listFormulaResponse,
    getFormulaResponse,
    formulaAlreadyExists,
    createFormulaSuccess,
    ratingSubmitted,
    // Miscellaneous
    unexpectedError,
};

#endif // FORMULA_EVENTS_EVENTTYPE_INLUDED