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

#ifndef __CSCOPE_FILES_H__
#define __CSCOPE_FILES_H__

#include "codebase.h"

namespace KScope
{

namespace Cscope
{

/**
 * Manages a cscope.files file.
 * A cscope.files file is a text file containing the name of each file in the
 * project on a separate line.
 * @author Elad Lahav
 */
class Files : public Core::Codebase
{
	Q_OBJECT

public:
	Files(QObject* parent = NULL);
	~Files();

	void create(const QString&);

	// Core::Codebase implementation.
	virtual void load(const QString&);
	virtual void store(const QString&);
	virtual void getFiles(Callback&) const;
	virtual void setFiles(const QStringList&);
	virtual bool canModify() { return writable_; }

private:
	/**
	 * The path to the cscope.files file.
	 */
	QString path_;

	/**
	 * Whether the file can be written to.
	 */
	bool writable_;
};

} // namespace Cscope

} // namespace KScope

#endif  // __CSCOPE_FILES_H__
