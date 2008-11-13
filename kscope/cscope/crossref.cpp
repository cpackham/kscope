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

/**
 * Class constructor.
 * @param  parent  Parent object
 */
Crossref::Crossref(QObject* parent) : Core::Engine(parent)
{
}

/**
 * Class destructor.
 */
Crossref::~Crossref()
{
}

/**
 * Opens a cscope cross-reference database.
 * The method expects to find a 'cscope.out' file under the given path.
 * The path is stored and used for all database operations.
 * The initialisation string should be colon-delimited, where the first section
 * is the project path (includes the cscope.out and cscope.files files),
 * followed by command-line arguments to Cscope (only the ones that apply to
 * both querying and building).
 * @param  initString  The initialisation string
 * @throw  Exception
 */
void Crossref::open(const QString& initString)
{
	QStringList args = initString.split(":", QString::SkipEmptyParts);
	QString path = args.takeFirst();

	qDebug() << __func__ << initString << path;

	QDir dir(path);
	if (!dir.exists())
		throw new Core::Exception("Database directory does not exist");

	QFileInfo fi(dir, "cscope.out");

	if (!fi.exists() || !fi.isReadable())
		throw new Core::Exception("Cannot read the 'cscope.out' file");

	path_ = path;
	args_ = args;
}

/**
 * Starts a Cscope query.
 * Creates a new Cscope process to handle the query.
 * @param  conn  Connection object to attach to the new process
 * @param  query Query information
 * @throw  Exception
 */
void Crossref::query(Core::Engine::Connection& conn,
                     const Core::Query& query) const
{
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
		// TODO: What happens if an exception is thrown from within a slot?
		throw new Core::Exception(QString("Unsupported query type '%1")
		                          .arg(query.type_));
	}

	// Create a new Cscope process object, and start the query.
	Cscope* cscope = new Cscope(args_);
	cscope->setDeleteOnExit();
	cscope->query(&conn, path_, type, query.pattern_);
}

/**
 * Starts a Cscope build process.
 * @param  conn  Connection object to attach to the new process
 */
void Crossref::build(Core::Engine::Connection& conn) const
{
	Cscope* cscope = new Cscope(args_);
	cscope->setDeleteOnExit();
	cscope->build(&conn, path_);
}

}

}
