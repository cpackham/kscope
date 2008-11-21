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

#ifndef __APP_QUERYDIALOG_H__
#define __APP_QUERYDIALOG_H__

#include <QDialog>
#include "ui_querydialog.h"
#include "engine.h"

namespace KScope
{

namespace App
{

/**
 * A dialogue that prompts for a query's type and pattern.
 * @author Elad Lahav
 */
class QueryDialog : public QDialog, private Ui::QueryDialog
{
	Q_OBJECT

public:
	typedef QList<Core::Query::Type> TypeList;

	QueryDialog(Core::Query::Type, QWidget* parent = 0);
	QueryDialog(const TypeList&, Core::Query::Type, QWidget* parent = 0);
	~QueryDialog();

	void accept();
	QString pattern();
	void setPattern(const QString&);
	Core::Query::Type type();

private:
	void setupUi(const TypeList&, Core::Query::Type);
};

} // namespace App

} // namespace KScope

#endif // __APP_QUERYDIALOG_H__
