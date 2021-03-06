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

#include "DistrhoPlugin.hpp"
#include "Ninjas.hpp"
#include <sndfile.hh>
#include <vector>
#include <iostream>
#include <string>
#include <limits>
#include "Sample.h"

#include "Slice.h"
#include "Voice.h"
#include "ADSR.h"
#include "Mixer.h"

#include "DistrhoPluginInfo.h"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------


// constructor
NinjasPlugin::NinjasPlugin()
    : Plugin ( paramCount, 0, 1 ) //1 parameter, 0 programs (presets) , 1 states
{
    samplerate = getSampleRate();
}

/* --------------------------------------------------------------------------------------------------------
  * Init
*/

/*
    Initialize the parameters.
    This function will be called once, shortly after the plugin is created.
  */
void NinjasPlugin::initParameter ( uint32_t index, Parameter& parameter )
{
    switch ( index )
    {
    case paramNumberOfSlices:
    {
        parameter.hints      = kParameterIsAutomable|kParameterIsInteger;
        parameter.ranges.def = 1.0f;
        parameter.ranges.min = 1.0f;
        parameter.ranges.max = 16.0f;
        parameter.name   = "Slices";
        parameter.symbol  = "number_of_slices";
        break;
    }
    case paramAttack:
    {
        parameter.hints      = kParameterIsAutomable ;
        parameter.ranges.def = 0.05f;
        parameter.ranges.min = 0.05f;
        parameter.ranges.max = 1.0f;
        parameter.name   = "Attack";
        parameter.symbol = "attack";
        break;
    }
    case paramDecay:
    {
        parameter.hints      = kParameterIsAutomable ;
        parameter.ranges.def = 0.05f;
        parameter.ranges.min = 0.05f;
        parameter.ranges.max = 1.0f;
        parameter.name   = "Decay";
        parameter.symbol =  "decay";
        break;
    }
    case paramSustain:
    {
        parameter.hints      = kParameterIsAutomable ;
        parameter.ranges.def = 1.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.name = "Sustain";
        parameter.symbol = "sustain";
        break;
    }
    case paramRelease:
    {
        parameter.hints      = kParameterIsAutomable ;
        parameter.ranges.def = 0.05f;
        parameter.ranges.min = 0.05f;
        parameter.ranges.max = 1.0f;
        parameter.name   = "Release";
        parameter.symbol = "release";
        break;
    }
    case paramOneShotFwd:
    {
        parameter.hints      = kParameterIsAutomable|kParameterIsBoolean ;
        parameter.ranges.def = 1.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.name   = "One Shot Forward";
        parameter.symbol  = "one_shot_fwd";
        break;
    }
    case paramOneShotRev:
    {
        parameter.hints      = kParameterIsAutomable|kParameterIsBoolean ;
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.name   = "One Shot Reverse";
        parameter.symbol  = "one_shot_rev";
        break;
    }
    case paramLoopFwd:
    {
        parameter.hints      = kParameterIsAutomable|kParameterIsBoolean ;
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.name   = "Looped Play Forward";
        parameter.symbol  = "loop_fwd";
        break;
    }
    case paramLoopRev:
    {
        parameter.hints      = kParameterIsAutomable|kParameterIsBoolean ;
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.name   = "Looped Play Reverse";
        parameter.symbol  = "loop_rev";
        break;
    }
    case paramFloppy:
    {
        parameter.hints = kParameterIsAutomable|kParameterIsBoolean ;;
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.name   = "Floppy";
        parameter.symbol  = "floppy";
        break;
    }

    case paramSwitch01:
        parameter.hints      = kParameterIsAutomable|kParameterIsBoolean ;
        parameter.ranges.def = 1.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.name   = "Switch "+String ( index - 9 );
        parameter.symbol  = "switch"+String ( index - 9 );

    }

    if ( index >= paramSwitch02 && index <= paramSwitch16 )
    {
        parameter.hints      = kParameterIsAutomable|kParameterIsBoolean ;
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        parameter.name   = "Switch "+String ( index - 9 );
        parameter.symbol  = "switch"+String ( index - 9 );
    }

}

void NinjasPlugin::initState ( uint32_t index, String& stateKey, String& defaultStateValue )
{
    if ( index == 0 )
    {
        stateKey ="filepath";
        defaultStateValue = "empty";
    }

}

String NinjasPlugin::getState ( const char* key ) const
{
    return String ( "filepath" );
}

void NinjasPlugin::setState ( const char* key, const char* value )
{
    if ( strcmp ( key, "filepath" ) == 0 )
    {
        std::string fp = value;
        // load file in sample memory
        if ( !SampleObject.loadSample ( fp, sampleVector, samplerate ) )
        {
            // sample loaded ok, slice it up and set bool
            SampleObject.createSlices ( a_slices,slices );
            bypass = false;
            //setParameterValue(paramFloppy,1.0);
        }
        else
        {
            bypass = true;
            std::cout << "setState - sample not loaded" << std::endl;
            // setState ( "filepath","empty" );
            setParameterValue ( paramFloppy,0.0 );
        }

    }



}

/* --------------------------------------------------------------------------------------------------------
* Internal data
*/

/**
Get the current value of a parameter.
The host may call this function from any context, including realtime processing.
*/
float NinjasPlugin::getParameterValue ( uint32_t index ) const

{
    float return_Value = 0;

    int ch = currentSlice;

    switch ( index )
    {
    case paramNumberOfSlices:
        return_Value = ( float ) slices;
        break;
    case paramAttack:
        return_Value = p_Attack[ch];
        break;
    case paramDecay:
        return_Value = p_Decay[ch];
        break;
    case paramSustain:
        return_Value = p_Sustain[ch];
        break;
    case paramRelease:
        return_Value = p_Release[ch];
        break;
    case paramOneShotFwd: // one shot forward
        if ( a_slices[ch].getSlicePlayMode() == Slice::ONE_SHOT_FWD )
            return_Value = 1;
        else
            return_Value = 0;
        break;
    case paramOneShotRev: // one shot Reverse
        if ( a_slices[ch].getSlicePlayMode() == Slice::ONE_SHOT_REV )
            return_Value = 1;
        else
            return_Value = 0;
        break;
    case paramLoopFwd: // Loop Fwd
        if ( a_slices[ch].getSlicePlayMode() == Slice::LOOP_FWD )
            return_Value = 1;
        else
            return_Value = 0;
        break;
    case paramLoopRev: // Loop Rev
        if ( a_slices[ch].getSlicePlayMode() == Slice::LOOP_REV )
            return_Value = 1;
        else
            return_Value = 0;
        break;
    }
    if ( index >= paramSwitch01 )
    {
        return_Value = p_Grid[index - paramSwitch01];
    }
    return return_Value;

}

/**
Change a parameter value.
The host may call this function from any context, including realtime processing.
When a parameter is marked as automable, you must ensure no non-realtime operations are performed.
@note This function will only be called for parameter inputs.
*/
void NinjasPlugin::setParameterValue ( uint32_t index, float value )
{

    switch ( index )
    {
    case paramNumberOfSlices:
        slices = ( int ) value;
        SampleObject.createSlices ( a_slices,slices );
        break;
    case paramAttack:
        p_Attack[currentSlice] = value;
        break;
    case paramDecay:
        p_Decay[currentSlice] = value;
        break;
    case paramSustain:
        p_Sustain[currentSlice] = value;
        break;
    case paramRelease:
        p_Release[currentSlice] = value;
        break;
    case paramOneShotFwd: // one shot forward
        if ( value == 1 )
            a_slices[currentSlice].setSlicePlayMode ( Slice::ONE_SHOT_FWD );
        break;
    case paramOneShotRev: // one shot Reverse
        if ( value == 1 )
            a_slices[currentSlice].setSlicePlayMode ( Slice::ONE_SHOT_REV );
        break;
    case paramLoopFwd: // Loop Fwd
        if ( value == 1 )
            a_slices[currentSlice].setSlicePlayMode ( Slice::LOOP_FWD );
        break;
    case paramLoopRev: // Loop Rev
        if ( value == 1 )
            a_slices[currentSlice].setSlicePlayMode ( Slice::LOOP_REV );
        break;

    } // switch
    if ( index >= paramSwitch01 )
    {
      std::cout << "JOEHOE!" << std::endl;
        p_Grid[index - paramSwitch01]=value;
        if ( value == 1 )
            currentSlice = index - 10;
    }
} // setParameterValue

/* --------------------------------------------------------------------------------------------------------
* Audio/MIDI Processing */

/*       inputs unused , outputs        , size of block we process, pointer to midi data       , number of midie events in current block */
void NinjasPlugin::run ( const float**, float** outputs, uint32_t frames,          const MidiEvent* midiEvents, uint32_t midiEventCount )
{
    float* const outL = outputs[0]; // output ports , stereo
    float* const outR = outputs[1];
    uint32_t framesDone = 0;
    uint32_t curEventIndex = 0; // index for midi event to process
    while ( framesDone < frames )   // we have frames to process !!
    {
        if ( !bypass )
        {
            /* process any ready midi events */
            //   we have midi data to proces, at precisly the current audio frame in the loop
            while ( curEventIndex < midiEventCount && framesDone == midiEvents[curEventIndex].frame )   // the .frame is the offset of the midi event in current block
            {
                if ( midiEvents[curEventIndex].size > MidiEvent::kDataSize ) // not excatly shure what's happening here. this is in both Nekobi and Kars sourcecode
                    continue;

                int status = midiEvents[curEventIndex].data[0]; // midi status
                // int channel = status & 0x0F ; // get midi channel
                int message = status & 0xF0 ; // get midi message
                int data1 = midiEvents[curEventIndex].data[1];// note number
                int data2 = midiEvents[curEventIndex].data[2]; //
                /*
                std::cout << std::hex << "Status : " << status << std::endl;
                std::cout << std::hex << "
                Message : " << message << std::endl;
                std::cout << std::hex << "
                Data1 : " << data1 << std::endl;
                std::cout << std::hex << "
                Data2 : " << data2 << std::endl;
                */

                // discard notes outside the 16 notes range
                // nn 60 - 74

                if ( !( (message == 0x80 || message == 0x90 ) && ( data1 >= 60 && data1 <= 74 ) ) )
                {
                  std::cout << "Message = " << message << ", Data1 = " << data1 << std::endl;
		  curEventIndex++;
                    continue;
                }

                switch ( message )
                {
                case 0x80 :   // note off
                {
                    int index = data1-60;
                    bool voice_playing = voices[index].active;
                    if ( voice_playing == false )

                        break; // note wasn't playing anyway .. ignore
                    if ( voice_playing )
                        voices[index].adsr.ADSRstage=ADSR::RELEASE;
                    break;
                }

                case 0x90 :
                {
                    int index = data1 - 60;
                    // new note .. let's activate
                    voices[index].active = true;
                    voices[index].velocity = data2;
                    voices[index].gain = ( float ) data2 / 127.0f;
                    voices[index].adsr.initADSR();
                    voices[index].adsr.setADSR ( p_Attack[index], p_Decay[index] ,p_Sustain[index],p_Release[index], samplerate );
                    // check playmode
                    // if LOOP_REV or ONE_SHOT_REV set playback indici to end of slice
                    if ( a_slices[index].getSlicePlayMode() == Slice::LOOP_REV || a_slices[index].getSlicePlayMode() == Slice::ONE_SHOT_REV )
                    {
                        voices[index].playbackIndex = a_slices[index].getSliceEnd();
                        voices[index].multiplierIndex = ( a_slices[index].getSliceEnd() - a_slices[index].getSliceStart() ) /SampleObject.getSampleChannels();
                    }
                    else     // playmode is forward .. playback indici to start
                    {
                        voices[index].playbackIndex = 0;
                        voices[index].multiplierIndex = 0;
                    }

                    float transpose = ( pitchbend/pitchbend_step ) -12;
                    voices[index].multiplier=pow ( 2.0, transpose / 12.0 );
                    // all set . add to stack
                    //stack.add_Voice(&voices[index]);
                    //
                    break;

                } // case 0x90

                case 0xe0:   // pitchbend
                {
                    std::cout << "pitchbend received" << std::endl;
                    pitchbend = ( data2 * 128 ) + data1;
                    break;

                }



                } // switch


                curEventIndex++; // we've processed a midi event,increase index so we know which midi event to process next
            }
            // loop through active voices
            int voice_count {0};
            for ( int i {0} ; i < slices ; i++ )
            {


                if ( voices[i].active )
                {
                    voice_count++;
                    /*get the raw samples from the voice
                    * float* pointer will allow any amount of samples to be pulled in
                    */
                    int sliceStart = a_slices[i].getSliceStart();
                    int sliceEnd = a_slices[i].getSliceEnd();
                    int pos = voices[i].playbackIndex;
                    int channels = SampleObject.getSampleChannels();
                    float* sample = &sampleVector.at ( sliceStart+pos );
                    float sampleL { *sample };
                    float sampleR { * ( sample + ( channels -1 ) ) };
                    // process adsr to get the gain back
                    float adsr_gain = voices[i].adsr.ADSRrun ( &voices[i].active );
                    gain = voices[i].gain * adsr_gain;

                    sampleL = sampleL * gain;
                    sampleR = sampleR * gain;

                    // put samples in mixer

                    mixL.add_Sample ( sampleL );
                    mixR.add_Sample ( sampleR );

                    // increase sample reading position
                    float transpose = ( pitchbend/pitchbend_step ) -12;
                    voices[i].multiplier=pow ( 2.0, transpose / 12.0 );
                    float multiplier = voices[i].multiplier;

                    Slice::slicePlayMode playmode = a_slices[i].getSlicePlayMode();

                    // set multiplier to negative if direction is reverse

                    if ( playmode == Slice::LOOP_REV || playmode == Slice::ONE_SHOT_REV )
                        multiplier=-multiplier;

                    // add the multiplier, when it's negative this should substract

                    voices[i].multiplierIndex += multiplier;
                    int tmp = ( int ) voices[i].multiplierIndex;
                    tmp=tmp * channels;

                    // check bounderies according to playmode: loop or oneshot.

                    switch ( playmode )
                    {
                    case Slice::LOOP_FWD:
                    {
                        if ( tmp >= ( sliceEnd-channels ) )
                        {
                            voices[i].playbackIndex = 0;
                            voices[i].multiplierIndex = 0;
                        }
                        else
                        {
                            voices[i].playbackIndex = tmp;
                        }
                        break;
                    }

                    case Slice::LOOP_REV:
                    {
                        if ( sliceStart + tmp <= sliceStart )
                        {
                            voices[i].playbackIndex = sliceEnd ;
                            voices[i].multiplierIndex = ( sliceEnd -sliceStart ) /channels;
                        }
                        else
                            voices[i].playbackIndex = tmp;
                        break;
                    }
                    case Slice::ONE_SHOT_FWD:
                    {
                        if ( sliceStart + tmp >= ( sliceEnd-channels ) )
                        {
                            voices[i].active=false;
                        }
                        else voices[i].playbackIndex = tmp;
                        break;
                    }
                    case Slice::ONE_SHOT_REV:
                    {
                        if ( sliceStart + tmp <= sliceStart )
                        {
                            voices[i].active=false;
                        }
                        else
                            voices[i].playbackIndex = tmp;
                        break;
                    }

                    } //switch
                }// if voices[i].active
            } // end for loop through active voices
            if ( voice_count == 0 )
            {
                mixL.add_Sample ( 0 );
                mixR.add_Sample ( 0 );
            }


            float left = mixL.get_Mix();
            float right = mixR.get_Mix();
            outL[framesDone] = left;
            outR[framesDone] = right;
        }
        else
        {
            // no voices playing
            outL[framesDone] = 0; // output 0 == silence
            outR[framesDone] = 0;
        }
        ++framesDone;

    } // run()
}

/* ------------------------------------------------------------------------------------------------------------
* Plugin entry point, called by DPF to create a new plugin instance. */

Plugin* createPlugin()
{
    return new NinjasPlugin();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
