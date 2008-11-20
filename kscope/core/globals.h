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

#ifndef __CORE_GLOBALS_H
#define __CORE_GLOBALS_H

#include <QString>

namespace KScope
{

namespace Core
{

/**
 * A location in the code base.
 * Locations are at the heart of the browsing system offered by KScope.
 * Query results, navigation history and bookmarks are all expressed as
 * lists of Location objects.
 * A location refers to a line and column in a file. Optionally, it may
 * reference a specific tag, in which case its type and scope can also be
 * given.
 */
struct Location
{
	/** File path. */
	QString file_;
	/** Line number. */
	uint line_;
	/** Column number. */
	uint column_;
	/** Tag type. */
	uint tag_;
	/** Scope of the tag (function name, structure, global, etc.) */
	QString scope_;
	/** Line text. */
	QString text_;

	/**
	 * Two locations are equal if and only if they refer to the same line and
	 * column in the same file.
	 * @param  other  The location to compare with
	 * @return true if the locations are equal, false otherwise
	 */
	bool operator==(const Location& other) {
		return ((file_ == other.file_)
				&& (line_ == other.line_)
				&& (column_ == other.column_));
	}
};

typedef QList<Location> LocationList;

struct Query
{
	/**
	 * Possible queries.
	 */
	enum Type {
		Invalid,
		/** Free text search. */
		Text,
		/** Symbol definition */
		Definition,
		/** All references to a symbol */
		References,
		/** Functions called by the given function name */
		CalledFunctions,
		/** Functions calling the given function name */
		CallingFunctions,
		/** Search for a file name */
		FindFile,
		/** Search for files including a given file name */
		IncludingFiles,
		/** List all tags in the given file */
		LocalTags
	};

	enum Flags {
		/** Make the search case-insensitive. */
		IgnoreCase = 0x1,
		/** The pattern is a regular expression. */
		RegExp = 0x2
	};

	/** The query type. */
	Type type_;

	/** The pattern to search for. */
	QString pattern_;

	/** Modifiers: A bitmask of Flags. */
	uint flags_;

	/**
	 * Constructor.
	 */
	Query() : type_(Invalid) {}
	Query(Type type, const QString& pattern, uint flags = 0)
		: type_(type), pattern_(pattern), flags_(flags) {}
};

}

}

#endif // __CORE_GLOBALS_H
