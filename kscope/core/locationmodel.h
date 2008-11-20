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
	/**
	 * Used to choose the columns to display and their order.
	 * Each column corresponds to a field in the Location structure.
	 * The constructor of the model accepts a list of columns that determines
	 * which fields are exposed to the view and in what order.
	 */
	enum Columns {
		/** File path. */
		File,
		/** Line number. */
		Line,
		/** Column number. */
		Column,
		/** Tag type (for tag locations). */
		Tag,
		/** Symbol scope (for tag locations). */
		Scope,
		/** Line text. */
		Text
	};

	LocationModel(QList<Columns>, QObject* parent = 0);
	virtual ~LocationModel();

	void setRootPath(const QString&);

	// QAsbstractItemModel implementation.
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant headerData(int, Qt::Orientation,
	                            int role = Qt::DisplayRole) const;

protected:
	/**
	 * A copy of the list of columns passed to the constructor.
	 */
	QList<Columns> colList_;

	/**
	 * A common root path for all files in the model.
	 * Files for which this path is a prefix will be presented with a '$' sign
	 * as an abbreviation of the common path.
	 */
	QString rootPath_;

	QString columnText(Columns) const;
};

} // namespace Core

} // namespace KScope

#endif // __CORE_LOCATIONMODEL_H__
