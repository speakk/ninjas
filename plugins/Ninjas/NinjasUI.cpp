/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "DistrhoPluginInfo.h"
#include "DistrhoUI.hpp"
#include "NinjasUI.hpp"
#include "NinjasArtwork.hpp"
#include "Ninjas.hpp"


START_NAMESPACE_DISTRHO

namespace Art = NinjasArtwork;

// -----------------------------------------------------------------------------------------------------------
NinjasUI::NinjasUI()
    : UI(Art::backgroundWidth, Art::backgroundHeight),
      fImgBackground(Art::backgroundData, Art::backgroundWidth, Art::backgroundHeight, GL_BGR)
{
  // knobs
  
  fKnobSlices = new ImageKnob(this,
		    Image(Art::rotary_slicesData, Art::rotary_slicesWidth, Art::rotary_slicesHeight, GL_BGRA));
    fKnobSlices->setId(2); // TODO: implement proper parameters
    fKnobSlices->setAbsolutePos(250, 81);
    fKnobSlices->setRange(1.0f, 16.0f);
    fKnobSlices->setDefault(1.0f);
    fKnobSlices->setValue(1.0f);
    fKnobSlices->setRotationAngle(316);
    fKnobSlices->setStep(1.0f);
    fKnobSlices->setCallback(this);
  
  
  // switches
  fSwitchFwd = new ImageSwitch(this,
				  Image(Art::switch_offData, Art::switch_offWidth, Art::switch_offHeight, GL_BGR),
				  Image(Art::switch_onData, Art::switch_onWidth, Art::switch_offHeight, GL_BGR));
  fSwitchFwd->setId(1);
  fSwitchFwd->setAbsolutePos(441,242);
  fSwitchFwd->setCallback(this);
}

   /**
      A parameter has changed on the plugin side.
      This is called by the host to inform the UI about parameter changes.
    */
void NinjasUI::parameterChanged(uint32_t index, float value)
    {
  switch (index)
    {
    case 1:
        fSwitchFwd->setDown(value > 0.5f);
        break;
    case 2:
       fKnobSlices->setValue(value);
       break;
   
    }
}
/* ----------------------------------------------------------------------------------------------------------
 * Widget Callbacks
 *----------------------------------------------------------------------------------------------------------*/
 void NinjasUI::imageSwitchClicked(ImageSwitch* imageSwitch, bool down)
{
    const uint buttonId(imageSwitch->getId());

    editParameter(buttonId, true);
    setParameterValue(buttonId, down ? 1.0f : 0.0f);
    editParameter(buttonId, false);
}

void NinjasUI::imageKnobDragStarted(ImageKnob* knob)
{
    editParameter(knob->getId(), true);
}

void NinjasUI::imageKnobDragFinished(ImageKnob* knob)
{
    editParameter(knob->getId(), false);
}

void NinjasUI::imageKnobValueChanged(ImageKnob* knob, float value)
{
    setParameterValue(knob->getId(), value);
}


void NinjasUI::onDisplay()
{
    fImgBackground.draw();
}



/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI* createUI()
{
    return new NinjasUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
