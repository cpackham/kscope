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
#include <QDebug>
#include "editorcontainer.h"
#include "editorconfigdialog.h"

namespace KScope
{

namespace App
{

/**
 * Class constructor.
 * @param  parent  Parent widget
 */
EditorContainer::EditorContainer(QWidget* parent)
	: QMdiArea(parent), activeEditor_(NULL), newFileIndex_(1)
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

/**
 * Class destructor.
 */
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

/**
 * Creates an editor window with an empty, unnamed file.
 */
void EditorContainer::newFile()
{
	(void)getEditor(QString(), true);
}

/**
 * Prompts the user for a file name, and creates an editor window for editing
 * the selected file.
 */
void EditorContainer::openFile()
{
	QString path = QFileDialog::getOpenFileName(0, tr("Open File"));
	if (!path.isEmpty())
		(void)getEditor(path, true);
}

/**
 * Displays the editor configuration dialogue.
 * Any changes to the configuration are then applied to all open editor windows.
 */
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
 * Sets the focus to a line in an editor window matching the given location.
 * @param  loc  The location to go to
 */
void EditorContainer::gotoLocation(const Core::Location& loc)
{
	if (gotoLocationInternal(loc))
		history_.add(loc);
}

/**
 * Sets the focus to a line in an editor window matching the next location in
 * the history list.
 */
void EditorContainer::gotoNextLocation()
{
	Core::Location loc;
	if (history_.next(loc))
		gotoLocationInternal(loc);
}

/**
 * Sets the focus to a line in an editor window matching the previous location
 * in the history list.
 */
void EditorContainer::gotoPrevLocation()
{
	Core::Location loc;
	if (history_.prev(loc))
		gotoLocationInternal(loc);
}

/**
 * Finds an editor window for the given file.
 * If one does not exist, a new one is created.
 * @param  path      The path of the file to edit
 * @param  activate  Whether to also activate the window
 * @return The found/created editor if successful, NULL otherwise
 */
QMdiSubWindow* EditorContainer::getEditor(const QString& path, bool activate)
{

	// Try to find an existing editor window, based on the path.
	QMap<QString, QMdiSubWindow*>::Iterator itr = fileMap_.find(path);
	if (itr != fileMap_.end()) {
		if (activate)
			activateEditor(*itr);

		return *itr;
	}

	// Create a new editor widget.
	Editor* editor = new Editor(this);

	// Open the given file in the editor.
	QString name;
	if (!path.isEmpty()) {
		if (!editor->load(path)) {
			delete editor;
			return NULL;
		}

		name = path;
	}
	else {
		// No path supplied, treat as a new file.
		name = QString("Untitled %1").arg(newFileIndex_++);
	}

	editor->applyConfig(config_);

	// Create a new sub window for the editor.
	QMdiSubWindow* window = addSubWindow(editor);
	window->setAttribute(Qt::WA_DeleteOnClose);
	window->setWindowTitle(name);
	fileMap_[name] = window;

	if (activate)
		activateEditor(window);

	return window;
}

/**
 * Makes the given window the active one.
 * Shows the window and sets the focus to the editor.
 * @param  window  The window to activate
 */
void EditorContainer::activateEditor(QMdiSubWindow* window)
{
	window->show();
	static_cast<Editor*>(window->widget())->setFocus();
}

/**
 * Activates an editor window showing the given location.
 * The file, line and column values of the Location structure are used to open
 * an editor for the given file (or activate an existing one), and to set the
 * cursor to the requested coordinates.
 * @param  loc  The requested location
 * @return true if successful, false otherwise
 */
bool EditorContainer::gotoLocationInternal(const Core::Location& loc)
{
	QMdiSubWindow* window;
	Editor* editor;

	// Open/activate an editor window for the requested file.
	window = getEditor(loc.file_, true);
	if (!window)
		return false;

	// Set the cursor to the given line and column.
	editor = static_cast<Editor*>(window->widget());
	editor->setCursorPosition(loc.line_, loc.column_);
	return true;
}

/**
 * Common handler for the file names in the "Window" menu.
 * Activates the window corresponding to the chosen file.
 * @param  action  The triggered action
 */
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
	// Get the new active editor widget, if any.
	Editor* editor = window ?  static_cast<Editor*>(window->widget()) : NULL;
	if (editor != activeEditor_) {
		// Stop forwarding signals to the active editor.
		if (activeEditor_)
			disconnect(activeEditor_);

		// Update the active editor.
		activeEditor_ = editor;

		if (activeEditor_) {
			// Forward signals.
			connect(this, SIGNAL(find()), activeEditor_, SLOT(search()));
			connect(this, SIGNAL(findNext()), activeEditor_,
			        SLOT(searchNext()));

			emit hasActiveEditor(true);
		}
		else {
			emit hasActiveEditor(false);
		}
	}
}

} // namespace App

} // namespace KScope
