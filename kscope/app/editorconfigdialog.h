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

#ifndef __APP_EDITORCONFIGDIALOG_H__
#define __APP_EDITORCONFIGDIALOG_H__

#include <QDialog>
#include "ui_editorconfigdialog.h"
#include "editor.h"

namespace KScope
{

namespace App
{

/**
 * A dialogue for configuring a QScintilla editor.
 * Unfortunately, QScintilla does not provide such a dialogue
 * @author Elad Lahav
 */
class EditorConfigDialog : public QDialog, public Ui::EditorConfigDialog
{
	Q_OBJECT

public:
	EditorConfigDialog(const Editor::Config&, QWidget* parent = NULL);
	~EditorConfigDialog();

	void getConfig(Editor::Config&);
};

} // namespace App

} // namespace KScope

#endif // __APP_EDITORCONFIGDIALOG_H__
