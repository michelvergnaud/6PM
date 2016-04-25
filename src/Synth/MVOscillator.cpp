/*
    6PM
     Copyright (c) 2015-2016, Michel Vergnaud

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; If not, see <http://www.gnu.org/licenses/>.
*/

#include "MVOscillator.h"

#include "MVDefines.h"

MVOscillator::MVOscillator(int i, MVNote * n, const float & noteT) : data(*Globals::presetManager->getCurrentPreset()->getOscData(i)), t(noteT)
{
    num = i;
    note = n;
    amp = note->getAmp();
    v = 0.0f;
    phi = 0.0f;
    oldFreq = freq = note->getFreq() * data.freqratio * data.detune;
    ampEnvelope = new MVAmpEnvelope(*Globals::presetManager->getCurrentPreset()->getOscAmpEnvData(i), note->getAmp(), num);
    freqEnvelope = new MVFreqEnvelope(*Globals::presetManager->getCurrentPreset()->getOscFreqEnvData(i), num);
    lfoTremolo = new MVLfo(*Globals::presetManager->getCurrentPreset()->getTremData(i), MVLfo::TREMOLO);
    lfoVibrato = new MVLfo(*Globals::presetManager->getCurrentPreset()->getVibData(i), MVLfo::VIBRATO);
}

MVOscillator::~MVOscillator()
{
    delete ampEnvelope;
    delete freqEnvelope;
    delete lfoTremolo;
    delete lfoVibrato;
}
