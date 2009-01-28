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

#ifndef __APP_EDITORCONTAINER_H
#define __APP_EDITORCONTAINER_H

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMap>
#include <QMenu>
#include "globals.h"
#include "editor.h"
#include "locationhistory.h"
#include "session.h"

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

	void populateWindowMenu(QMenu*) const;
	bool canClose();
	void saveSession(Session&);
	void loadSession(Session&);

	inline Editor* currentEditor() {
		QMdiSubWindow* window = currentSubWindow();
		if (!window)
			return NULL;

		return static_cast<Editor*>(window->widget());
	}

public slots:
	void newFile();
	void openFile();
	void openFile(const QString&);
	void configEditor();
	void gotoLocation(const Core::Location&);
	void gotoNextLocation();
	void gotoPrevLocation();
	void showLocalTags();
	void browseHistory();
	void closeAll();

signals:
	void hasActiveEditor(bool has);
	void find();
	void findNext();

private:
	QMdiSubWindow* currentWindow_;
	QMap<QString, QMdiSubWindow*> fileMap_;
	uint newFileIndex_;
	Editor::Config config_;
	LocationHistory history_;
	bool blockWindowActivation_;

	QMdiSubWindow* getEditor(const Core::Location&, bool activate = true);

private slots:
	void handleWindowAction(QAction*);
	void windowActivated(QMdiSubWindow*);
	void removeEditor(const QString&);
	void remapEditor(const QString&, const QString&);
};

} // namespace App

} // namespace KScope

#endif // __APP_EDITORCONTAINER_H
