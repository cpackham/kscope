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

#include "findtextdialog.h"

namespace KScope
{

namespace App
{

FindTextDialog::FindTextDialog(QWidget* parent)
	: QDialog(parent), Ui::FindTextDialog()
{
	setupUi(this);
}

FindTextDialog::~FindTextDialog()
{
}

void FindTextDialog::getParams(Editor::FindParams& params)
{
	params.pattern_ = patternCombo_->lineEdit()->text();
	params.regExp_ = regExpCheck_->isChecked();
	params.caseSensitive_ = caseSensitiveCheck_->isChecked();;
	params.wholeWordsOnly_ = wholeWordsCheck_->isChecked();
	params.wrap_ = wrapCheck_->isChecked();
	params.forward_ = !backwardsCheck_->isChecked();
}

} // namespace App

} // namespace KScope
