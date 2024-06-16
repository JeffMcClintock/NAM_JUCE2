/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MyAmpPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MyAmpPluginAudioProcessorEditor (MyAmpPluginAudioProcessor&);
    ~MyAmpPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MyAmpPluginAudioProcessor& audioProcessor;

    // !! New code !!
    juce::TextEditor modelPath;
    juce::TextButton loadModelButton;
    std::unique_ptr<juce::FileChooser> myChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyAmpPluginAudioProcessorEditor)
};
