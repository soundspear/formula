/*
    Copyright (c) 2022 Soundspear.
    Distributed under the Boost Software License, Version 1.0.
    See http://www.boost.org/LICENSE_1_0.txt
*/

#include "FormulaCodeTokenizer.hpp"

formula::gui::FormulaCodeTokenizer::FormulaCodeTokenizer() {
    formulaMacros.insert({
        "DEBUG",
        "KNOB_1", "KNOB_2","KNOB_3","KNOB_4","KNOB_5","KNOB_6",
        "KNOB_7", "KNOB_8","KNOB_9","KNOB_10","KNOB_11","KNOB_12",
        "SAMPLE_RATE",
        "SWITCH_1", "SWITCH_2","SWITCH_3",
        "TIME"
     });

    formulaKeywords.insert(
       { "do", "if", "for", "int", "not", "auto", "case", "char",
         "else", "enum", "goto", "long", "void", "break", "const", "float",
         "short", "union", "while", "double", "extern", "inline",
         "return", "signed", "sizeof", "static", "struct", "switch",
         "typedef", "continue",  "register", "unsigned", "volatile"}
    );
}

bool formula::gui::FormulaCodeTokenizer::isFormulaMacro(const String &token) {
    return formulaMacros.contains(token);
}

bool formula::gui::FormulaCodeTokenizer::isFormulaKeyword(const String &token) {
    return formulaKeywords.contains(token);
}

CodeEditorComponent::ColourScheme formula::gui::FormulaCodeTokenizer::getDefaultColourScheme() {
    struct Type
    {
        const char* name;
        juce::uint32 colour;
    };

    const Type types[] =
            {
                    { "FormulaMacro",       0xff62a6da },
                    { "Error",              0xffe60000 },
                    { "Comment",            0xff72d20c },
                    { "Keyword",            0xffee6f6f },
                    { "Operator",           0xffc4eb19 },
                    { "Identifier",         0xffcfcfcf },
                    { "Integer",            0xff42c8c4 },
                    { "Float",              0xff885500 },
                    { "String",             0xffbc45dd },
                    { "Bracket",            0xff058202 },
                    { "Punctuation",        0xffcfbeff },
                    { "Preprocessor Text",  0xfff8f631 }
            };

    CodeEditorComponent::ColourScheme cs;

    for (auto& t : types)
        cs.set(t.name, Colour(t.colour));

    return cs;
}

int formula::gui::FormulaCodeTokenizer::readNextToken(CodeDocument::Iterator& source) {
    source.skipWhitespace();
    auto firstChar = source.peekNextChar();

    switch (firstChar)
    {
        case 0:
            break;

        case '0':   case '1':   case '2':   case '3':   case '4':
        case '5':   case '6':   case '7':   case '8':   case '9':
        case '.':
        {
            auto result = CppTokeniserFunctions::parseNumber (source);

            if (result == FormulaCodeTokenizer::tokenType_error)
            {
                source.skip();

                if (firstChar == '.')
                    return FormulaCodeTokenizer::tokenType_punctuation;
            }

            return result;
        }

        case ',':
        case ';':
        case ':':
            source.skip();
            return FormulaCodeTokenizer::tokenType_punctuation;

        case '(':   case ')':
        case '{':   case '}':
        case '[':   case ']':
            source.skip();
            return FormulaCodeTokenizer::tokenType_bracket;

        case '"':
        case '\'':
            CppTokeniserFunctions::skipQuotedString (source);
            return FormulaCodeTokenizer::tokenType_string;

        case '+':
            source.skip();
            CppTokeniserFunctions::skipIfNextCharMatches (source, '+', '=');
            return FormulaCodeTokenizer::tokenType_operator;

        case '-':
        {
            source.skip();
            auto result = CppTokeniserFunctions::parseNumber (source);

            if (result == FormulaCodeTokenizer::tokenType_error)
            {
                CppTokeniserFunctions::skipIfNextCharMatches (source, '-', '=');
                return FormulaCodeTokenizer::tokenType_operator;
            }

            return result;
        }

        case '*':   case '%':
        case '=':   case '!':
            source.skip();
            CppTokeniserFunctions::skipIfNextCharMatches (source, '=');
            return FormulaCodeTokenizer::tokenType_operator;

        case '/':
        {
            source.skip();
            auto nextChar = source.peekNextChar();

            if (nextChar == '/')
            {
                source.skipToEndOfLine();
                return FormulaCodeTokenizer::tokenType_comment;
            }

            if (nextChar == '*')
            {
                source.skip();
                CppTokeniserFunctions::skipComment (source);
                return FormulaCodeTokenizer::tokenType_comment;
            }

            if (nextChar == '=')
                source.skip();

            return FormulaCodeTokenizer::tokenType_operator;
        }

        case '?':
        case '~':
            source.skip();
            return FormulaCodeTokenizer::tokenType_operator;

        case '<':   case '>':
        case '|':   case '&':   case '^':
            source.skip();
            CppTokeniserFunctions::skipIfNextCharMatches (source, firstChar);
            CppTokeniserFunctions::skipIfNextCharMatches (source, '=');
            return FormulaCodeTokenizer::tokenType_operator;

        case '#':
            CppTokeniserFunctions::skipPreprocessorLine (source);
            return FormulaCodeTokenizer::tokenType_preprocessor;

        default:
            if (CppTokeniserFunctions::isIdentifierStart (firstChar))
                return parseFormulaIdentifier(source);

            source.skip();
            break;
    }

    return FormulaCodeTokenizer::tokenType_error;
}

int formula::gui::FormulaCodeTokenizer::parseFormulaIdentifier (CodeDocument::Iterator& source)
{
    int tokenLength = 0;
    String::CharPointerType::CharType possibleIdentifier[100];
    String::CharPointerType possible (possibleIdentifier);

    while (CppTokeniserFunctions::isIdentifierBody (source.peekNextChar()))
    {
        auto c = source.nextChar();

        if (tokenLength < 20)
            possible.write (c);

        ++tokenLength;
    }

    if (tokenLength > 1 && tokenLength <= 16)
    {
        possible.writeNull();
        juce::String tokenStr(String::CharPointerType (possibleIdentifier), tokenLength);
        if (isFormulaMacro(tokenStr))
            return FormulaCodeTokenizer::tokenType_formulaMacro;
        else if (isFormulaKeyword(tokenStr))
            return FormulaCodeTokenizer::tokenType_keyword;
    }

    return FormulaCodeTokenizer::tokenType_identifier;
}
