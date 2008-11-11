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

#include <QDir>
#include <QFileInfo>
#include "crossref.h"
#include "exception.h"

namespace KScope
{

namespace Cscope
{

Crossref::Crossref(QObject* parent) : Core::Engine(parent)
{
}

Crossref::~Crossref()
{
}

/**
 * Opens a cscope cross-reference database.
 * The method expects to find a 'cscope.out' file under the given path.
 * The path is stored and used for all database operations.
 * @param  path  The path under which to find the cross-reference file
 * @return true if successful, false otherwise
 */
bool Crossref::open(const QString& path)
{
	QDir dir(path);
	if (!dir.exists())
		throw new Core::Exception("Database directory does not exist");

	QFileInfo fi(dir, "cscope.out");

	if (!fi.exists() || !fi.isReadable())
		throw new Core::Exception("Cannot read the 'cscope.out' file");

	path_ = path;
	return true;
}

bool Crossref::query(Core::Engine::Connection& conn,
                     const Core::Query& query) const
{
	Cscope* cscope;
	Cscope::QueryType type;

	// Translate the requested type into a Cscope query number.
	switch (query.type_) {
	case Core::Query::Text:
		if (query.flags_ & Core::Query::RegExp)
			type = Cscope::EGrepPattern;
		else
			type = Cscope::Text;
		break;

	case Core::Query::References:
		type = Cscope::References;
		break;

	case Core::Query::Definition:
		type = Cscope::Definition;
		break;

	case Core::Query::CalledFunctions:
		type = Cscope::CalledFunctions;
		break;

	case Core::Query::CallingFunctions:
		type = Cscope::CallingFunctions;
		break;

	case Core::Query::FindFile:
		type = Cscope::FindFile;
		break;

	case Core::Query::IncludingFiles:
		type = Cscope::IncludingFiles;
		break;

	default:
		// Query type is not supported.
		return false;
	}

	cscope = new Cscope(conn);
	cscope->setDeleteOnExit();
	cscope->query(path_, type, query.pattern_);
	return true;
}

bool Crossref::build(Core::Engine::Connection& conn) const
{
	Cscope* cscope;

	cscope = new Cscope(conn);
	cscope->setDeleteOnExit();
	cscope->build(path_);
	return true;
}

}

}
