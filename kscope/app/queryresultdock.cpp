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
#include "strings.h"

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
 */
void QueryResultDock::query(const Core::Query& query, bool tree)
{
	Core::QueryView* view;

	if (tree) {
		QString title = tr("Call Tree: ") + Strings::toString(query);
		view = addView(title, Core::QueryView::Tree);
	}
	else {
		view = addView(Strings::toString(query), Core::QueryView::List);
	}

	// Run the query.
	view->initQuery(query, ProjectManager::project()->rootPath());
	try {
		ProjectManager::engine().query(view, query);
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
 * @param  type   Whether to create a list or a tree view
 * @return The created widget
 */
Core::QueryView* QueryResultDock::addView(const QString& title,
                                          Core::QueryView::Type type)
{
	// Create a new query view.
	Core::QueryView* view = new Core::QueryView(this, type);
	connect(view, SIGNAL(locationRequested(const Core::Location&)), this,
	        SIGNAL(locationRequested(const Core::Location&)));
	view->setAutoSelectSingleResult(true);

	// Add to the tab widget.
	tabWidget()->addTab(view, title);
	return view;
}

} // namespace App

} // namespace KScope
