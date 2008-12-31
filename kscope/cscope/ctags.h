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

#ifndef __CSCOPE_CTAGS_H__
#define __CSCOPE_CTAGS_H__

#include "process.h"
#include "globals.h"
#include "engine.h"

namespace KScope
{

namespace Cscope
{

/**
 * Front-end to a Ctags process.
 * Since Cscope does not provide a query type for local tags, we use a Ctags
 * process to provide this information. Thus, whenever a query of this type is
 * selected for execution by the Cscope::Crossref engine, the engine creates a
 * Ctags object instead of a Cscope one.
 * @author Elad Lahav
 */
class Ctags : public Core::Process, public Core::Engine::Controlled
{
public:
	Ctags();
	~Ctags();

	void query(Core::Engine::Connection*, const QString&);

	/**
	 * Stops a running process.
	 */
	virtual void stop() { kill(); }

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
	 * Functor for the initial-state transition-function.
	 */
	struct ParseAction
	{
		/**
		 * Struct constructor.
		 * @param  self  The owner Ctags object
		 */
		ParseAction(Ctags& self) : self_(self) {}

		/**
		 * Functor operator.
		 * Parses result lines.
		 * @param  capList  List of captured strings
		 */
		void operator()(const Parser::CapList& capList) const {
			// Fill-in a Location object, using the parsed result information.
			Core::Location loc;
			loc.tag_.name_ = capList[0].toString();
			loc.file_ = capList[1].toString();
			loc.line_ = capList[2].toUInt();

			// Translate a Ctags type character into a tag type value.
			switch (capList[3].toString().at(0).toLatin1()) {
			case 'v':
				loc.tag_.type_ = Core::Tag::Variable;
				break;

			case 'f':
				loc.tag_.type_ = Core::Tag::Function;
				break;

			case 's':
				loc.tag_.type_ = Core::Tag::Struct;
				break;

			case 'u':
				loc.tag_.type_ = Core::Tag::Union;
				break;

			case 'm':
				loc.tag_.type_ = Core::Tag::Member;
				break;

			case 'g':
				loc.tag_.type_ = Core::Tag::Enum;
				break;

			case 'e':
				loc.tag_.type_ = Core::Tag::Enumerator;
				break;

			case 'd':
				loc.tag_.type_ = Core::Tag::Define;
				break;

			default:
				loc.tag_.type_ = Core::Tag::UnknownTag;
			}

			int i;
			for (i = 4; i < capList.size(); i += 2) {
				if (capList[i].toString() == "struct")
					loc.tag_.scope_ = capList[i + 1].toString();
			}

			// Add to the list of parsed locations.
			self_.locList_.append(loc);
		}
		/**
		 * The owner Ctags object.
		 */
		Ctags& self_;
	};
};

} // namespace Cscope

} // namespace KScope

#endif // __CSCOPE_CTAGS_H__
