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
	: QAbstractItemModel(parent), lexer_(lexer)
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

/**
 * Creates an index.
 * An index can only be created with the root (invalid) index as a parent.
 * @param  row    The row of the index, relative to the parent
 * @param  column The column of the index
 * @param  parent The parent index.
 * @return The resulting index
 */
QModelIndex LexerStyleModel::index(int row, int column,
                                   const QModelIndex& parent) const
{
	if (parent.isValid())
		return QModelIndex();

	return createIndex(row, column, NULL);
}

/**
 * Finds the parent for the given index.
 * Since this is a list model, an invalid index will always be returned.
 * @param  index The index for which the parent is needed
 * @return The parent index
 */
QModelIndex LexerStyleModel::parent(const QModelIndex& index) const
{
	(void)index;
	return QModelIndex();
}

/**
 * Determines the number of child indices for the given parent.
 * Only the root has children, and their number is equal to styleNum_;
 * @param  parent The parent index
 * @return The number of children for the index
 */
int LexerStyleModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return styleNum_;
}

/**
 * Determines the number of columns in children of the given index.
 * The model presents two columns, one for the style name and the other for the
 * style's formatting.
 * @param  parent The requested index
 * @return The number of columns for children of the index
 */
int LexerStyleModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return 2;
}

QVariant LexerStyleModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() >= styleNum_)
		return QVariant();

	switch (index.column()) {
	case 0:
		if (role == Qt::DisplayRole)
			return lexer_->description(index.row());
		break;

	case 1:
		switch (role) {
		case Qt::DisplayRole:
			return QString("Abc123");

		case Qt::FontRole:
			return lexer_->font(index.row());

		case Qt::TextColorRole:
			return lexer_->color(index.row());

		case Qt::BackgroundRole:
			return lexer_->paper(index.row());
		}
		break;

	default:
		;
	}

	return QVariant();
}

} // namespace Editor

} // namespace KScope
