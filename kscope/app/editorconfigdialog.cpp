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

#include "editorconfigdialog.h"

namespace KScope
{

namespace App
{

EditorConfigDialog::EditorConfigDialog(const Editor::Config& config,
                                       QWidget* parent)
	: QDialog(parent), Ui::EditorConfigDialog()
{
	setupUi(this);

	// Update the controls to reflect the given configuration.
	fontFamilyCombo_->setCurrentFont(config.font_);
	fontSizeSpin_->setValue(config.font_.pointSize());
	fontBoldCheck_->setChecked(config.font_.bold());
	fontItalicCheck_->setChecked(config.font_.italic());
	hlCurLineCheck_->setChecked(config.hlCurLine_);
	indentTabsCheck_->setChecked(config.indentTabs_);
	tabWidthSpin_->setValue(config.tabWidth_);
}

EditorConfigDialog::~EditorConfigDialog()
{
}

void EditorConfigDialog::getConfig(Editor::Config& config)
{
	config.font_ = QFont(fontFamilyCombo_->currentFont().family(),
	                     fontSizeSpin_->value(),
	                     fontBoldCheck_->isChecked() ? QFont::Bold
	                    		                     : QFont::Normal,
	                     fontItalicCheck_->isChecked());
	config.hlCurLine_ = hlCurLineCheck_->isChecked();
	config.indentTabs_ = indentTabsCheck_->isChecked();
	config.tabWidth_ = tabWidthSpin_->value();
}

}

}
