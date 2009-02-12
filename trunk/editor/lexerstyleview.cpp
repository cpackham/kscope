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

#include <QTreeView>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QFontDialog>
#include <QColorDialog>
#include "lexerstyleview.h"
#include "lexerstylemodel.h"

namespace KScope
{

namespace Editor
{

LexerStyleView::LexerStyleView(QsciLexer* lexer, QWidget* parent)
	: QWidget(parent)
{
	// Create the model.
	model_ = new LexerStyleModel(lexer, this);

	// Create the tree view.
	QTreeView* view = new QTreeView(this);
	view->setModel(model_);
	connect(view, SIGNAL(activated(const QModelIndex&)), this,
	        SLOT(editStyle(const QModelIndex&)));

	// Create the "Default Font" check-box.
	QCheckBox* cbox = new QCheckBox(tr("Use default font for all styles"),
	                                this);
	connect(cbox, SIGNAL(toggled(bool)), model_, SLOT(useDefaultFont(bool)));

	// Layout widgets.
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(view);
	layout->addWidget(cbox);
	setLayout(layout);
}

LexerStyleView::~LexerStyleView()
{
}

void LexerStyleView::editStyle(const QModelIndex& index)
{
	QVariant editData = model_->data(index, Qt::EditRole);

	switch (editData.type()) {
	case QVariant::Font:
		{
			bool ok;
			QFont font = QFontDialog::getFont(&ok, editData.value<QFont>(),
			                                  this);
			if (ok)
				model_->setData(index, font);
		}
		break;

	case QVariant::Color:
		{
			QColor color = QColorDialog::getColor(editData.value<QColor>(),
			                                      this);
			if (color.isValid())
				model_->setData(index, color);
		}
		break;

	default:
		;
	}
}

} // namespace Editor

} // namespace KScope
