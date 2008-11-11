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

#ifndef __KSCOPE_QUERYRESULTDOCK_H
#define __KSCOPE_QUERYRESULTDOCK_H

#include <QDockWidget>
#include "stackwidget.h"
#include "queryview.h"

namespace KScope
{

namespace App
{

/**
 * @author Elad Lahav
 */
class QueryResultDock : public QDockWidget
{
	Q_OBJECT

public:
	QueryResultDock(const QString&, QWidget* parent = 0);
	~QueryResultDock();

	void query(const Core::Query& query, Core::QueryView* view = NULL);
	Core::QueryView* addView(const QString&);

signals:
	void locationRequested(const Core::Location& loc);

private:
	QMap<Core::Query::Type, QString> titleMap_;

	inline StackWidget* tabWidget() {
		return static_cast<StackWidget*>(widget());
	}

	inline QString tabTitle(const Core::Query& query) {
		return titleMap_[query.type_].arg(query.pattern_);
	}
};

}

}

#endif  // __KSCOPE_QUERYRESULTDOCK_H
