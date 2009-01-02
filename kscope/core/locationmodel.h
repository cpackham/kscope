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

#ifndef __CORE_LOCATIONMODEL_H__
#define __CORE_LOCATIONMODEL_H__

#include <QAbstractItemModel>
#include "globals.h"

namespace KScope
{

namespace Core
{

/**
 * Abstract base-class for location models.
 * Provides a common base for LocationListModel and LocationTreeModel.
 * @author Elad Lahav
 */
class LocationModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	LocationModel(QObject* parent = 0);
	virtual ~LocationModel();

	void setRootPath(const QString&);
	void setColumns(const QList<Location::Fields>&);

	virtual void add(const LocationList&, const QModelIndex&) = 0;
	virtual void clear() = 0;
	virtual bool locationFromIndex(const QModelIndex&, Location&) const = 0;
	virtual bool firstLocation(Location&) const = 0;
	virtual QModelIndex nextIndex(const QModelIndex&) const = 0;
	virtual QModelIndex prevIndex(const QModelIndex&) const = 0;

	// QAsbstractItemModel implementation.
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant headerData(int, Qt::Orientation,
	                            int role = Qt::DisplayRole) const;

protected:
	/**
	 * A copy of the list of columns passed to the constructor.
	 */
	QList<Location::Fields> colList_;

	/**
	 * A common root path for all files in the model.
	 * Files for which this path is a prefix will be presented with a '$' sign
	 * as an abbreviation of the common path.
	 */
	QString rootPath_;

	QVariant locationData(const Location&, uint, int) const;
	QString columnText(Location::Fields) const;
};

} // namespace Core

} // namespace KScope

#endif // __CORE_LOCATIONMODEL_H__
