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

#ifndef __KSCOPE_QUERYDIALOG_H
#define __KSCOPE_QUERYDIALOG_H

#include <QDialog>
#include "ui_querydialog.h"
#include "engine.h"

namespace KScope
{

namespace App
{

/**
 * @author Elad Lahav
 */
class QueryDialog : public QDialog, private Ui::QueryDialog
{
	Q_OBJECT

public:
	QueryDialog(Core::Query::Type, QWidget* parent = 0);
	QueryDialog(QWidget* parent = 0);
	~QueryDialog();

	QString pattern();
	void setPattern(const QString&);
	Core::Query::Type type();
};

}

}

#endif // __KSCOPE_QUERYDIALOG_H
