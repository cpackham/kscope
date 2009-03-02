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

#ifndef __EDITOR_VISCINTILLA_H__
#define __EDITOR_VISCINTILLA_H__

#include <QKeyEvent>
#include <QMultiHash>
#include <qsciscintilla.h>

namespace KScope
{

namespace Editor
{

/**
 * An implementation of a Vi-style commands on top of Scintilla.
 * @author Elad Lahav
 */
class ViScintilla : public QsciScintilla
{
	Q_OBJECT

public:
	ViScintilla(QWidget*);
	~ViScintilla();

	/**
	 * Editing modes.
	 */
	enum EditMode
	{
		/** Vi compatibility is disabled. */
		Disabled,
		/** Scintilla's default. */
		InsertMode,
		/** Vi normal mode. */
		NormalMode,
		/** Vi visual mode. */
		VisualMode,
	};

	struct Command
	{
		enum ProcessResult {
			Continue,
			Done,
			Exit,
			NotHandled
		};

		virtual ProcessResult processKey(QKeyEvent* event, ViScintilla* editor,
		                                 QString& seq) = 0;
		virtual QString name() const = 0;
	};

	void setEditMode(EditMode mode);

	/**
	 * @return The current edit mode
	 */
	EditMode editMode() const { return mode_; }

signals:
	/**
	 * Emitted when the editing mode changes.
	 * @param  mode The new mode
	 */
	void editModeChanged(Editor::ViScintilla::EditMode mode);

	/**
	 * Sends a message to be displayed by the application.
	 * @param  msg       The message to display
	 * @param  msTimeOut How long to display the message, in milliseconds
	 */
	void message(const QString& msg, int msTimeOut);

protected:
	/**
	 * The current editing mode.
	 */
	EditMode mode_;

	/**
	 * The current command, based on the first key in the sequence, NULL if no
	 * command is active.
	 */
	Command* curCommand_;

	/**
	 * The current key sequence.
	 */
	QString cmdSequence_;

	void keyPressEvent(QKeyEvent*);

private:
	/**
	 * Maps keys to lists of commands.
	 * Commands hashed to the same key are differentiated by modifier keys.
	 * @author Elad Lahav
	 */
	class CommandHash : public QMultiHash<int, Command*>
	{
	public:
		CommandHash();
		~CommandHash();

	private:
		QList<Command*> cmdList_;
	};

	static CommandHash commandHash_;
};

} // namespace Editor

} // namespace KScope

#endif  // __EDITOR_VISCINTILLA_H__
