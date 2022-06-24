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