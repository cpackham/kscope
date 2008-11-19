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

#include "querydialog.h"

namespace KScope
{

namespace App
{

/**
 * Class constructor.
 * @param  type    Default query type
 * @param  parent  Parent widget
 */
QueryDialog::QueryDialog(Core::Query::Type type, QWidget* parent)
	: QDialog(parent), Ui::QueryDialog()
{
	// Generate the GUI.
	setupUi(this);

	// Prepare the query type combo box.
	typeCombo_->addItem(tr("Text"), Core::Query::Text);
	typeCombo_->addItem(tr("References"), Core::Query::References);
	typeCombo_->addItem(tr("Definition"), Core::Query::Definition);
	typeCombo_->addItem(tr("Called Functions"),
	                    Core::Query::CalledFunctions);
	typeCombo_->addItem(tr("Calling Functions"),
						Core::Query::CallingFunctions);
	typeCombo_->addItem(tr("File"), Core::Query::FindFile);
	typeCombo_->addItem(tr("Including Files"), Core::Query::IncludingFiles);

	typeCombo_->setCurrentIndex(typeCombo_->findData(type));
}

/**
 * Class constructor.
 * Used for prompting for a symbol when generating a call-graph/tree.
 * @param  parent  Parent widget
 */
QueryDialog::QueryDialog(QWidget* parent) : QDialog(parent), Ui::QueryDialog()
{
	// Generate the GUI.
	setupUi(this);

	// Prepare the query type combo box.
	typeCombo_->addItem(tr("Call Tree"));
	typeCombo_->addItem(tr("Calling Tree"));
	typeCombo_->addItem(tr("Call Graph"));
}

/**
 * Class destructor.
 */
QueryDialog::~QueryDialog()
{
}

/**
 * @return The text in the pattern editor
 */
QString QueryDialog::pattern()
{
	return patternEdit_->text();
}

/**
 * @param  pattern  Initial text for the pattern editor
 */
void QueryDialog::setPattern(const QString& pattern)
{
	patternEdit_->setText(pattern);
	patternEdit_->selectAll();
}

/**
 * @return The selected query type.
 */
Core::Query::Type QueryDialog::type()
{
	int index;
	QVariant data;

	index = typeCombo_->currentIndex();
	if (index == -1)
		return Core::Query::References;

	data = typeCombo_->itemData(index);
	return static_cast<Core::Query::Type>(data.toUInt());
}

} // namespace App

} // namespace KScope
