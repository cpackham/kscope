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

#ifndef __EDITOR_LEXERSTYLEMODEL_H__
#define __EDITOR_LEXERSTYLEMODEL_H__

#include <QAbstractItemModel>
#include <qscilexer.h>

namespace KScope
{

namespace Editor
{

/**
 * A model for displaying/editing lexer styles.
 * The model holds the data in two levels. First-level items represent styles
 * (e.g., keyword, number, comment), while second-level items represent the
 * different properties of the style (font, colour, etc.). The items are
 * distinguished by their internal IDs: styles have an ID of -1, with the row
 * number matching the style number, while properties hold the number of the
 * style in the internal ID.
 * @author Elad Lahav
 */
class LexerStyleModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	LexerStyleModel(QsciLexer*, QObject* parent = NULL);
	~LexerStyleModel();

	// QAbstractItemModel implementation.
	QModelIndex index(int, int,
	                  const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex&) const;
	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	QVariant data(const QModelIndex&, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex&, const QVariant&,
	             int role = Qt::EditRole);

public slots:
	void useDefaultFont(bool);
	void setDefaultFont(const QFont&);

private:
	/**
	 * The lexer to display.
	 */
	QsciLexer* lexer_;

	/**
	 * The number of available styles.
	 */
	int styleNum_;

	/**
	 * Whether all styles should be using the same (default) font.
	 */
	bool useDefaultFont_;

	/**
	 * Style properties.
	 */
	enum StyleProperty {
		Font = 0,
		Foreground,
		Background,
		_Last = Background
	};

	QVariant styleData(int, int) const;
	QString propertyName(StyleProperty) const;
	QVariant propertyData(int, StyleProperty, int) const;
};

} // namespace Editor

} // namespace KScope

#endif // __EDITOR_LEXERSTYLEMODEL_H__
