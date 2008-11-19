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

#ifndef __APP_FINDTEXTDIALOG_H__
#define __APP_FINDTEXTDIALOG_H__

#include <QDialog>
#include <QLineEdit>
#include "ui_findtextdialog.h"
#include "editor.h"

namespace KScope
{

namespace App
{

/**
 * A simple dialogue for searching text within an editor window.
 * @author  Elad Lahav
 */
class FindTextDialog : public QDialog, public Ui::FindTextDialog
{
	Q_OBJECT

public:
	FindTextDialog(const QString&, QWidget* parent = 0);
	~FindTextDialog();

	void getParams(Editor::FindParams&);
};

} // namespace App

} // namespace KScope

#endif // __APP_FINDTEXTDIALOG_H__
