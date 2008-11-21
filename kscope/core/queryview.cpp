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

#include "queryview.h"
#include "locationlistmodel.h"
#include "locationtreemodel.h"

namespace KScope
{

namespace Core
{

/**
 * Class constructor.
 * @param  parent  The parent widget
 * @param  type    Whether the view works in list or tree modes
 */
QueryView::QueryView(QWidget* parent, Type type)
	: QTreeView(parent), type_(type), progBar_(NULL),
	  autoSelectSingleResult_(false)
{
	// Set tree view properties.
	setRootIsDecorated(type_ == Tree);
	setUniformRowHeights(true);

	// Create an ordered list of columns.
	QList<LocationListModel::Columns> colList;
	colList << Core::LocationListModel::Scope
	        << Core::LocationListModel::File
	        << Core::LocationListModel::Line
	        << Core::LocationListModel::Text;

	// Create a location model.
	switch (type_) {
	case List:
		setModel(new LocationListModel(colList, this));
		break;

	case Tree:
		setModel(new LocationTreeModel(colList, this));
		break;
	}

	// Emit requests for locations when an item is double-clicked.
	connect(this, SIGNAL(activated(const QModelIndex&)), this,
	        SLOT(requestLocation(const QModelIndex&)));
}

/**
 * Class destructor.
 */
QueryView::~QueryView()
{
}

/**
 * Associates a query with this view.
 * Clears the model in preparation of new results.
 * @param  query     The query to associate with this view
 * @param  rootPath  Common root path for displaying results
 */
void QueryView::initQuery(const Query& query, const QString& rootPath)
{
	query_ = query;
	model()->setRootPath(rootPath);
}

/**
 * Called by the engine when results are available.
 * Adds the list of locations to the model.
 * @param  locList  Query results
 */
void QueryView::onDataReady(const LocationList& locList)
{
	model()->add(locList, currentIndex());
}

/**
 * Displays progress information in a progress-bar at the top of the view.
 * @param  text  Progress message
 * @param  cur   Current value
 * @param  total Expected final value
 */
void QueryView::onProgress(const QString& text, uint cur, uint total)
{
	// Create the progress-bar widget, if it does not exist.
	if (!progBar_) {
		progBar_ = new ProgressBar(this);
		connect(progBar_, SIGNAL(cancelled()), this, SLOT(stopQuery()));
		progBar_->show();
	}

	// Update progress information in the progres bar.
	progBar_->setLabel(text);
	progBar_->setProgress(cur, total);

	if (!isVisible())
		emit needToShow();
}

/**
 * Called by the engine when a query terminates normally.
 */
void QueryView::onFinished()
{
	// Destroy the progress-bar, if it exists.
	if (progBar_) {
		delete progBar_;
		progBar_ = NULL;
	}

	// Auto-select a single result, if required.
	Location loc;
	if (autoSelectSingleResult_ && model()->rowCount() == 1
	                            && model()->firstLocation(loc)) {
		emit locationRequested(loc);
	}
	else if (!isVisible()) {
		emit needToShow();
	}
}

/**
 * Called by the engine when a query terminates abnormally.
 */
void QueryView::onAborted()
{
	// Destroy the progress-bar, if it exists.
	if (progBar_) {
		delete progBar_;
		progBar_ = NULL;
	}
}

/**
 * Selects the next location in the list.
 */
void QueryView::selectNext()
{
	QModelIndex selIndex = model()->nextIndex(currentIndex());
	if (selIndex.isValid())
		setCurrentIndex(selIndex);

	Location loc;
	if (model()->locationFromIndex(selIndex, loc))
		emit locationRequested(loc);
}

/**
 * Selects the previous location in the list.
 */
void QueryView::selectPrev()
{
	QModelIndex selIndex = model()->prevIndex(currentIndex());
	if (selIndex.isValid())
		setCurrentIndex(selIndex);

	Location loc;
	if (model()->locationFromIndex(selIndex, loc))
		emit locationRequested(loc);
}

/**
 * Called when the user double-clicks a location item in the list.
 * Emits the locationRequested() signal for this location.
 * @param  index  Identifies the clicked item
 */
void QueryView::requestLocation(const QModelIndex& index)
{
	Location loc;
	if (model()->locationFromIndex(index, loc))
		emit locationRequested(loc);
}

/**
 * Called when the "Cancel" button is clicked in the progress-bar.
 * Informs the engine that the query process should be stopped.
 */
void QueryView::stopQuery()
{
	stop();
}

} // namespace Core

} // namespace KScope
