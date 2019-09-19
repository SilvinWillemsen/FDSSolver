/*
  ==============================================================================

    CoefficientList.cpp
    Created: 19 Jul 2019 4:01:57pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "CoefficientList.h"

//==============================================================================
CoefficientList::CoefficientList()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    addAndMakeVisible (list);
    
    coeffTopLabel = new Label();
    coeffTopLabel->setColour (Label::textColourId, Colours::white);
    coeffTopLabel->setText ("Coefficients", dontSendNotification);
    coeffTopLabel->setFont (Font (18.0f));
    addAndMakeVisible (coeffTopLabel);
    coeffTopLabel->setVisible (false);
    
    list.setColour(ListBox::backgroundColourId, getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    list.setRowHeight (GUIDefines::listBoxRowHeight);
}

CoefficientList::~CoefficientList()
{
    for (auto coefficient : coefficients)
        removeCoefficient (coefficient);
    
    repaintAndUpdate();
}

void CoefficientList::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

//    g.fillAll (Colours::yellow);   // clear the background
//    if (numRows != 0)
//        for (int i = 0; i < numRows; ++i)
//            paintListBoxItem (i, g, GUIDefines::buttonAreaWidth, GUIDefines::buttonHeight, false);
}

void CoefficientList::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    Rectangle<int> totArea = getLocalBounds();
    totArea.removeFromTop (GUIDefines::margin);
    totArea.setX (totArea.getX() - 5);
    coeffTopLabel->setBounds (totArea.removeFromTop (GUIDefines::buttonHeight));
    totArea.setX (totArea.getX() + 5);
    totArea.removeFromTop (GUIDefines::margin);

    list.setBounds (totArea);
}

std::shared_ptr<CoefficientComponent> CoefficientList::addCoefficient (const String& name, double value, bool dynamic)
{
    coefficients.push_back (std::make_shared<CoefficientComponent> (name, value, dynamic));
    std::shared_ptr<CoefficientComponent> newCoeff = coefficients[coefficients.size() - 1];
    newCoeff.get()->setApplicationState (appState, false);
    addAndMakeVisible (newCoeff.get());
    return newCoeff;
}

std::shared_ptr<CoefficientComponent> CoefficientList::addCoefficient (std::shared_ptr<CoefficientComponent> coefficient)
{
    coefficients.push_back (coefficient);
    std::shared_ptr<CoefficientComponent> newCoeff = coefficients[coefficients.size() - 1];
    newCoeff.get()->setApplicationState (appState, false);
    addAndMakeVisible (newCoeff.get());
    return newCoeff;
}

void CoefficientList::repaintAndUpdate()
{
    coeffTopLabel->setVisible (coefficients.size() == 0 ? false : true);
    repaint();
    list.updateContent();
}

void CoefficientList::paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
{

    auto alternateColour = getLookAndFeel().findColour (ListBox::backgroundColourId)
    .interpolatedWith (getLookAndFeel().findColour (ListBox::textColourId), 0.03f);
    auto alternateColour2 = getLookAndFeel().findColour (ListBox::backgroundColourId)
    .interpolatedWith (Colours::darkgrey, 0.03f);
    if (rowNumber % 2)
        g.fillAll (alternateColour);
    else
        g.fillAll (alternateColour2);
}

Component* CoefficientList::refreshComponentForRow (int rowNumber, bool isRowSelected, Component* existingComponentToUpdate)
{
    if (rowNumber < coefficients.size())
        return coefficients[rowNumber].get();
    
    return nullptr;
}


int CoefficientList::containsCoefficient (String& coeff)
{
    for (int i = 0; i < coefficients.size(); ++i)
        if (coefficients[i]->getName() == coeff)
            return i;
    return -1;
}

void CoefficientList::removeCoefficient (std::shared_ptr<CoefficientComponent> coeffToRemove, bool eraseFromVector)
{
    for (int i = 0; i < coefficients.size(); ++i)
        if (coefficients[i] == coeffToRemove)
        {
            coefficients[i]->setVisible (false);
            coefficients[i].reset();
            
            if (eraseFromVector)
                coefficients.erase(coefficients.begin() + i);
        }
}

void CoefficientList::emptyCoefficientList (bool update)
{
    for (auto coeff : coefficients)
        removeCoefficient (coeff, false);
    
    for (int i = static_cast<int> (coefficients.size() - 1); i >= 0; --i)
        coefficients.erase(coefficients.begin() + i);
    
    if (update)
        repaintAndUpdate();
}

void CoefficientList::loadCoefficientsFromObject (std::vector<std::shared_ptr<CoefficientComponent>>& coefficientsFromObject)
{
    emptyCoefficientList (false);
    for (auto coeff : coefficientsFromObject)
    {
        coeff->setVisible (true);
        addCoefficient (coeff);
    }
    repaintAndUpdate();
}

NamedValueSet CoefficientList::getNamedValueSet (StringArray coefficientNames)
{
    NamedValueSet namedValueSet;
    for (auto coeff : coefficients)
        if (coefficientNames.isEmpty()) // if the coefficientNames vector is empty, return all coefficients
            namedValueSet.set (coeff.get()->getName(), coeff.get()->getValue());
        else if (coefficientNames.contains (coeff.get()->getName()))
            namedValueSet.set (coeff.get()->getName(), coeff.get()->getValue());
    
    return namedValueSet;
}

void CoefficientList::setApplicationState (ApplicationState applicationState, bool init)
{
    for (auto coefficient : coefficients)
    {
        coefficient->setApplicationState (applicationState, init);
    }
    switch (applicationState)
    {
        case newObjectState:
        case normalAppState:
            emptyCoefficientList();
            break;
        case editObjectState:
            break;
        default:
            break;
    }
    appState = applicationState;
}
