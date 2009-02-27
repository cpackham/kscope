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

#ifndef __EDITOR_ACTIONS_H__
#define __EDITOR_ACTIONS_H__

#include <QObject>
#include <QAction>
#include <QMenu>

namespace KScope
{

namespace Editor
{

class Editor;

/**
 * Manages actions for an editor.
 * This class provides a slot proxy for an editor object. This is beneficial in
 * a multi-editor environment, where the active window changes constantly. The
 * alternative would be to disconnect and connect editor slots directly, which
 * is inefficient.
 * @author Elad Lahav
 */
class Actions : public QObject
{
	Q_OBJECT

public:
	Actions(QObject* parent = 0);
	~Actions();

	void setup(QMenu*);
	void setEditor(Editor*);

public slots:
	void copy();
	void cut();
	void paste();
	void undo();
	void redo();
	void find();
	void findNext();
	void gotoLine();

signals:
	void hasEditor(bool has);

private:
	Editor* editor_;

	QAction* actCopy_;

	QAction* actCut_;

	QAction* actPaste_;

	QAction* actUndo_;

	QAction* actRedo_;

	QAction* actFind_;

	QAction* actFindNext_;

	QAction* actGotoLine_;
};

} // namespace Editor

} // namespace KScope

#endif // __EDITOR_ACTIONS_H__
