/***************************************************************************
 *   Copyright (C) 2007-2009 by Elad Lahav
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

#ifndef __CORE_LOCATIONTREEMODEL_H__
#define __CORE_LOCATIONTREEMODEL_H__

#include "locationmodel.h"
#include "treeitem.h"

namespace KScope
{

namespace Core
{

/**
 * A tree-like model for holding location results.
 * This is suitable for creating call/calling trees. For flat result lists, use
 * the more efficient LocationListModel.
 * @author Elad Lahav
 */
class LocationTreeModel : public LocationModel
{
	Q_OBJECT

public:
	LocationTreeModel(QObject* parent = 0);
	~LocationTreeModel();

	// LocationModel implementation.
	void add(const LocationList&, const QModelIndex&);
	void setEmpty(const QModelIndex&);
	void clear();
	bool locationFromIndex(const QModelIndex&, Location&) const;
	bool firstLocation(Location&) const;
	QModelIndex nextIndex(const QModelIndex&) const;
	QModelIndex prevIndex(const QModelIndex&) const;

	// QAsbstractItemModel implementation.
	virtual QModelIndex index(int row, int column,
							  const QModelIndex& parent) const;
	virtual QModelIndex parent(const QModelIndex& index) const;
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
	virtual bool hasChildren (const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex&,
	                      int role = Qt::DisplayRole) const;

private:
	struct CallTreeItem {
		Location loc_;
		bool queried_;

		CallTreeItem(const Location& loc) : loc_(loc), queried_(false) {}
	};

	typedef TreeItem<CallTreeItem> Node;

	/**
	 * The root item of the tree.
	 */
	Node root_;
};

} // namespace Core

} // namespace KScope

#endif // __CORE_LOCATIONTREEMODEL_H__
