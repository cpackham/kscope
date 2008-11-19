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

#ifndef __CSCOPE_CSCOPE_H
#define __CSCOPE_CSCOPE_H

#include "process.h"
#include "globals.h"
#include "engine.h"

namespace KScope
{

namespace Cscope
{

/**
 * Front-end to a Cscope process.
 * This object can be used for both querying and building the Cscope
 * cross-reference file.
 * @author Elad Lahav
 */
class Cscope : public Core::Process, public Core::Engine::Controlled
{
public:
	Cscope(const QStringList&);
	~Cscope();

	enum QueryType {
		References = 0,
		Definition = 1,
		CalledFunctions = 2,
		CallingFunctions = 3,
		Text = 4,
		EGrepPattern = 6,
		FindFile = 7,
		IncludingFiles = 8
	};

	void query(Core::Engine::Connection*, const QString&, QueryType,
	           const QString&);
	void build(Core::Engine::Connection*, const QString&);
	virtual void stop() { kill(); }

signals:
	void dataReady(const Core::Location& loc);
	void finished();
	void progress(const QString&, uint cur, uint total);

protected slots:
	virtual void handleFinished(int, QProcess::ExitStatus);

private:
	/**
	 * Command-line arguments common to both querying and building the database.
	 */
	QStringList baseArgs_;

	/**
	 * The current connection object, used to communicate progress and result
	 * information.
	 */
	Core::Engine::Connection* conn_;
	uint resNum_;
	uint resParsed_;
	State buildInitState_;
	State buildProgState_;
	State queryInitState_;
	State queryProgState_;
	State queryResultState_;
	Core::LocationList locList_;

	struct ProgAction
	{
		ProgAction(Cscope& self, const QString& text) : self_(self),
			text_(text) {}

		void operator()(const Parser::CapList& capList) const {
			self_.conn_->onProgress(text_, capList[0].toUInt(),
			                        capList[1].toUInt());
		}

		Cscope& self_;
		QString text_;
	};

	struct QueryEndAction
	{
		QueryEndAction(Cscope& self) : self_(self) {}

		void operator()(const Parser::CapList& capList) const {
			self_.resNum_ = capList[0].toUInt();
			self_.resParsed_ = 0;
			self_.conn_->onProgress(tr("Parsing..."), 0, self_.resNum_);
		}

		Cscope& self_;
	};

	struct QueryResultAction
	{
		QueryResultAction(Cscope& self) : self_(self) {}

		void operator()(const Parser::CapList& capList) const {
			Core::Location result;

			result.file_ = capList[0].toString();
			result.line_ = capList[2].toUInt();
			result.column_ = 0;
			result.tag_ = 0;
			result.scope_ = capList[1].toString();
			result.text_ = capList[3].toString();

			self_.locList_.append(result);

			self_.resParsed_++;
			if ((self_.resParsed_ & 0xff) == 0)
				self_.conn_->onProgress(tr("Parsing..."), self_.resParsed_,
				                        self_.resNum_);
		}

		Cscope& self_;
	};
};

}

}

#endif // __CSCOPE_CSCOPE_H
