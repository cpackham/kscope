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

#include <QDebug>
#include "locationtreemodel.h"

namespace KScope
{

namespace Core
{

/**
 * Class constructor.
 * @param  colList  An ordered list of the columns to show
 * @param  parent   Parent object
 */
LocationTreeModel::LocationTreeModel(QList<Columns> colList, QObject* parent)
	: LocationModel(colList, parent)
{
}

/**
 * Class destructor.
 */
LocationTreeModel::~LocationTreeModel()
{
}

/**
 * Appends the given list to the one held by the model.
 * @param  locList  Result information
 * @param  parent   Index under which to add the results
 */
void LocationTreeModel::add(const LocationList& locList,
                            const QModelIndex& parent)
{
	NodeT* node;

	// Determine the node under which to add the results.
	if (!parent.isValid()) {
		node = &root_;
	}
	else {
		node = static_cast<NodeT*>(parent.internalPointer());
		if (node == NULL)
			return;
	}

	// Determine the first and last rows for the new items.
	int firstRow = node->childCount();
	int lastRow = firstRow + locList.size() - 1;
	if (lastRow < firstRow)
		return;

	// Begin row insertion.
	// This is required by QAbstractItemModel.
	beginInsertRows(parent, firstRow, lastRow);

	// Add the entries.
	Location loc;
	foreach (loc, locList)
		node->addChild(loc);

	// End row insertion.
	// This is required by QAbstractItemModel.
	endInsertRows();
}

/**
 * Removes all tree nodes.
 * Resets the model.
 */
void LocationTreeModel::clear()
{
	root_.clear();
	reset();
}

/**
 * Converts an index into a location descriptor.
 * @param  idx  The index to convert
 * @param  loc    An object to fill with the location information
 * @return true if successful, false if the index does not describe a valid
 *         position in the location tree
 */
bool LocationTreeModel::locationFromIndex(const QModelIndex& idx,
                                          Location& loc) const
{
	// Make sure the index is valid.
	if (!idx.isValid())
		return false;

	NodeT* node = static_cast<NodeT*>(idx.internalPointer());
	if (node == NULL)
		return false;

	loc = node->data();
	return true;
}

/**
 * Returns the location at the root of the tree.
 * @param  loc  An object to fill with the location information
 * @return true if successful, false if the list is empty
 */
bool LocationTreeModel::firstLocation(Location& loc) const
{
	loc = root_.data();
	return true;
}

/**
 * Finds the successor of the given index.
 * @param  idx  The index for which to find a successor
 * @return The successor index
 */
QModelIndex LocationTreeModel::nextIndex(const QModelIndex& idx) const
{
	// If the given index is invalid, return the index of the first item on the
	// list.
	if (!idx.isValid())
		return index(0, 0, QModelIndex());

	// Get the tree item for the index.
	const NodeT* node = static_cast<NodeT*>(idx.internalPointer());
	if (node == NULL)
		return QModelIndex();

	// Go up the tree, looking for the first immediate sibling.
	const NodeT* parent;
	while ((parent = node->parent()) != NULL) {
		// Get the node's sibling.
		if (node->index() < parent->childCount()) {
			node = parent->child(node->index() + 1);
			return createIndex(node->index(), 0, (void*)node);
		}

		node = parent;
	}

	return QModelIndex();
}

/**
 * Finds the predecessor of the given index.
 * @param  idx  The index for which to find a predecessor
 * @return The predecessor index
 */
QModelIndex LocationTreeModel::prevIndex(const QModelIndex& idx) const
{
	// If the given index is invalid, return the index of the first item on the
	// list.
	// TODO: What's the best default for a previous index on an invalid index?
	if (!idx.isValid())
		return index(0, 0, QModelIndex());

	// Get the tree item for the index.
	const NodeT* node = static_cast<NodeT*>(idx.internalPointer());
	if (node == NULL)
		return QModelIndex();

	// Go up the tree, looking for the first immediate sibling.
	const NodeT* parent;
	while ((parent = node->parent()) != NULL) {
		// Get the node's sibling.
		if (node->index() > 0) {
			node = parent->child(node->index() - 1);
			return createIndex(node->index(), 0, (void*)node);
		}

		node = parent;
	}

	return QModelIndex();
}

/**
 * Creates an index for the given parameters.
 * @param  row     Row number, with respect to the parent
 * @param  column  Column number
 * @param  parent  Parent index
 * @return The new index, if created, an invalid index otherwise
 */
QModelIndex LocationTreeModel::index(int row, int column,
									 const QModelIndex& parent) const
{
	// An invalid parent is given when the root index is requested.
	if (!parent.isValid())
		return createIndex(0, 0, (void*)&root_);

	// Get the tree item for the parent.
	const NodeT* node = static_cast<NodeT*>(parent.internalPointer());
	if (node == NULL)
		return QModelIndex();

	// Get the child at the row'th position.
	node = node->child(row);
	if (node == NULL)
		return QModelIndex();

	return createIndex(row, column, (void*)node);
}

/**
 * Returns an index for the parent of the given one.
 * @param  idx  The index for which the parent is to be returned
 * @return An invalid index
 */
QModelIndex LocationTreeModel::parent(const QModelIndex& idx) const
{
	if (!idx.isValid())
		return QModelIndex();

	// Get the tree item for the index.
	const NodeT* node = static_cast<NodeT*>(idx.internalPointer());
	if (node == NULL)
		return QModelIndex();

	// Get the parent node.
	node = node->parent();
	if (node == NULL)
		return QModelIndex();

	return createIndex(node->index(), 0, (void*)node);
}

/**
 * Determines the number of children for the given parent index.
 * @param  parent  The parent index
 * @return The number of child indices belonging to the parent
 */
int LocationTreeModel::rowCount(const QModelIndex& parent) const
{
	const NodeT* node;

	if (!parent.isValid()) {
		// An invalid index represents the root item.
		node = &root_;
	}
	else {
		// Get the tree item for the index.
		node = static_cast<NodeT*>(parent.internalPointer());
		if (node == NULL)
			return 0;
	}

	return node->childCount();
}

/**
 * Extracts location data from the given index.
 * @param   index  The index for which data is requested
 * @param   role   The role of the data
 * @return
 */
QVariant LocationTreeModel::data(const QModelIndex& idx, int role) const
{
	// No data for invalid indices.
	if (!idx.isValid())
		return QVariant();

	// Only support DisplayRole.
	if (role != Qt::DisplayRole)
		return QVariant();

	// Get the location for the index's row.
	NodeT* node = static_cast<NodeT*>(idx.internalPointer());
	if (node == NULL)
		return false;

	Location loc = node->data();

	// Get the column-specific data.
	switch (colList_[idx.column()]) {
	case File:
		// File path.
		// Replace root prefix with "$".
		if (!rootPath_.isEmpty() && loc.file_.startsWith(rootPath_))
			return QString("$/") + loc.file_.mid(rootPath_.length());

		return loc.file_;

	case Line:
		// Line number.
		return loc.line_;

	case Column:
		// Column number.
		return loc.column_;

	case Tag:
		// Tag type.
		return loc.tag_;

	case Scope:
		// Scope.
		return loc.scope_;

	case Text:
		// Line text.
		return loc.text_;
	}

	return QVariant();
}

} // namespace Core

} // namespace KScope
