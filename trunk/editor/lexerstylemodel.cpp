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
#include "lexerstylemodel.h"

namespace KScope
{

namespace Editor
{

/**
 * Class constructor.
 * @param  lexer  The lexer to use
 * @param  parent Parent object
 */
LexerStyleModel::LexerStyleModel(QsciLexer* lexer, QObject* parent)
	: QAbstractItemModel(parent), lexer_(lexer), useDefaultFont_(false)
{
	// Determine the number of styles used by the lexer.
	for (styleNum_ = 0; !lexer_->description(styleNum_).isEmpty(); styleNum_++)
		;
}

/**
 * Class destructor.
 */
LexerStyleModel::~LexerStyleModel()
{
}

void LexerStyleModel::useDefaultFont(bool use)
{
	useDefaultFont_ = use;
	if (use) {
		lexer_->setFont(lexer_->defaultFont());
		reset();
	}
}

void LexerStyleModel::setDefaultFont(const QFont& font)
{
	lexer_->setDefaultFont(font);
	if (useDefaultFont_) {
		lexer_->setFont(font);
		reset();
	}
}

/**
 * Creates an index.
 * @param  row    The row of the index, relative to the parent
 * @param  column The column of the index
 * @param  parent The parent index.
 * @return The resulting index
 */
QModelIndex LexerStyleModel::index(int row, int column,
                                   const QModelIndex& parent) const
{
	return createIndex(row, column,
	                   parent.isValid() ? parent.row() : -1);
}

/**
 * Finds the parent for the given index.
 * The parent of the first-level (style) items is the root (invalid) index,
 * while property items have style indices as their parents.
 * @param  index The index for which the parent is needed
 * @return The parent index
 */
QModelIndex LexerStyleModel::parent(const QModelIndex& index) const
{
	if (index.internalId() == -1)
		return QModelIndex();

	return createIndex((int)index.internalId(), 0, -1);
}

/**
 * Determines the number of child indices for the given parent.
 * For the root index, this is the number of styles. For each style index, the
 * number is equal to the number of registered properties.
 * @param  parent The parent index
 * @return The number of children for the index
 */
int LexerStyleModel::rowCount(const QModelIndex& parent) const
{
	if (!parent.isValid())
		return styleNum_;

	if (parent.internalId() == -1)
		return _Last + 1;

	return 0;
}

/**
 * Determines the number of columns in children of the given index.
 * This number is always 2.
 * @param  (ignored)
 * @return The number of columns for children of the index
 */
int LexerStyleModel::columnCount(const QModelIndex& parent) const
{
	(void)parent;
	return 2;
}

/**
 * Provides the data to display/edit for a given index and role.
 * @param  index The index for which data is requested
 * @param  role  The requested role
 * @return The relevant data
 */
QVariant LexerStyleModel::data(const QModelIndex& index, int role) const
{
	// Nothing for the root index.
	if (!index.isValid())
		return QVariant();

	if (index.internalId() == -1) {
		// Handle data for styles.
		if ((index.column() == 0) && (role == Qt::DisplayRole))
			return lexer_->description(index.row());
		if (index.column() == 1)
			return styleData(index.row(), role);
	}
	else {
		// Handle data for properties.
		if ((index.column() == 0) && (role == Qt::DisplayRole))
			return propertyName(static_cast<StyleProperty>(index.row()));
		if (index.column() == 1)
			return propertyData((int)index.internalId(),
			                    static_cast<StyleProperty>(index.row()),
			                    role);
	}

	return QVariant();
}

bool LexerStyleModel::setData(const QModelIndex& index, const QVariant& value,
                              int role)
{
	// Only second-level (property) indices may be changed.
	if (!index.isValid() || (index.internalId() == -1))
		return false;

	// Only handle Qt::EditRole roles.
	if (role != Qt::EditRole)
		return false;

	// Modify the lexer's property.
	int style = (int)index.internalId();
	bool update = false;
	switch (static_cast<StyleProperty>(index.row())) {
	case Font:
		if (value.type() == QVariant::Font) {
			lexer_->setFont(value.value<QFont>(), style);
			update = true;
		}
		break;

	case Foreground:
		if (value.type() == QVariant::Color) {
			lexer_->setColor(value.value<QColor>(), style);
			update = true;
		}
		break;

	case Background:
		if (value.type() == QVariant::Color) {
			lexer_->setPaper(value.value<QColor>(), style);
			update = true;
		}
		break;
	}

	if (!update)
		return false;

	// Update changes.
	emit dataChanged(index, index);
	emit dataChanged(parent(index), parent(index));
	return true;
}

/**
 * Creates a string with the style's font and colours to be displayed for the
 * second column of a style item.
 * @param  style The style to use
 * @param  role  The role to use
 * @return The data for the given style and role
 */
QVariant LexerStyleModel::styleData(int style, int role) const
{
	switch (role) {
	case Qt::DisplayRole:
		return QString("Abc123");

	case Qt::FontRole:
		return lexer_->font(style);

	case Qt::ForegroundRole:
		return lexer_->color(style);

	case Qt::BackgroundRole:
		return lexer_->paper(style);

	default:
		;
	}

	return QVariant();
}

/**
 * @param  prop Property value
 * @return A display name for this property
 */
QString LexerStyleModel::propertyName(StyleProperty prop) const
{
	switch (prop) {
	case Font:
		return tr("Font");

	case Foreground:
		return tr("Text Colour");

	case Background:
		return tr("Background Colour");
	}

	return QString();
}

/**
 * Returns a value used to display and edit the given property for the given
 * style.
 * Each property provides a value for one of the display roles, as well as for
 * Qt::EditRole. The latter is used by LexerStyleView to determine which
 * editor to use to modify the property's data.
 * @param  style The style to use
 * @param  prop  The property
 * @param  role  The role for which to return the data
 * @return The property's data
 */
QVariant LexerStyleModel::propertyData(int style, StyleProperty prop,
                                       int role) const
{
	switch (prop) {
	case Font:
		switch (role) {
		case Qt::DisplayRole:
			return lexer_->font(style);

		case Qt::EditRole:
			// Can edit the font only if not using the default font for all
			// styles.
			if (!useDefaultFont_)
				return lexer_->font(style);
		}
		break;

	case Foreground:
		switch (role) {
		case Qt::DecorationRole:
		case Qt::EditRole:
			return lexer_->color(style);
		}
		break;

	case Background:
		switch (role) {
		case Qt::DecorationRole:
		case Qt::EditRole:
			return lexer_->paper(style);
		}
		break;
	}

	return QVariant();
}

} // namespace Editor

} // namespace KScope
