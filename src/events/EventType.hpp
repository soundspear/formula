#ifndef FORMULA_EVENTS_EVENT_TYPE
#define FORMULA_EVENTS_EVENT_TYPE

enum class EventType {
    // Formula management
	compilationRequest,
	compilationSuccess,
	compilationFail,
	loadFormulaRequest,
    searchFormulaRequest,
    // GUI
    scaleUp,
    scaleDown,
    // Web client events
    webRequestSent,
    webRequestFinished,
    loginSuccess,
    loginFail,
    refreshAccessTokenFail,
    subscriptionExpired,
    listFormulaResponse,
    getFormulaResponse,
    createFormulaSuccess,
    ratingSubmitted,
    unknownWebError,
};

#endif // FORMULA_EVENTS_EVENT_TYPE