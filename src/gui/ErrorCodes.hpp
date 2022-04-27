#ifndef FORMULA_GUI_ERRORCODES_INCLUDED
#define FORMULA_GUI_ERRORCODES_INCLUDED

namespace formula::gui {
    enum ErrorCodes {
        unknownError = 1,
        // Web
        networkError = 101,
        webResponseError = 102,
        cannotParseJson = 103,
        // Saved files
        cannotImportFile = 201,
        cannotExportFile = 202,
    };
}

#endif //FORMULA_GUI_ERRORCODES_INCLUDED
