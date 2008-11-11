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

#ifndef __CSCOPE_CROSSREF_H
#define __CSCOPE_CROSSREF_H

#include "engine.h"
#include "cscope.h"

namespace KScope
{

namespace Cscope
{

/**
 * Manages a Cscope cross-reference database.
 * @author Elad Lahav
 */
class Crossref : public Core::Engine
{
	Q_OBJECT

public:
	Crossref(QObject* parent = 0);
	~Crossref();

public slots:
	bool open(const QString&);
	bool query(Core::Engine::Connection&, const Core::Query&) const;
	bool build(Core::Engine::Connection&) const;

	const QString& path() { return path_; }

private:
	/**
	 * The path of the directory containing the cscope.out file.
	 */
	QString path_;
};

}

}

#endif // __CSCOPE_CROSSREF_H
