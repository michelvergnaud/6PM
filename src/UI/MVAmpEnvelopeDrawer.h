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

#ifndef MVAMPENVELOPEDRAWER_H
#define MVAMPENVELOPEDRAWER_H

#include "UI/MVEnvelopeDrawer.h"

class MVAmpEnvelopeDrawer : public MVEnvelopeDrawer
{
public:
    MVAmpEnvelopeDrawer(AmpEnvelopeData &, int &);

protected:
    void paintEvent ( QPaintEvent *) ;
    void contextMenu(QPoint);
    void sustainPointChanged(int);
    void copyEnvelope();
    void pasteEnvelope();

private:
    AmpEnvelopeData & envData;
};

#endif // MVAMPENVELOPEDRAWER_H
