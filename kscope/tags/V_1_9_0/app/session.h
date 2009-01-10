/***************************************************************************
 *   Copyright (C) 2007-2009 by Elad Lahav
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

#ifndef __APP_SESSION_H__
#define __APP_SESSION_H__

#include "globals.h"

namespace KScope
{

namespace App
{

class Session
{
public:
	Session(const QString&);
	~Session();

	void load();
	void save();

	const Core::LocationList& editorList() const { return editorList_; }
	const QString& activeEditor() const { return activeEditor_; }

	void setEditorList(const Core::LocationList& list) { editorList_ = list; }
	void setActiveEditor(const QString& editor) { activeEditor_ = editor; }

private:
	/**
	 * The path of the configuration file holding session information.
	 */
	QString path_;
	Core::LocationList editorList_;
	QString activeEditor_;
};

} // namespace App

} // namespace KScope

#endif // __APP_SESSION_H__
