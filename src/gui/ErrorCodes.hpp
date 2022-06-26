/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#ifndef FORMULA_GUI_ERRORCODES_INCLUDED
#define FORMULA_GUI_ERRORCODES_INCLUDED

namespace formula::gui {
    /**
     * Error codes used across the application
     */
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
