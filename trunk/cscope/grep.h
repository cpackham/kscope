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

#ifndef __CSCOPE_GREP_H__
#define __CSCOPE_GREP_H__

#include <core/process.h>
#include <core/globals.h>
#include <core/engine.h>

namespace KScope
{

namespace Cscope
{

/**
 * Front-end to a Grep process.
 */
class Grep : public Core::Process, public Core::Engine::Controlled
{
public:
	Grep();
	~Grep();

	void query(Core::Engine::Connection*, const QString&, const QString&);

	/**
	 * Stops a running process.
	 */
	virtual void stop() { kill(); }

	static QString execPath_;

protected slots:
	virtual void handleFinished(int, QProcess::ExitStatus);

private:
	/**
	 * The current connection object, used to communicate progress and result
	 * information.
	 */
	Core::Engine::Connection* conn_;

	/**
	 * List of locations.
	 * The list is constructed when result lines are parsed.
	 */
	Core::LocationList locList_;

	/**
	 * State for parsing output.
	 */
	State parseOutputState_;

	/**
	 * Functor for the initial-state transition-function.
	 */
	struct ParseAction
	{
		/**
		 * Struct constructor.
		 * @param  self  The owner Grep object
		 */
		ParseAction(Grep& self) : self_(self) {}

		/**
		 * Functor operator.
		 * Parses result lines.
		 * @param  capList  List of captured strings
		 */
		void operator()(const Parser::CapList& capList) const {
			// Fill-in a Location object, using the parsed result information.
			Core::Location loc;
			loc.file_ = capList[0].toString();
			loc.line_ = capList[1].toUInt();
			loc.text_ = capList[2].toString();
			loc.column_ = 0;

			// Add to the list of parsed locations.
			self_.locList_.append(loc);
		}
		/**
		 * The owner Grep object.
		 */
		Grep& self_;
	};

	/**
	 * Functor for the initial-state transition-function.
	 */
	struct ParseAttributeAction
	{
		/**
		 * Struct constructor.
		 * @param  self  The owner Grep object
		 */
		ParseAttributeAction(Grep& self) : self_(self) {}

		/**
		 * The owner Grep object.
		 */
		Grep& self_;
	};
};

} // namespace Cscope

} // namespace KScope

#endif // __CSCOPE_GREP_H__
