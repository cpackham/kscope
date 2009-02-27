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

#include "actions.h"
#include "editor.h"

namespace KScope
{

namespace Editor
{

Actions::Actions(QObject* parent) : QObject(parent), editor_(NULL)
{
}

Actions::~Actions()
{
}

void Actions::setup(QMenu* menu)
{
	connect(this, SIGNAL(hasEditor(bool)), menu, SLOT(setEnabled(bool)));

	actCopy_ = new QAction(tr("&Copy"), this);
	actCopy_->setShortcut(QKeySequence("Ctrl+C"));
	actCopy_->setStatusTip(tr("Copy selected text"));
	connect(actCopy_, SIGNAL(triggered()), this, SLOT(copy()));
	menu->addAction(actCopy_);

	actCut_ = new QAction(tr("Cu&t"), this);
	actCut_->setShortcut(QKeySequence("Ctrl+X"));
	actCut_->setStatusTip(tr("Cut selected text"));
	connect(actCut_, SIGNAL(triggered()), this, SLOT(cut()));
	menu->addAction(actCut_);

	actPaste_ = new QAction(tr("&Paste"), this);
	actPaste_->setShortcut(QKeySequence("Ctrl+V"));
	actPaste_->setStatusTip(tr("Paste clipboard contents"));
	connect(actPaste_, SIGNAL(triggered()), this, SLOT(paste()));
	menu->addAction(actPaste_);

	menu->addSeparator();

	actUndo_ = new QAction(tr("&Undo"), this);
	actUndo_->setShortcut(QKeySequence("Ctrl+Z"));
	actUndo_->setStatusTip(tr("Undo last action"));
	connect(actUndo_, SIGNAL(triggered()), this, SLOT(undo()));
	menu->addAction(actUndo_);

	actRedo_ = new QAction(tr("&Redo"), this);
	actRedo_->setShortcut(QKeySequence("Ctrl+Y"));
	actRedo_->setStatusTip(tr("Repeat undone action"));
	connect(actRedo_, SIGNAL(triggered()), this, SLOT(redo()));
	menu->addAction(actRedo_);

	menu->addSeparator();

	actFind_ = new QAction(tr("&Find..."), this);
	actFind_->setShortcut(QKeySequence("Ctrl+F"));
	actFind_->setStatusTip(tr("Find text in file"));
	connect(actFind_, SIGNAL(triggered()), this, SLOT(find()));
	menu->addAction(actFind_);

	actFindNext_ = new QAction(tr("Find &Next"), this);
	actFindNext_->setShortcut(QKeySequence("F3"));
	actFindNext_->setStatusTip(tr("Find next occurrence of last search"));
	connect(actFindNext_, SIGNAL(triggered()), this, SLOT(findNext()));
	menu->addAction(actFindNext_);

	menu->addSeparator();

	actGotoLine_ = new QAction(tr("&Go to Line..."), this);
	actGotoLine_->setShortcut(QKeySequence("Ctrl+G"));
	actGotoLine_->setStatusTip(tr("Move cursor to a different line"));
	connect(actGotoLine_, SIGNAL(triggered()), this, SLOT(gotoLine()));
	menu->addAction(actGotoLine_);

	emit hasEditor(editor_ != NULL);
}

void Actions::setEditor(Editor* editor)
{
	editor_ = editor;
	emit hasEditor(editor_ != NULL);
}

void Actions::copy()
{
	editor_->copy();
}

void Actions::cut()
{
	editor_->cut();
}

void Actions::paste()
{
	editor_->paste();
}

void Actions::undo()
{
	editor_->undo();
}

void Actions::redo()
{
	editor_->redo();
}

void Actions::find()
{
	editor_->search();
}

void Actions::findNext()
{
	editor_->searchNext();
}

void Actions::gotoLine()
{
	editor_->gotoLine();
}

} // namespace Editor

} // namespace KScope
