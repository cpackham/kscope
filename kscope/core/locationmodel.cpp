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
 * @param  colList  An ordered list of the columns to show
 * @param  parent   Parent object
 */
LocationModel::LocationModel(QList<Columns> colList, QObject* parent)
	: QAbstractItemModel(parent),
	  colList_(colList)
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
 * Creates a column header title for the given column.
 * @param  col  The column for which the title is requested
 * @return A string describing the column
 */
QString LocationModel::columnText(Columns col) const
{
	switch (col) {
	case File:
		return tr("File");

	case Line:
		return tr("Line");

	case Column:
		return tr("Column");

	case Tag:
		return tr("Tag");

	case Scope:
		return tr("Scope");

	case Text:
		return tr("Text");
	}

	return "";
}

} // namespace Core

} // mamespace KScope
