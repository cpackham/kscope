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

#ifndef __CORE_QUERYVIEW_H
#define __CORE_QUERYVIEW_H

#include <QTreeView>
#include "globals.h"
#include "engine.h"
#include "progressbar.h"
#include "locationlistmodel.h"

namespace KScope
{

namespace Core
{

/**
 * A view for displaying query results.
 * Since the class implements Engine::Connection, an object of this type it can
 * be passed to an engine's query method. Progress will be displayed in the
 * form of a progress-bar at the top of the view.
 * @todo: This class only supports LocationListModel at the moment. A location
 * tree-model needs to be implemented and be supported by this view.
 * @author Elad Lahav
 */
class QueryView : public QTreeView, public Engine::Connection
{
	Q_OBJECT

public:
	QueryView(QWidget* parent = 0);
	~QueryView();

	void initQuery(const Query&);

	/**
	 * In the case the query returns only a single location, determines whether
	 * this location should be selected automatically.
	 * @param  select  true to select a single result, false otherwise
	 */
	void setAutoSelectSingleResult(bool select) {
		autoSelectSingleResult_ = select;
	}

	// Engine::Connection implementation.
	virtual void onDataReady(const LocationList&);
	virtual void onFinished();
	virtual void onAborted();
	virtual void onProgress(const QString&, uint, uint);

signals:
	void locationRequested(const Core::Location& loc);

private:
	/**
	 * The query associated with this view.
	 * This can be used, e.g., for re-running the querty from within the view.
	 */
	Query query_;

	/**
	 * A progress-bar for displaying query progress information.
	 * This widget is created upon the first reception of progress information,
	 * and destroyed when the query terminates.
	 */
	ProgressBar* progBar_;

	/**
	 * In the case the query returns only a single location, determines whether
	 * this location should be selected automatically.
	 * @param  select  true to select a single result, false otherwise
	 */
	bool autoSelectSingleResult_;

	inline LocationListModel* model() {
		return static_cast<LocationListModel*>(QTreeView::model());
	}

private slots:
	void handleDoubleClick(const QModelIndex&);
	void stopQuery();
};

} // namespace Core

} // namespace KScope

#endif  // __CORE_QUERYVIEW_H
