/*
  ==============================================================================

    GUIDefines.h
    Created: 12 Jul 2019 2:55:51pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class StringCode
{
    
public:
    static StringArray& getEncoded()
    {
        static StringArray encoded {
            "100",
            "101",
            "102",
            "200",
            "201",
            "202",
            "203",
            "204",
            "205",
            "206",
            "207",
            "300",
            "901"
        };
        return encoded;
    }
    
    static StringArray& getDecoded()
    {
        static StringArray decoded {
            " = ",
            " + ",
            " " + String(CharPointer_UTF8 ("\xe2\x80\x93")) + " ",
            String(CharPointer_UTF8 ("\xce\xb4")) + "t+",
            String(CharPointer_UTF8 ("\xce\xb4")) + "t-",
            String(CharPointer_UTF8 ("\xce\xb4")) + "t" + String (CharPointer_UTF8 ("\xc2\xb7")),
            String(CharPointer_UTF8 ("\xce\xb4")) + "tt",
            String(CharPointer_UTF8 ("\xce\xb4")) + "x+",
            String(CharPointer_UTF8 ("\xce\xb4")) + "x-",
            String(CharPointer_UTF8 ("\xce\xb4")) + "x" + String (CharPointer_UTF8 ("\xc2\xb7")),
            String(CharPointer_UTF8 ("\xce\xb4")) + "xx",
            "u",
            "-"
        };
        return decoded;
    }
    
    static int getStringArraySize() {
        if (getEncoded().size() != getDecoded().size())
        {
            std::cout << "StringArrays are not the same size." << std::endl;
            return 0;
        } else {
            return getEncoded().size();
        }
        
    };
};

enum Action
{
    noAction,
    insertCoeff,
    editCoeff,
    removeCoeff,
    sliderMoved,
    
    muteObject,
    editObject,
    removeObject,
    objectClicked,
};

enum ApplicationState
{
    normalAppState,
    newObjectState,
    editObjectState,
};

class GUIDefines
{
public:
    static const int margin = 10;
    static const int buttonHeight = 30;
    static const int buttonAreaWidth = 250;
    static const int buttonWidth = (buttonAreaWidth - margin * 4) / 4.0;
    static const int calculatorHeight = 200;
    static const bool debug = false;
    constexpr static const double horStateArea = 0.9;
    
    static const int listBoxRowHeight = 40;

};
