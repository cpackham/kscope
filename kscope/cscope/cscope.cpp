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

#include <QtDebug>
#include "cscope.h"

namespace KScope
{

namespace Cscope
{

Cscope::Cscope(Core::Engine::Connection& conn) : Process(), conn_(&conn),
	buildInitState_("buildInitState"),
	buildProgState_("buildProgState"),
	queryProgState_("queryProgState"),
	queryResultState_("queryResultState")
{
	addRule(buildInitState_, Parser::Literal("Building cross-reference..."),
	        buildProgState_);
	addRule(buildProgState_, Parser::Literal("> Building symbol database ")
	                         << Parser::Number()
	                         << Parser::Literal(" of ") << Parser::Number(),
	        buildProgState_, ProgAction(*this, tr("Building database...")));
	addRule(buildProgState_, Parser::Literal("> Symbols matched ")
	                         << Parser::Number()
	                         << Parser::Literal(" of ")
	                         << Parser::Number(),
	        queryProgState_, ProgAction(*this, tr("Querying...")));
	addRule(queryProgState_, Parser::Literal("> Possible references retrieved ")
	                         << Parser::Number()
	                         << Parser::Literal(" of ")
	                         << Parser::Number(),
		    queryProgState_, ProgAction(*this, tr("Querying...")));
	addRule(queryProgState_, Parser::Literal("> Search ")
	                         << Parser::Number()
	                         << Parser::Literal(" of ")
	                         << Parser::Number(),
	        queryProgState_, ProgAction(*this, tr("Querying...")));
	addRule(queryProgState_, Parser::Literal("cscope: ")
	                         << Parser::Number()
	                         << Parser::Literal(" lines"),
	        queryResultState_, QueryEndAction(*this));
	addRule(queryResultState_, Parser::String(' ')
	                           << Parser::Whitespace()
	                           << Parser::String(' ')
	  	                       << Parser::Whitespace()
	                           << Parser::Number()
	                           << Parser::String(),
	        queryResultState_, QueryResultAction(*this));

	conn_->setCtrlObject(this);
}

Cscope::~Cscope()
{
}

void Cscope::query(const QString& path, QueryType type,
                   const QString& pattern)
{
	QString prog = "/usr/bin/cscope";
	QStringList argList;

	// Prepare the argument list.
	prepareArgList(argList);
	argList << "-d";
	argList << QString("-L%1").arg(type);
	argList << pattern;
	setWorkingDirectory(path);

	// Prepare the object for accepting query results.
	setState(queryProgState_);
	locList_.clear();

#ifndef QT_NO_DEBUG
	qDebug() << "Running cscope:" << argList << "in" << path;
#endif

	start(prog, argList);
}

void Cscope::build(const QString& path)
{
	QString prog = "/usr/bin/cscope";
	QStringList argList;

	prepareArgList(argList);
	argList << "-b";

	setWorkingDirectory(path);
	setState(buildInitState_);

#ifndef QT_NO_DEBUG
	qDebug() << "Running cscope:" << argList << "in" << path;
#endif

	start(prog, argList);
}

void Cscope::handleFinished(int code, QProcess::ExitStatus status)
{
	Process::handleFinished(code, status);
	if (!locList_.isEmpty())
		conn_->onDataReady(locList_);

	conn_->onFinished();
}

void Cscope::prepareArgList(QStringList& argList)
{
	if (optKernel_)
		argList << "-k";

	if (optInvIndex_)
		argList << "-q";

	argList << "-v";
}

}

}
