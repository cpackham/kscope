/***************************************************************************
 *   Copyright (C) 2007-2009 by Elad Lahav
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

#ifndef __CORE_QUERYVIEW_H__
#define __CORE_QUERYVIEW_H__

#include <QTreeView>
#include <QDomDocument>
#include <QDomElement>
#include "globals.h"
#include "engine.h"
#include "progressbar.h"
#include "locationmodel.h"

namespace KScope
{

namespace Core
{

/**
 * A view for displaying query results.
 * Results can be displayed either as either a list or a tree (for call trees).
 * Since the class implements Engine::Connection, an object of this type can be
 * passed to an engine's query() method. Progress will be displayed in the
 * form of a progress-bar at the top of the view.
 * There are two ways in which the widget can be used to view query results:
 * 1. Constructing the widget and passing it to an engine's query() method;
 * 2. Using the widget's own query() method.
 * For the latter to work, engine() needs to be specialised. This is a
 * compromise between functionality and good design. Functionality requires an
 * engine to be available to the widget, in order to be able to refresh queries
 * and expand tree items by itself. Good design requires that we do not keep
 * an embedded engine object/pointer in this class.
 * Note that the tree view can only work with option 2, as the queryTreeItem()
 * method, connected to the expanded() signal, uses the engine to query run a
 * query on a child item.
 * @author Elad Lahav
 */
class QueryView : public QTreeView, public Engine::Connection
{
	Q_OBJECT

public:
	/**
	 * The view can work in either list or tree modes.
	 */
	enum Type { List, Tree };

	QueryView(QWidget*, Type type = List);
	~QueryView();

	void query(const Query&);
	void resizeColumns();
	QDomElement toXML(QDomDocument&) const;
	void fromXML(const QDomElement&);

	/**
	 * In the case the query returns only a single location, determines whether
	 * this location should be selected automatically.
	 * @param  select  true to select a single result, false otherwise
	 */
	void setAutoSelectSingleResult(bool select) {
		autoSelectSingleResult_ = select;
	}

	/**
	 * @return  The type of the view
	 */
	Type type() const { return type_; }

	/**
	 * @return The location model for this view
	 */
	inline LocationModel* model() {
		return static_cast<LocationModel*>(QTreeView::model());
	}

	/**
	 * @return The location model for this view
	 */
	const LocationModel* model() const {
		return static_cast<const LocationModel*>(QTreeView::model());
	}

	// Engine::Connection implementation.
	virtual void onDataReady(const LocationList&);
	virtual void onFinished();
	virtual void onAborted();
	virtual void onProgress(const QString&, uint, uint);

public slots:
	void selectNext();
	void selectPrev();

signals:
	/**
	 * Emitted when a location item is selected.
	 * @param  loc  The location descriptor
	 */
	void locationRequested(const Core::Location& loc);

	/**
	 * Emitted when the view needs to be visible.
	 * This is useful for creating containers that only become visible when
	 * there is something to show in the view (either progress or results).
	 */
	void needToShow();

protected:
	/**
	 * Used by the query() method to launch queries on the engine.
	 * The default implementation returns NULL, which means that a query cannot
	 * be started directly. Re-implement to allow the widget to handle its own
	 * queries.
	 * @return The engine to use for running queries
	 */
	virtual Engine* engine() { return NULL; }

private:
	/**
	 * Whether the view is in list or tree modes.
	 */
	Type type_;

	/**
	 * The query associated with this view.
	 * This can be used, e.g., for re-running the query from within the view.
	 */
	Query query_;

	/**
	 * The index under which query results should be put.
	 */
	QModelIndex queryIndex_;

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

	void locationToXML(QDomDocument&, QDomElement&, const QModelIndex&) const;
	void locationFromXML(const QDomElement&, const QModelIndex&);

private slots:
	void requestLocation(const QModelIndex&);
	void stopQuery();
	void queryTreeItem(const QModelIndex&);
};

} // namespace Core

} // namespace KScope

#endif  // __CORE_QUERYVIEW_H__
