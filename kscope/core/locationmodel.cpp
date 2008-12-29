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

#include "locationmodel.h"

namespace KScope
{

namespace Core
{

/**
 * Class constructor.
 * @param  parent   Parent object
 */
LocationModel::LocationModel(QObject* parent) : QAbstractItemModel(parent)
{
}

/**
 * Class destructor.
 */
LocationModel::~LocationModel()
{
}

/**
 * Sets a new common root path for display purposes.
 * @param  path  The new path to set
 */
void LocationModel::setRootPath(const QString& path)
{
	QString actPath = path;
	if (path == "/")
		actPath = QString();
	else if (!path.endsWith("/"))
		actPath += "/";

	if (actPath != rootPath_) {
		rootPath_ = actPath;
		reset();
	}
}

/**
 * Determines which fields of a location structure the model supports, and in
 * what order.
 * @param  colList  An ordered list of location structure fields
 */
void LocationModel::setColumns(const QList<Location::Fields>& colList)
{
	colList_ = colList;
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
QVariant LocationModel::headerData(int section, Qt::Orientation orient,
                                   int role) const
{
	if (orient != Qt::Horizontal || role != Qt::DisplayRole)
		return QVariant();

	return columnText(colList_[section]);
}

/**
 * Returns the number of columns for the children of the given parent.
 * @param  parent  The parent index
 * @return The size of the column list for all the children of the root index
 */
int LocationModel::columnCount(const QModelIndex& parent) const
{
	if (!parent.isValid())
		return colList_.size();

	return 0;
}

/**
 * Extracts data from a location object, for the given column index.
 * @param  loc  The location object
 * @param  col  The requested column
 * @return Matching location data, QVariant() if the column is invalid
 */
QVariant LocationModel::locationData(const Location& loc, uint col) const
{
	switch (colList_[col]) {
	case Location::File:
		// File path.
		// Replace root prefix with "$".
		if (!rootPath_.isEmpty() && loc.file_.startsWith(rootPath_))
			return QString("$/") + loc.file_.mid(rootPath_.length());

		return loc.file_;

	case Location::Line:
		// Line number.
		return loc.line_;

	case Location::Column:
		// Column number.
		return loc.column_;

	case Location::TagName:
		// Tag name.
		return loc.tag_.name_;

	case Location::TagType:
		// Tag type.
		return loc.tag_.type_;

	case Location::Scope:
		// Scope.
		return loc.tag_.scope_;

	case Location::Text:
		// Line text.
		return loc.text_;
	}

	return QVariant();
}

/**
 * Creates a column header title for the given column.
 * @param  col  The column for which the title is requested
 * @return A string describing the column
 */
QString LocationModel::columnText(Location::Fields col) const
{
	switch (col) {
	case Location::File:
		return tr("File");

	case Location::Line:
		return tr("Line");

	case Location::Column:
		return tr("Column");

	case Location::TagName:
		return tr("Tag");

	case Location::TagType:
		return tr("Type");

	case Location::Scope:
		return tr("Scope");

	case Location::Text:
		return tr("Text");
	}

	return "";
}

} // namespace Core

} // mamespace KScope
