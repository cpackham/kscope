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

/**
 * Class constructor.
 * @param  colList  An ordered list of the columns to show
 * @param  parent   Parent object
 */
LocationListModel::LocationListModel(QList<Columns> colList, QObject* parent)
	: QAbstractItemModel(parent),
	  colList_(colList)
{
}

/**
 * Class destructor.
 */
LocationListModel::~LocationListModel()
{
}

/**
 * Converts an index into a location descriptor.
 * @param  idx  The index to convert
 * @param  loc    An object to fill with the location information
 * @return true if successful, false if the index does not describe a valid
 *         position in the location list
 */
bool LocationListModel::locationFromIndex(const QModelIndex& idx,
                                          Location& loc) const
{
	// Make sure the index is valid.
	if (!idx.isValid())
		return false;

	// Make sure the index is inside the list's boundaries.
	int pos = idx.row();
	if (pos < 0 || pos >= locList_.size())
		return false;

	// Copy the location descriptor.
	loc = locList_.at(pos);
	return true;
}

/**
 * Returns the location at the first position in the list.
 * @param  loc  An object to fill with the location information
 * @return true if successful, false if the list is empty
 */
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
 * Finds the successor of the given index.
 * In a list, this is simply the next item.
 * @param  idx  The index for which to find a successor
 * @return The successor index
 */
QModelIndex LocationListModel::nextIndex(const QModelIndex& idx) const
{
	// If the given index is invalid, return the index of the first item on the
	// list.
	if (!idx.isValid())
		return index(0, 0, QModelIndex());

	// Do not go past the last item.
	if (idx.row() >= (rowCount() - 1))
		return QModelIndex();

	// Return the index of the next item.
	return index(idx.row() + 1, 0, idx.parent());
}

/**
 * Finds the predecessor of the given index.
 * In a list, this is simply the previous item.
 * @param  idx  The index for which to find a predecessor
 * @return The predecessor index
 */
QModelIndex LocationListModel::prevIndex(const QModelIndex& idx) const
{
	// If the given index is invalid, return the index of the last item on the
	// list.
	if (!idx.isValid())
		return index(rowCount() - 1, 0, QModelIndex());

	// Do not go before the first item.
	if (idx.row() <= 0)
		return QModelIndex();

	// Return the index of the next item.
	return index(idx.row() - 1, 0, idx.parent());
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

	if (row < 0 || row >= locList_.size())
		return QModelIndex();

	return createIndex(row, column, NULL);
}

/**
 * Returns an index for the parent of the given one.
 * Since this is a flat list, there are no parent indices, so an invalid index
 * is always returned.
 * @param  idx  The index for which the parent is to be returned
 * @return An invalid index
 */
QModelIndex LocationListModel::parent(const QModelIndex& idx) const
{
	(void)idx;
	return QModelIndex();
}

/**
 * Provides information for constructing a header when this model is displayed
 * in a view.
 * @param  section  Corresponds to the column number
 * @param  orient   Horizontal or vertical header
 * @param  role     How to interpret the information
 * @return For a horizontal header with a display role, returns the column
 *         title; An empty variant for all other type
 */
QVariant LocationListModel::headerData(int section, Qt::Orientation orient,
									   int role) const
{
	if (orient != Qt::Horizontal || role != Qt::DisplayRole)
		return QVariant();

	return columnText(colList_[section]);
}

/**
 * Determines the number of children for the given parent index.
 * For a flat list, this is the number of items for the root (invalid) index,
 * and 0 for any other index.
 * @param  parent  The parent index
 * @return The number of child indices belonging to the parent
 */
int LocationListModel::rowCount(const QModelIndex& parent) const
{
	if (!parent.isValid())
		return locList_.size();

	return 0;
}

/**
 * Returns the number of columns for the children of the given parent.
 * @param  parent  The parent index
 * @return The size of the column list for all the children of the root index
 */
int LocationListModel::columnCount(const QModelIndex& parent) const
{
	if (!parent.isValid())
		return colList_.size();

	return 0;
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

} // namespace Core

} // namespace KScope
