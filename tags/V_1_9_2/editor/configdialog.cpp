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
#include <QColorDialog>
#include "configdialog.h"
#include "lexerstylemodel.h"

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
	globalFontLabel_->setText(config.font_.family());
	globalFontLabel_->setFont(config.font_);
	hlCurLineCheck_->setChecked(config.hlCurLine_);
	indentTabsCheck_->setChecked(config.indentTabs_);
	tabWidthSpin_->setValue(config.tabWidth_);

	// Create a model for the style tree view.
	LexerStyleModel* styleModel = new LexerStyleModel(this);
	styleView_->setModel(styleModel);
	connect(globalFontCheck_, SIGNAL(toggled(bool)), styleModel,
	        SLOT(useGlobalFont(bool)));
	connect(this, SIGNAL(globalFontChanged()), styleModel,
	        SLOT(onGlobalFontChange()));
	connect(resetButton_, SIGNAL(clicked()), styleModel,
	        SLOT(resetStyles()));

	// Populate language combo-boxes.
	lexerModel_ = new QStandardItemModel(this);
	int row = 0;
	foreach (QsciLexer* lexer, config.lexers_) {
		QStandardItem* item = new QStandardItem(lexer->language());
		item->setData(QVariant::fromValue((void*)lexer));
		lexerModel_->setItem(row++, 0, item);
	}

	indentLanguageCombo_->setModel(lexerModel_);
	styleLanguageCombo_->setModel(lexerModel_);
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
	config.font_ = globalFontLabel_->font();
	config.hlCurLine_ = hlCurLineCheck_->isChecked();
	config.indentTabs_ = indentTabsCheck_->isChecked();
	config.tabWidth_ = tabWidthSpin_->value();
}

/**
 * Called when the user clicks the "..." button by the global font label.
 * Prompts the user for a new global font.
 */
void ConfigDialog::changeGlobalFont()
{
	// Prompt the user for a new font.
	bool ok;
	QFont font = QFontDialog::getFont(&ok, globalFontLabel_->font(), this);
	if (!ok)
		return;

	// Set the font label information.
	globalFontLabel_->setText(font.family());
	globalFontLabel_->setFont(font);

	// Apply the new global font to all lexers.
	QStandardItem* item;
	for (int i = 0; (item = lexerModel_->item(i)) != NULL; i++) {
		QsciLexer* lexer = static_cast<QsciLexer*>(item->data().value<void*>());
		dynamic_cast<LexerExInterface*>(lexer)->setGlobalFont(font);
	}

	// Notify the style model of the change.
	emit globalFontChanged();
}

/**
 * Called when the user activates an item in the style view.
 * For a property item, the appropriate editing dialogue is displayed, which
 * allows the user to change the property's value.
 * @param  index The index of the activated item
 */
void ConfigDialog::editStyle(const QModelIndex& index)
{
	// Get the edit role data for this item.
	// Only property items will return a valid value.
	LexerStyleModel* styleModel
		= static_cast<LexerStyleModel*>(styleView_->model());
	QVariant editData = styleModel->data(index, Qt::EditRole);

	// Display an editor based on the value's type.
	switch (editData.type()) {
	case QVariant::Font:
		{
			bool ok;
			QFont font = QFontDialog::getFont(&ok, editData.value<QFont>(),
			                                  this);
			if (ok)
				styleModel->setData(index, font);
		}
		break;

	case QVariant::Color:
		{
			QColor color = QColorDialog::getColor(editData.value<QColor>(),
			                                      this);
			if (color.isValid())
				styleModel->setData(index, color);
		}
		break;

	default:
		;
	}
}

void ConfigDialog::resetStyles()
{
	globalFontCheck_->setChecked(false);
}

void ConfigDialog::indentLanguageChanged(int id)
{
	// TODO: Implement.
	(void)id;
}

/**
 * Called when a different item is selected in the styles language combo-box.
 * Updates the style view to reflect the settings for the newly selected
 * language.
 * @param  index The selected combo-box item
 */
void ConfigDialog::styleLanguageChanged(int index)
{
	// Get the current item from the language model.
	QStandardItem* item = lexerModel_->item(index);
	if (item == NULL)
		return;

	// Get the lexer stored in the item.
	QsciLexer* lexer = static_cast<QsciLexer*>(item->data().value<void*>());

	// Apply the lexer to the style model.
	LexerStyleModel* styleModel
		= static_cast<LexerStyleModel*>(styleView_->model());
	styleModel->setLexer(lexer);

	// Update the global font check box.
	globalFontCheck_->setChecked(dynamic_cast<LexerExInterface*>(lexer)
	                             ->useGlobalFont());
}

} // namespace Editor

} // namespace KScope
