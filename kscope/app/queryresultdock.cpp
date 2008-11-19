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

#include "queryresultdock.h"
#include "projectmanager.h"

namespace KScope
{

namespace App
{

/**
 * Class constructor.
 * @param  parent  Parent widget
 */
QueryResultDock::QueryResultDock(QWidget* parent) :
	QDockWidget(tr("Query Results"), parent)
{
	setObjectName("QueryResultDock");
	setWidget(new StackWidget(this));

	titleMap_[Core::Query::Text] = tr("Text search '%1'");
	titleMap_[Core::Query::References] = tr("References to '%1'");
	titleMap_[Core::Query::Definition] = tr("Definition of '%1'");
	titleMap_[Core::Query::CalledFunctions]
		= tr("Functions called by '%1'");
	titleMap_[Core::Query::CallingFunctions]
		= tr("Functions calling '%1'");
	titleMap_[Core::Query::FindFile] = tr("Find file '%1'");
	titleMap_[Core::Query::IncludingFiles] = tr("Files including '%1'");
}

/**
 * Class destructor.
 */
QueryResultDock::~QueryResultDock()
{
}

/**
 * Runs a query and displays its results in a query view.
 * @param  query  The query to run
 * @param  view   If not NULL, uses this view to display the results. Otherwise,
 *                a new QueryView object is created
 */
void QueryResultDock::query(const Core::Query& query, Core::QueryView* view)
{
	// If a view was given, adjust its title.
	// Otherwise, create a new view.
	if (view) {
		// TODO: Implement in StackWidget.
#if 0
		tabWidget()->setItemText(tabWidget()->indexOf(view),
		                         tabTitle(query));
#endif
	}
	else {
		view = addView(tabTitle(query));
	}

	// Run the query.
	view->initQuery(query);
	try {
		ProjectManager::engine().query(*view, query);
	}
	catch (Core::Exception* e) {
		e->showMessage();
		delete e;
	}
}

/**
 * Selects the next location in the current view.
 */
void QueryResultDock::selectNextResult()
{
	Core::QueryView* view
		= static_cast<Core::QueryView*>(tabWidget()->currentWidget());
	if (view != NULL)
		view->selectNext();
}

/**
 * Selects the next location in the current view.
 */
void QueryResultDock::selectPrevResult()
{
	Core::QueryView* view
		= static_cast<Core::QueryView*>(tabWidget()->currentWidget());
	if (view != NULL)
		view->selectPrev();
}

/**
 * Creates a new query view and adds it to the container widget.
 * @param  title  The title of the query view
 * @return The created widget
 */
Core::QueryView* QueryResultDock::addView(const QString& title)
{
	// Create a new query view.
	Core::QueryView* view = new Core::QueryView(this);
	connect(view, SIGNAL(locationRequested(const Core::Location&)), this,
	        SIGNAL(locationRequested(const Core::Location&)));
	view->setAutoSelectSingleResult(true);

	// Add to the tab widget.
	tabWidget()->addTab(view, title);
	view->model()->setRootPath(ProjectManager::project()->rootPath());
	return view;
}

} // namespace App

} // namespace KScope
