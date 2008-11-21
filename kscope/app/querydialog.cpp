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

#include <QMessageBox>
#include "querydialog.h"
#include "strings.h"

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
	// Create a list with all supported query types.
	TypeList typeList;
	typeList << Core::Query::Text << Core::Query::References
	         << Core::Query::Definition << Core::Query::CalledFunctions
	         << Core::Query::CallingFunctions << Core::Query::FindFile
	         << Core::Query::IncludingFiles;

	setupUi(typeList, type);
}

/**
 * Class constructor.
 * @param  typeList  The types to show
 * @param  type      Default query type
 * @param  parent    Parent widget
 */
QueryDialog::QueryDialog(const TypeList& typeList, Core::Query::Type type,
                         QWidget* parent)
	: QDialog(parent), Ui::QueryDialog()
{
	setupUi(typeList, type);
}

/**
 * Class destructor.
 */
QueryDialog::~QueryDialog()
{
}

/**
 * Called when the user clicks the "OK" button.
 * Removes all white space from before and after the entered text.
 */
void QueryDialog::accept()
{
	QString text = patternEdit_->text().trimmed();
	if (text.isEmpty()) {
		QMessageBox::warning(this, tr("Invalid Pattern"),
		                     tr("Please enter a non-empty pattern"));
		return;
	}

	patternEdit_->setText(text);
	QDialog::accept();
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

/**
 * Common method for both constructors.
 * Creates the dialogue's user interface.
 * @param  typeList  The types to show
 * @param  type      Default query type
 */
void QueryDialog::setupUi(const TypeList& typeList, Core::Query::Type defType)
{
	// Generate the GUI.
	Ui::QueryDialog::setupUi(this);

	// Prepare the query type combo box.
	Core::Query::Type type;
	foreach (type, typeList)
		typeCombo_->addItem(Strings::toString(type), type);

	// Select the default type.
	if (typeList.size() > 1)
		typeCombo_->setCurrentIndex(typeCombo_->findData(defType));
}

} // namespace App

} // namespace KScope
