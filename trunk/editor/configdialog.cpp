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

#include <QFontDialog>
#include "configdialog.h"
#include "lexerstyleview.h"

namespace KScope
{

namespace Editor
{

/**
 * Class constructor.
 * @param  config Holds the configuration parameters to display by default
 * @param  parent Parent widget
 */
ConfigDialog::ConfigDialog(const Config& config, QWidget* parent)
	: QDialog(parent), Ui::ConfigDialog()
{
	setupUi(this);

	// Update the controls to reflect the given configuration.
	fontLabel_->setText(config.font_.family());
	fontLabel_->setFont(config.font_);
	hlCurLineCheck_->setChecked(config.hlCurLine_);
	indentTabsCheck_->setChecked(config.indentTabs_);
	tabWidthSpin_->setValue(config.tabWidth_);

	// Add a configuration page for each lexer.
	LexerStyleView* view = new LexerStyleView(config.cppLexer_, this);
	connect(this, SIGNAL(defaultFontChanged(const QFont&)), view->model(),
	        SLOT(setDefaultFont(const QFont&)));
	tabs_->addTab(view, tr("Styles: %1").arg(config.cppLexer_->language()));
}

/**
 * Class destructor.
 */
ConfigDialog::~ConfigDialog()
{
}

/**
 * Copies the parameters in the dialogue to the given configuration object.
 * @param  config The configuration object
 */
void ConfigDialog::getConfig(Config& config)
{
	config.font_ = fontLabel_->font();
	config.hlCurLine_ = hlCurLineCheck_->isChecked();
	config.indentTabs_ = indentTabsCheck_->isChecked();
	config.tabWidth_ = tabWidthSpin_->value();
}

/**
 * Called when the user clicks the "..." button by the default font label.
 * Prompts the user for a new default font.
 */
void ConfigDialog::changeDefaultFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, fontLabel_->font(), this);
	if (ok) {
		fontLabel_->setText(font.family());
		fontLabel_->setFont(font);
		emit defaultFontChanged(font);
	}
}

} // namespace Editor

} // namespace KScope
