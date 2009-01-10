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

#include <QDebug>
#include "session.h"
#include "projectmanager.h"
#include "exception.h"

namespace KScope
{

namespace App
{

/**
 * Class constructor.
 * @param  path  Path to the session file
 */
Session::Session(const QString& path) : path_(path)
{
}

/**
 * Class destructor.
 */
Session::~Session()
{
}

/**
 * Reads session information from the configuration file.
 */
void Session::load()
{
	QSettings settings(path_, QSettings::IniFormat);

	// Get a list of files being edited, along with the location of the
	// cursor on each one.
	int size = settings.beginReadArray("Editors");
	for (int i = 0; i < size; i++) {
		settings.setArrayIndex(i);

		Core::Location loc;
		loc.file_ = settings.value("Path").toString();
		loc.line_ = settings.value("Line").toUInt();
		loc.column_ = settings.value("Column").toUInt();

		editorList_.append(loc);
	}
	settings.endArray();

	// Get the path of the active editor.
	activeEditor_ = settings.value("ActiveEditor").toString();
}

/**
 * Writes session information to the configuration file.
 */
void Session::save()
{
	QSettings settings(path_, QSettings::IniFormat);

	// Get a list of files being edited, along with the location of the
	// cursor on each one.
	settings.beginWriteArray("Editors");
	for (int i = 0; i < editorList_.size(); i++) {
		settings.setArrayIndex(i);

		const Core::Location& loc = editorList_.at(i);
		settings.setValue("Path", loc.file_);
		settings.setValue("Line", loc.line_);
		settings.setValue("Column", loc.column_);
	}
	settings.endArray();

	// Get the path of the active editor.
	settings.setValue("ActiveEditor", activeEditor_);
}

} // namespace App

} // namespace KScope
