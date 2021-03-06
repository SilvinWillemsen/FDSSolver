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
#define CREATECCODE

#ifdef USE_AVX
    #if __AVX2__
        #define AVX_SUPPORTED
        #define OBJECT1D Object1DAVX
        #include <immintrin.h>
    #endif
#else
    #define OBJECT1D Object1D
#endif

/*
    The GUIDefines class contains some global (static) variables that the entire application should know about.
 */

/* The StringCode class contains symbols and their respective 3-digit codes. The first digit determines the type of the symbol:
 - 1: Mathematical operator (=, +, -)
 - 2: Derivatives
     00 - 03 : time
     04 - 07 : space (1D)
     08 - 09 : space (2D)
 - 3: State variable u
 - 900 is reserved for the button name of the add-coefficient button
 - 901: Negative sign
 */

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
            "208",
            "209",
            "210",
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
            String(CharPointer_UTF8 ("\xce\xb4")) + "xx",
            String(CharPointer_UTF8 ("\xce\xb4")) + "yy",
            String(CharPointer_UTF8 ("\xce\x94")),
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
    String u (int numDim, int time, int l = 0, int m = 0)
    {
        String res = "u";
        switch (time)
        {
            case 0:
                res += "Next";
                break;
            case 1:
                break;
            case 2:
                res += "Prev";
                break;
            default:
                std::cout << "Not Possible" << std::endl;
                return "";
        };
        res += "[l";
        
        if (l < 0)
            res += String(l);
        else if (l > 0)
            res += "+" + String(l);
        
        if (numDim == 2)
        {
            res += " + ";
            if (m < 0)
                res += "(m" + String(m) + ") * Nx";
            else if (m == 0)
                res += "m * Nx";
            else if (m > 0)
                res += "(m+" + String(m) + ") * Nx";
        }
        res += "]";
        return res; 
    }
    
//    String uNext (int l = 0) {
//        if (l < 0)
//            return "uNext[l" + String(l) + "]";
//        else if (l == 0)
//            return "uNext[l]";
//        else
//            return "uNext[l+" + String(l) + "]";
//    };
//    String uCur (int l = 0) {
//        if (l < 0)
//            return "u[l" + String(l) + "]";
//        else if (l == 0)
//            return "u[l]";
//        else
//            return "u[l+" + String(l) + "]";
//    };
//    String uPrev (int l = 0) {
//        if (l < 0)
//            return "uPrev[l" + String(l) + "]";
//        else if (l == 0)
//            return "uPrev[l]";
//        else
//            return "uPrev[l+" + String(l) + "]";
//    };
    String forLoop (String eq, int bound, int N) {
        return "\n for (int l = " + String(bound) + "; l < " + String(N - bound) + "; ++l)" +
        "\n { \n    " + eq + " \n }";
    };
    
//    String uNext2D (int l = 0, int m = 0) {
//        String res = "uNext[l";
//        if (l < 0)
//            res += String(l);
//        else if (l > 0)
//            res += "+" + String(l);
//        
//        res += "+m";
//        if (m < 0)
//            res += "(m" + String(m) + ") * Nx";
//        else if (m == 0)
//            res += "m * Nx"
//        else if (m > 0)
//            res += "+(m" + String(m) + ") * Nx"
//
//        res += "]";
//    };
//    String uCur2D (int l = 0,  int m = 0) {
//        String res = "u[l";
//        if (l < 0)
//            res += String(l);
//        else if (l > 0)
//            res += "+" + String(l);
//
//        res += "+m";
//        if (m < 0)
//            res += "(m" + String(m) + ") * Nx";
//        else if (m == 0)
//            res += "m * Nx"
//            else if (m > 0)
//                res += "+(m" + String(m) + ") * Nx"
//
//                res += "]";
//    };
//    String uPrev2d (int l = 0,  int m = 0) {
//        String res = "uPrev[l";
//        if (l < 0)
//            res += String(l);
//        else if (l > 0)
//            res += "+" + String(l);
//
//        res += "+m";
//        if (m < 0)
//            res += "(m" + String(m) + ") * Nx";
//        else if (m == 0)
//            res += "m * Nx"
//            else if (m > 0)
//                res += "+(m" + String(m) + ") * Nx"
//
//                res += "]";
//    };
    
    String forLoop2D (String eq, int bound, int Nx, int Ny) {
        return "\n for (int l = " + String(bound) + "; l < " + String(Nx - bound + 1) + "; ++l) \n {" +
        "for (int m = " + String(bound) + "; m < " + String(Ny - bound + 1) + "; ++m)" +
        "\n { \n    " + eq + " \n } \n }";
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
    
    addCoeffAction,
    createPMAction,
    
    muteObject,
    editObject,
    removeObject,
    objectClicked,
    
    newObjectAction,
    cancelNewObjectAction,
    muteAction,
    createPredefinedModel,
    
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

    static const int maxDim = 2;
};
