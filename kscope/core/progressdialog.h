/***************************************************************************
 *   Copyright (C) 2007-2008 by Elad Lahav
 *   elad_lahav@users.sourceforge.net
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QProgressDialog>

namespace KScope
{

namespace Core
{

class ProgressDialog : public QProgressDialog
{
	Q_OBJECT

public:
	ProgressDialog(QObject* obj, QWidget* parent = NULL)
		: QProgressDialog(parent) {
		connect(obj, SIGNAL(progress(uint, uint)), this,
		        SLOT(setProgress(uint, uint)));
	}

public slots:
	void setProgress(uint cur, uint total) {
		setValue((int)(cur * 100 / total));
	}
};

}

}

#endif // PROGRESSDIALOG_H
