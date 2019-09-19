/*
  ==============================================================================

    CoefficientList.h
    Created: 19 Jul 2019 4:01:57pm
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CoefficientComponent.h"
//==============================================================================
/*
*/
class CoefficientList    : public Component,//public ListBox,
                           public ListBoxModel
{
public:
    CoefficientList();
    ~CoefficientList();

    void paint (Graphics&) override;
    void resized() override;

    std::shared_ptr<CoefficientComponent> addCoefficient (const String& name, double value, bool dynamic);
    std::shared_ptr<CoefficientComponent> addCoefficient (std::shared_ptr<CoefficientComponent>);
    
    void repaintAndUpdate();
    
    std::vector<std::shared_ptr<CoefficientComponent>>&  getCoefficients() { return coefficients; };
    
    int getNumRows() override { return static_cast<int> (coefficients.size()); };

    void paintListBoxItem (int rowNumber,
                           Graphics& g,
                           int width, int height,
                           bool rowIsSelected) override;
    
    Component* refreshComponentForRow (int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override;
    
    int containsCoefficient (String& coeff);
    
    // if we want to empty the entire vector, we can't erase the vector entries before resetting all shared_ptrs
    void removeCoefficient (std::shared_ptr<CoefficientComponent> coeffToRemove, bool eraseFromVector = true);
    void emptyCoefficientList (bool update = true);
    
    void loadCoefficientsFromObject (std::vector<std::shared_ptr<CoefficientComponent>>& coefficientsFromObject);

    NamedValueSet getNamedValueSet (StringArray coefficientNames = StringArray());
    
    void setApplicationState (ApplicationState applicationState, bool init);
    
private:
    ScopedPointer<Label> coeffTopLabel;
    
    std::vector<std::shared_ptr<CoefficientComponent>> coefficients;
    ListBox list { {}, this };
    
    ApplicationState appState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CoefficientList)
};
