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

#ifndef __APP_LOCATIONHISTORY_H__
#define __APP_LOCATIONHISTORY_H__

#include <QList>
#include "globals.h"

namespace KScope
{

namespace App
{

/**
 * Keeps track of visited locations in the source code.
 * @author Elad Lahav
 */
class LocationHistory
{
public:
	LocationHistory() : pos_(-1), insertItr_(locList_.end()) {}
	~LocationHistory() {}

	/**
	 * Adds a location descriptor to the list.
	 * The location is added immediately after the current position. If this
	 * is not the end of the list, all locations following the new one are
	 * discarded.
	 * @param  loc  The descriptor to add
	 */
	void add(const Core::Location& loc) {
		// Don't do anything if the location is the same as the current one.
		if (pos_ >= 0) {
			Core::Location curLoc = locList_.at(pos_);
			if (curLoc == loc)
				return;
		}

		// Remove any location descriptors after the current position.
		if (insertItr_ != locList_.end())
			locList_.erase(insertItr_, locList_.end());

		// Add the new descriptor.
		locList_.append(loc);
		pos_++;
		insertItr_ = locList_.end();
	}

	/**
	 * Retrieves the next descriptor in the list, updating the current position.
	 * @param  loc  A descriptor to fill
	 * @return true if successful, false if already at the end of the list
	 */
	bool next(Core::Location& loc) {
		if (insertItr_ == locList_.end())
			return false;

		// Update the current position.
		++pos_;
		++insertItr_;

		// Get the new current descriptor.
		Q_ASSERT(pos_ >= 0 && pos_ < locList_.size());
		loc = locList_.at(pos_);
		return true;
	}

	/**
	 * Retrieves the previous descriptor in the list, updating the current
	 * position.
	 * @param  loc  A descriptor to fill
	 * @return true if successful, false if already at the beginning of the list
	 */
	bool prev(Core::Location& loc) {
		if (pos_ <= 0)
			return false;

		// Update the current position.
		--pos_;
		--insertItr_;

		// Get the new current descriptor.
		Q_ASSERT(pos_ >= 0 && pos_ < locList_.size());
		loc = locList_.at(pos_);
		return true;
	}

private:
	/**
	 * The history is kept as a list of locations.
	 */
	QList<Core::Location> locList_;

	/**
	 * The current position in the history.
	 */
	int pos_;

	/**
	 * An iterator pointing to the place where new entries should be added.
	 * This should always be one ahead of the current position.
	 */
	QList<Core::Location>::Iterator insertItr_;
};

} // namespace App

} // namespace KScope

#endif // __APP_LOCATIONHISTORY_H__
