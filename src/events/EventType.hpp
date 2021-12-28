#ifndef FORMULA_EVENTS_EVENT_TYPE
#define FORMULA_EVENTS_EVENT_TYPE

enum class EventType {
	compilationRequest,
	compilationSuccess,
	compilationFail,
	loadFormulaRequest,
	saveLocalFormulaRequest,
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