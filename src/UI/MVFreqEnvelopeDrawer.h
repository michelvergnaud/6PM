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

#ifndef MVFREQENVELOPEDRAWER_H
#define MVFREQENVELOPEDRAWER_H

#include "MVEnvelopeDrawer.h"

class MVFreqEnvelopeDrawer : public MVEnvelopeDrawer
{
public:
    MVFreqEnvelopeDrawer(FreqEnvelopeData &, int &);

protected:
    void paintEvent ( QPaintEvent *) ;
    void contextMenu(QPoint);
    void sustainPointChanged(int);
    void copyEnvelope();
    void pasteEnvelope();

private:
    FreqEnvelopeData & envData;


};

#endif // MVFREQENVELOPEDRAWER_H
