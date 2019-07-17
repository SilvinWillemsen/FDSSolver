/*
  ==============================================================================

    GUIDefines.h
    Created: 12 Jul 2019 2:55:51pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

enum Action
{
    noAction,
    insertCoeff,
    editCoeff,
    removeCoeff,
    sliderMoved,
    editObject,
    removeObject,
};

class GUIDefines
{
public:
    static const int margin = 10;
    static const int buttonHeight = 30;
    static const int areaWidth = 250;
    static const int buttonWidth = (areaWidth - margin * 5) / 4.0;
    
    static const bool debug = false;
    constexpr static const double horStateArea = 0.9;

};
