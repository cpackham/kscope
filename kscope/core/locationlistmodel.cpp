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

#include <QtDebug>
#include "locationlistmodel.h"

namespace KScope
{

namespace Core
{

LocationListModel::LocationListModel(QList<Columns> colList, QObject* parent)
	: QAbstractItemModel(parent),
	  colList_(colList)
{
}

LocationListModel::~LocationListModel()
{
}

bool LocationListModel::locationFromIndex(const QModelIndex& index,
                                          Location& loc) const
{
	if (!index.isValid())
		return false;

	int i = index.row();
	if (i > locList_.size())
		return false;

	loc = locList_.at(i);
	return true;
}

bool LocationListModel::firstLocation(Location& loc) const
{
	if (locList_.isEmpty())
		return false;

	loc = locList_.at(0);
	return true;
}

/**
 * Sets a new common root path for display purposes.
 * @param  path  The new path to set
 */
void LocationListModel::setRootPath(const QString& path)
{
	QString actPath = path;
	if (path == "/")
		actPath = QString();

	if (actPath != rootPath_) {
		rootPath_ = actPath;
		reset();
	}
}

/**
 * Creates an index for the given parameters.
 * @param  row     Row number, with respect to the parent
 * @param  column  Column number
 * @param  parent  Parent index
 * @return The new index, if created, an invalid index otherwise
 */
QModelIndex LocationListModel::index(int row, int column,
									 const QModelIndex& parent) const
{
	if (parent.isValid())
		return QModelIndex();

	return createIndex(row, column, NULL);
}

QModelIndex LocationListModel::parent(const QModelIndex& index) const
{
	(void)index;
	return QModelIndex();
}

QVariant LocationListModel::headerData(int section, Qt::Orientation orient,
									   int role) const
{
	if (orient != Qt::Horizontal || role != Qt::DisplayRole)
		return QVariant();

	return columnText(colList_[section]);
}

int LocationListModel::rowCount(const QModelIndex& parent) const
{
	if (!parent.isValid())
		return locList_.size();

	return 0;
}

int LocationListModel::columnCount(const QModelIndex& parent) const
{
	(void)parent;
	return colList_.size();
}

QVariant LocationListModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole)
		return QVariant();

	const Location& loc = locList_.at(index.row());

	switch (colList_[index.column()]) {
	case File:
		if (!rootPath_.isEmpty() && loc.file_.startsWith(rootPath_))
			return QString("$/") + loc.file_.mid(rootPath_.length());

		return loc.file_;

	case Line:
		return loc.line_;

	case Column:
		return loc.column_;

	case Tag:
		return loc.tag_;

	case Scope:
		return loc.scope_;

	case Text:
		return loc.text_;
	}

	return "";
}

/**
 * Appends the given list to the one held by the model.
 * @param  locList  Result information
 */
void LocationListModel::add(const LocationList& locList)
{
	// Determine the first and last rows for the new items.
	int firstRow = locList_.size();
	int lastRow = firstRow + locList.size() - 1;
	if (lastRow < firstRow)
		return;

	// Begin row insertion.
	// This is required by QAbstractItemModel.
	beginInsertRows(QModelIndex(), firstRow, lastRow);

	// Add the entries.
	// The condition optimises for the case where the list can be internally
	// copied from one object to another.
	// Not sure whether the condition is also checked by the += operator itself
	// (probably it is, but let's be on the safe side).
	if (locList_.isEmpty())
		locList_ = locList;
	else
		locList_ += locList;

	// End row insertion.
	// This is required by QAbstractItemModel.
	endInsertRows();
}

void LocationListModel::clear()
{
	locList_.clear();
}

QString LocationListModel::columnText(Columns col) const
{
	switch (col) {
	case File:
		return "File";

	case Line:
		return "Line";

	case Column:
		return "Column";

	case Tag:
		return "Tag";

	case Scope:
		return "Scope";

	case Text:
		return "Text";
	}

	return "";
}

}

}
