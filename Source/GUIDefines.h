/*
  ==============================================================================

    GUIDefines.h
    Created: 12 Jul 2019 2:55:51pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

//#define USE_AVX

#ifdef USE_AVX
    #if __AVX2__
        #define AVX_SUPPORTED
        #define OBJECT1D Object1DAVX
        #include <immintrin.h>
    #endif
#else
    #define OBJECT1D Object1D
#endif

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

struct UEB // update equation blocks
{
    String u (int time, int l = 0)
    {
        switch (time)
        {
            case 0:
                return uNext(l);
            case 1:
                return uCur(l);
            case 2:
                return uPrev(l);
            default:
                std::cout << "Not Possible" << std::endl;
                return "";
        };
    }
    
    String uNext (int l = 0) {
        if (l < 0)
            return "uNext[l" + String(l) + "]";
        else if (l == 0)
            return "uNext[l]";
        else
            return "uNext[l+" + String(l) + "]";
    };
    String uCur (int l = 0) {
        if (l < 0)
            return "u[l" + String(l) + "]";
        else if (l == 0)
            return "u[l]";
        else
            return "u[l+" + String(l) + "]";
    };
    String uPrev (int l = 0) {
        if (l < 0)
            return "uPrev[l" + String(l) + "]";
        else if (l == 0)
            return "uPrev[l]";
        else
            return "uPrev[l+" + String(l) + "]";
    };
    String forLoop (String eq, int bound, int N) {
        return "\n for (int l = " + String(bound) + "; l < " + String(N - bound) + "; ++l)" +
        "\n { \n    " + eq + " \n }";
    };
};

// Is needed to tell the MainComponent the reason for the callback
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
    
    caughtReturnKey,
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
    static const bool AVXdouble = true;
    static const int AVXnum = AVXdouble ? 4 : 8;
    static const int listBoxRowHeight = 40;

};
