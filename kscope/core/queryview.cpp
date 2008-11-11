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

namespace KScope
{

namespace Core
{

QueryView::QueryView(QWidget* parent) : QTreeView(parent),
	                                    progBar_(NULL),
	                                    autoSelectSingleResult_(false)
{
	setRootIsDecorated(false);
	setUniformRowHeights(true);

	QList<LocationListModel::Columns> colList;

	colList << Core::LocationListModel::Scope
			<< Core::LocationListModel::File
			<< Core::LocationListModel::Line
			<< Core::LocationListModel::Text;

	setModel(new LocationListModel(colList, this));

	connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this,
	        SLOT(handleDoubleClick(const QModelIndex&)));
}

QueryView::~QueryView()
{
}

void QueryView::initQuery(const Query& query)
{
	query_ = query;
	model()->clear();
}

void QueryView::onDataReady(const LocationList& locList)
{
	model()->add(locList);
}

void QueryView::onProgress(const QString& text, uint cur, uint total)
{
	if (!progBar_) {
		progBar_ = new ProgressBar(this);
		progBar_->show();
	}

	progBar_->setLabel(text);
	progBar_->setProgress(cur, total);
}

void QueryView::onFinished()
{
	if (progBar_) {
		delete progBar_;
		progBar_ = NULL;
	}

	Location loc;
	if (autoSelectSingleResult_ && model()->firstLocation(loc))
		emit locationRequested(loc);
}

void QueryView::onAborted()
{
	if (progBar_) {
		delete progBar_;
		progBar_ = NULL;
	}
}

void QueryView::handleDoubleClick(const QModelIndex& index)
{
	Location loc;

	if (model()->locationFromIndex(index, loc))
		emit locationRequested(loc);
}

}

}
