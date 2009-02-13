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

#ifndef __CORE_GLOBALS_H__
#define __CORE_GLOBALS_H__

#include <QString>
#include <QIcon>

namespace KScope
{

namespace Core
{

/**
 * The Tag structure provides information for a defined symbol in the code base.
 */
struct Tag
{
	enum Type
	{
		/** Default tag. */
		UnknownTag,
		/** Variable declaration. */
		Variable,
		/** Function declaration/definition. */
		Function,
		/** Structure. */
		Struct,
		/** Union. */
		Union,
		/** Structure/union field. */
		Member,
		/** Enumeration declaration. */
		Enum,
		/** Member in an enumeration. */
		Enumerator,
		/** Type definition. */
		Typedef,
		/** Preprocessor definition. */
		Define,
		/** Preprocessor #include directive. */
		Include,
		/** Goto label. */
		Label
	};

	/**
	 * The name of the tag.
	 */
	QString name_;

	/**
	 * The tag's type, determined by its definition.
	 */
	Type type_;

	/**
	 * Some tags are defined within a scope (e.g., a structure member).
	 * An empty string is used for the "Global" scope.
	 */
	QString scope_;

	/**
	 * Generates an icon for a given tag type.
	 * TODO This should probably be moved somewhere else (like a Strings class)
	 * @param  type  The type for which a name is requested
	 * @return The matching name
	 */
	static QString name(Type type) {
		switch (type) {
		case Variable:
			return "Variable";

		case Function:
			return "Function";

		case Struct:
			return "Struct";

		case Union:
			return "Union";

		case Member:
			return "Struct/Union Member";

		case Enum:
			return "Enumeration";

		case Enumerator:
			return "Enumeration Value";

		case Typedef:
			return "Type Definition";

		case Define:
			return "Preprorcessor Definition";

		case Include:
			return "#include Directive";

		case Label:
			return "Go-to Label";

		default:
			;
		}

		return QString();
	}

	/**
	 * Generates an icon for a given tag type.
	 * TODO This should probably be moved somewhere else (like an Images class)
	 * @param  type  The type for which an icon is requested
	 * @return The matching icon
	 */
	static QIcon icon(Type type) {
		switch (type) {
		case Variable:
			return QIcon(":/images/variable");

		case Function:
			return QIcon(":/images/function");

		case Struct:
			return QIcon(":/images/struct");

		case Union:
			return QIcon(":/images/union");

		case Member:
			return QIcon(":/images/member");

		case Enum:
			return QIcon(":/images/enum");

		case Enumerator:
			return QIcon(":/images/enumerator");

		case Typedef:
			return QIcon(":/images/typedef");

		case Define:
			return QIcon(":/images/define");

		case Include:
			return QIcon(":/images/include");

		case Label:
			return QIcon(":/images/label");

		default:
			;
		}

		return QIcon();
	}
};

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
	/**
	 * File path.
	 */
	QString file_;

	/**
	 * Line number.
	 */
	uint line_;

	/**
	 * Column number.
	 */
	uint column_;

	/**
	 * Tag information (optional).
	 */
	Tag tag_;

	/**
	 * Line text.
	 */
	QString text_;

	/**
	 * Each member in the structure is assigned a numeric value. These can be
	 * used for, e.g., creating lists of fields for displaying query results.
	 */
	enum Fields
	{
		/** File path. */
		File,
		/** Line number. */
		Line,
		/** Column number. */
		Column,
		/** Symbol name. */
		TagName,
		/** Tag type (function, variable, structure, etc.) */
		TagType,
		/** Scope of the tag (function name, structure, global, etc.) */
		Scope,
		/** Line text. */
		Text
	};

	/**
	 * Default constructor.
	 * Creates an empty (invalid) location object.
	 */
	Location() : line_(0), column_(0) {}

	/**
	 * Convenience constructor.
	 * @param  file
	 * @param  line
	 * @param  column
	 */
	Location(const QString& file, uint line = 0, uint column = 0)
		: file_(file), line_(line), column_(column) {}

	/**
	 * @return true if the object represents a valid location (at least the
	 *         file path is set), false otherwise
	 */
	bool isValid() const { return !file_.isEmpty(); }

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

/**
 * A list of locations.
 * This is useful for passing query results around.
 */
typedef QList<Location> LocationList;

/**
 * Defines parameters for running queries on an engine.
 */
struct Query
{
	/**
	 * Possible queries.
	 */
	enum Type {
		/** Default type. */
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

	/**
	 * The query type.
	 */
	Type type_;

	/**
	 * Determines certain aspects of the query.
	 */
	enum Flags {
		/**
		 * Make the search case-insensitive.
		 */
		IgnoreCase = 0x1,
		/**
		 * The pattern is a regular expression.
		 */
		RegExp = 0x2
	};

	/**
	 * The pattern to search for.
	 */
	QString pattern_;

	/**
	 * Modifiers: A bitmask of Flags.
	 */
	uint flags_;

	/**
	 * Default constructor.
	 * Creates an invalid query object.
	 */
	Query() : type_(Invalid) {}

	/**
	 * Struct constructor.
	 * @param  type     The type of query
	 * @param  pattern  The pattern to look for
	 * @param  flags    Modifiers
	 */
	Query(Type type, const QString& pattern,
	      uint flags = 0)
		: type_(type), pattern_(pattern), flags_(flags) {}
};

/**
 * A generic associative set, indexed by strings.
 */
typedef QMap<QString, QVariant> KeyValuePairs;

/**
 * Used as a default type.
 */
struct Void {};

/**
 * Defines a generic callback functor, with a type T parameter.
 */
template<class T = Void>
struct Callback
{
	virtual void call(T t) = 0;
};

/**
 * Defines a generic callback functor, without parameters.
 */
template<>
struct Callback<Void>
{
	virtual void call() = 0;
};

} // namespace Core

} // namespace KScope

#endif // __CORE_GLOBALS_H__
