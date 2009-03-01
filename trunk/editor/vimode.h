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

#ifndef __EDITOR_VIMODE_H__
#define __EDITOR_VIMODE_H__

#include <QKeyEvent>
#include <QMultiHash>

namespace KScope
{

namespace Editor
{

class Editor;

/**
 * An implementation of a Vi-like Ex mode.
 * @author Elad Lahav
 */
class ViMode
{
public:
	ViMode(Editor*);
	~ViMode();

	struct Command
	{
		enum ProcessResult {
			Continue,
			Done,
			Exit,
			NotHandled
		};

		virtual ProcessResult processKey(QKeyEvent* event, Editor* editor,
		                                 QString& seq) = 0;
		virtual QString name() const = 0;
	};

	void processKey(QKeyEvent*);
	void setEnabled(bool enabled);

	/**
	 * @return true if the mode is enabled, false otherwise
	 */
	bool isEnabled() const { return enabled_; }

	/**
	 * @return The current command sequence
	 */
	const QString& sequence() const { return cmdSequence_; }

private:
	/**
	 * The controlled editor.
	 */
	Editor* editor_;

	/**
	 * Whether the mode is enabled.
	 */
	bool enabled_;

	/**
	 * The current command, based on the first key in the sequence, NULL if no
	 * command is active.
	 */
	Command* curCommand_;

	/**
	 * The current key sequence.
	 */
	QString cmdSequence_;

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

#endif  // __EDITOR_VIMODE_H__
