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

#include <QMdiSubWindow>
#include <QFileDialog>
#include "editorcontainer.h"
#include "editorconfigdialog.h"

namespace KScope
{

namespace App
{

EditorContainer::EditorContainer(QWidget* parent) : QMdiArea(parent),
	newFileIndex_(1)
{
	// Load editor configuration settings.
	QSettings settings;
	settings.beginGroup("Editor");
	config_.load(settings);
	settings.endGroup();

	// Notify when an active editor is available.
	connect(this, SIGNAL(subWindowActivated(QMdiSubWindow*)), this,
	        SLOT(windowActivated(QMdiSubWindow*)));
}

EditorContainer::~EditorContainer()
{
}

/**
 * Returns the editor widget of the active sub-window.
 * @return  The active editor widget, NULL if there are no open editors
 */
Editor* EditorContainer::currentEditor() const
{
	QMdiSubWindow* window;

	// Get the active sub-window.
	window = activeSubWindow();
	if (window == NULL)
		return NULL;

	// Return the editor widget.
	return static_cast<Editor*>(window->widget());
}

/**
 * Fills the "Window" menu with a list of all open sub-window titles.
 * @param  wndMenu  The menu to populate
 */
void EditorContainer::populateWindowMenu(QMenu* wndMenu) const
{
	QMap<QString, QMdiSubWindow*>::ConstIterator itr;

	// Add an entry for each open sub-window.
	for (itr = fileMap_.begin(); itr != fileMap_.end(); ++itr)
		wndMenu->addAction(itr.key());

	// Activate a sub-window when its menu entry is selected.
	connect(wndMenu, SIGNAL(triggered(QAction*)), this,
	        SLOT(handleWindowAction(QAction*)));
}

void EditorContainer::newFile()
{
	(void)getEditor(QString(), true);
}

void EditorContainer::openFile()
{
	QString path;

	path = QFileDialog::getOpenFileName(0, tr("Open File"));
	if (!path.isEmpty())
		(void)getEditor(path, true);
}

void EditorContainer::configEditor()
{
	// Show the "Configure Editor" dialogue.
	EditorConfigDialog dlg(config_, this);
	if (dlg.exec() == QDialog::Rejected)
		return;

	// Get the configuration parameters from the dialogue.
	dlg.getConfig(config_);

	// Store the editor configuration.
	QSettings settings;
	settings.beginGroup("Editor");
	config_.store(settings);
	settings.endGroup();

	foreach (QMdiSubWindow* window, fileMap_) {
		Editor* editor = static_cast<Editor*>(window->widget());
		editor->applyConfig(config_);
	}
}

/**
 * Activates an editor window showing the given location.
 * The file, line and column values of the Location structure are used to open
 * an editor for the given file (or activate an existing one), and to set the
 * cursor to the requested coordinates.
 * @param  loc  The requested location
 */
void EditorContainer::gotoLocation(const Core::Location& loc)
{
	QMdiSubWindow* window;
	Editor* editor;

	// Open/activate an editor window for the requested file.
	window = getEditor(loc.file_, true);
	if (window) {
		// Set the cursor to the given line and column.
		editor = static_cast<Editor*>(window->widget());
		editor->setCursorPosition(loc.line_, loc.column_);
	}
}

/**
 */
void EditorContainer::findText()
{
	Editor* editor = currentEditor();
	if (editor != NULL)
		editor->findFirst();
}

/**
 * @param  path
 * @param  activate
 * @return
 */
QMdiSubWindow* EditorContainer::getEditor(const QString& path, bool activate)
{
	QMap<QString, QMdiSubWindow*>::Iterator itr;
	QMdiSubWindow* window;
	Editor* editor;
	QString name;

	itr = fileMap_.find(path);
	if (itr != fileMap_.end()) {
		if (activate)
			activateEditor(*itr);

		return *itr;
	}

	editor = new Editor(this);

	if (!path.isEmpty()) {
		if (!editor->load(path)) {
			delete editor;
			return NULL;
		}

		name = path;
	}
	else {
		name = QString("Untitled %1").arg(newFileIndex_++);
	}

	editor->applyConfig(config_);

	window = addSubWindow(editor);
	window->setAttribute(Qt::WA_DeleteOnClose);
	window->setWindowTitle(name);
	fileMap_[name] = window;

	if (activate)
		activateEditor(window);

	return window;
}

void EditorContainer::activateEditor(QMdiSubWindow* window)
{
	Editor* editor;

	window->show();
	editor = static_cast<Editor*>(window->widget());
	editor->setFocus();
}

void EditorContainer::handleWindowAction(QAction* action)
{
	(void)getEditor(action->text(), true);
}

/**
 * Emits the hasActiveEditor() signal whenever the active window changes.
 * @param  window  The new active window, or NULL if there is no such window
 */
void EditorContainer::windowActivated(QMdiSubWindow* window)
{
	emit hasActiveEditor(window != NULL);
}

} // namespace App

} // namespace KScope
