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

#include "ctags.h"
#include "exception.h"

namespace KScope
{

namespace Cscope
{

/**
 * Class constructor.
 */
Ctags::Ctags() : Process(), conn_(NULL)
{
	addRule(initState_, Parser::String('\t')               // tag name
	                    << Parser::Whitespace()
	                    << Parser::String('\t')            // file name
	                    << Parser::Whitespace()
	                    << Parser::Number()                // line number
	                    << Parser::Literal(";\"\t")
                        << Parser::String('\t')            // tag type
                        << Parser::Whitespace()
	                    << *(Parser::String(':')           // attribute name
	                         << Parser::Literal(":")
	                         << Parser::String('\t', true) // attribute value
	                         << Parser::Whitespace()),
	        initState_, ParseAction(*this));
}

/**
 * Class destructor.
 */
Ctags::~Ctags()
{
}

void Ctags::query(Core::Engine::Connection* conn, const QString& file)
{
	// Abort if a process is already running.
	if (state() != QProcess::NotRunning || conn_ != NULL)
		throw Core::Exception("Process already running");

	// TODO: Make the Cscope path configurable.
	QString prog = "/usr/bin/ctags";

	// Prepare the argument list.
	QStringList args;
	args << "-n"          // use line numbers instead of patterns
         << "--fields=+s" // add scope information
         << "--sort=no"   // do not sort by tag name
         << "-f" << "-"   // output to stdout instead of a file
         << file;

	// Initialise parsing.
	conn_ = conn;
	conn_->setCtrlObject(this);
	locList_.clear();

	// Start the process.
	qDebug() << "Running ctags:" << args;
	start(prog, args);
}

/**
 * Called when the process terminates.
 * @param  code    The exit code of the process
 * @param  status  Used to indicate process crashes
 */
void Ctags::handleFinished(int code, QProcess::ExitStatus status)
{
	Process::handleFinished(code, status);

	// Hand over data to the other side of the connection.
	if (!locList_.isEmpty())
		conn_->onDataReady(locList_);

	// Signal normal termination.
	conn_->onFinished();

	// Detach from the connection object.
	conn_->setCtrlObject(NULL);
	conn_ = NULL;
}

} // namespace Cscope

} // namespace KScope
