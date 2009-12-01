/***************************************************************************
 *   Copyright (C) 2009 by Chris Packham
 *   cpackham@users.sourceforge.net
 *
 *   Based on ctags.cpp by Elad Lahav
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

#include <core/exception.h>
#include "grep.h"

namespace KScope
{

namespace Cscope
{

QString Grep::execPath_ = "/usr/bin/grep";

/**
 * Class constructor.
 */
Grep::Grep() : Process(), conn_(NULL)
{
	// Parse a line starting with the following format:
	// FILE_NAME:LINE_NUMBER:TEXT
	addRule(initState_, Parser::String<>(':') 
		<< Parser::Literal(":")
		<< Parser::Number()
		<< Parser::Literal(":")
		<< Parser::String<>('\n'),
	        parseOutputState_, ParseAction(*this));
	addRule(parseOutputState_, Parser::Literal("\n"),
	        initState_);

}

/**
 * Class destructor.
 */
Grep::~Grep()
{
}

void Grep::query(Core::Engine::Connection* conn, const QString& pattern, const QString& path)
{
	// Abort if a process is already running.
	if (state() != QProcess::NotRunning || conn_ != NULL)
		throw Core::Exception("Process already running");

	// Prepare the argument list.
	QStringList args;
	args << "-r"                            // Recursive
	     << "-n"                            // Print line numbers
	     << "--exclude-dir=.[ck]scope"      // Exclude in-tree .cscope/.kscope dirs
	     << "--binary-files=without-match"  // Ignore binary files
             << pattern
	     << path;

	// Initialise parsing.
	conn_ = conn;
	conn_->setCtrlObject(this);
	locList_.clear();

	// Start the process.
	qDebug() << "Running" << execPath_ << args;
	start(execPath_, args);
}

/**
 * Called when the process terminates.
 * @param  code    The exit code of the process
 * @param  status  Used to indicate process crashes
 */
void Grep::handleFinished(int code, QProcess::ExitStatus status)
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
