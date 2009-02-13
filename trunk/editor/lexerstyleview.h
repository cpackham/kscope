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

#ifndef __EDITOR_LEXERSTYLEVIEW_H__
#define __EDITOR_LEXERSTYLEVIEW_H__

#include <QWidget>
#include <qscilexer.h>
#include "lexerstylemodel.h"

namespace KScope
{

namespace Editor
{

/**
 * Used to display/edit lexer properties in a QTreeView widget.
 * @author Elad Lahav
 */
class LexerStyleView : public QWidget
{
	Q_OBJECT

public:
	LexerStyleView(QsciLexer*, QWidget* parent = NULL);
	~LexerStyleView();

	/**
	 * @return The lexer style model used by the tree view
	 */
	LexerStyleModel* model() { return model_; }

private:
	/**
	 * The lexer style model to use.
	 */
	LexerStyleModel* model_;

private slots:
	void editStyle(const QModelIndex&);
};

} // namespace Editor

} // namespace KScope

#endif // __EDITOR_LEXERSTYLEVIEW_H__
