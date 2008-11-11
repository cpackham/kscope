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

#ifndef __KSCOPE_EDITORCONTAINER_H
#define __KSCOPE_EDITORCONTAINER_H

#include <QMdiArea>
#include <QMap>
#include <QMenu>
#include "globals.h"
#include "editor.h"

namespace KScope
{

namespace App
{

/**
 * Manages editor windows.
 * This is the central widget of the main window. It contains and manages all
 * open editor windows in an MDI-style.
 * @author Elad Lahav
 */
class EditorContainer : public QMdiArea
{
	Q_OBJECT

public:
	EditorContainer(QWidget* parent = 0);
	~EditorContainer();

	Editor* currentEditor() const;
	void populateWindowMenu(QMenu*) const;

public slots:
	void newFile();
	void openFile();
	void configEditor();
	void gotoLocation(const Core::Location&);

private:
	QMap<QString, QMdiSubWindow*> fileMap_;
	uint newFileIndex_;
	Editor::Config config_;

	QMdiSubWindow* getEditor(const QString& path, bool activate = false);
	void activateEditor(QMdiSubWindow*);

private slots:
	void handleWindowAction(QAction*);
};

}

}

#endif // __KSCOPE_EDITORCONTAINER_H
