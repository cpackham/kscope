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


#ifndef __CORE_LOCATIONVIEW_H__
#define __CORE_LOCATIONVIEW_H__

#include <QTreeView>
#include <QSortFilterProxyModel>
#include <QDomDocument>
#include <QDomElement>
#include "globals.h"
#include "engine.h"
#include "progressbar.h"
#include "locationmodel.h"

namespace KScope
{

namespace Core
{

/**
 * A view for displaying LocationModel models.
 * Adds filtering and save/restore capabilities to a standard QTreeView.
 * @author Elad Lahav
 */
class LocationView : public QTreeView
{
	Q_OBJECT

public:
	/**
	 * The view can work in either list or tree modes.
	 */
	enum Type { List, Tree };

	LocationView(QWidget*, Type type = List);
	~LocationView();

	void resizeColumns();
	virtual void toXML(QDomDocument&, QDomElement&) const;
	virtual void fromXML(const QDomElement&);

	/**
	 * @return  The type of the view
	 */
	Type type() const { return type_; }

	/**
	 * @return The proxy model
	 */
	inline QSortFilterProxyModel* proxy() {
		return static_cast<QSortFilterProxyModel*>(model());
	}

	/**
	 * @return The proxy model
	 */
	inline const QSortFilterProxyModel* proxy() const {
		return static_cast<QSortFilterProxyModel*>(model());
	}

	/**
	 * @return The location model for this view
	 */
	inline LocationModel* locationModel() {
		return static_cast<LocationModel*>(proxy()->sourceModel());
	}

	/**
	 * @return The location model for this view
	 */
	inline const LocationModel* locationModel() const {
		return static_cast<LocationModel*>(proxy()->sourceModel());
	}

public slots:
	void selectNext();
	void selectPrev();

signals:
	/**
	 * Emitted when a location item is selected.
	 * @param  loc  The location descriptor
	 */
	void locationRequested(const Core::Location& loc);

	/**
	 * Emitted when the view needs to be visible.
	 * This is useful for creating containers that only become visible when
	 * there is something to show in the view (either progress or results).
	 */
	void needToShow();

protected:
	/**
	 * Whether the view is in list or tree modes.
	 */
	Type type_;

	virtual void locationToXML(QDomDocument&, QDomElement&,
	                           const QModelIndex&) const;
	virtual void locationFromXML(const QDomElement&, const QModelIndex&);

protected slots:
	void requestLocation(const QModelIndex&);
};

} // namespace Core

} // namespace KScope

#endif // __CORE_LOCATIONVIEW_H__
