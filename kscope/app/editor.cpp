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

#include <QMessageBox>
#include <QDebug>
#include <qscilexercpp.h>
#include "editor.h"
#include "fileiothread.h"
#include "findtextdialog.h"

namespace KScope
{

namespace App
{

/**
 * Class constructor.
 * @param  parent  Owner object
 */
Editor::Editor(QWidget* parent) : QsciScintilla(parent),
	isLoading_(false),
	onLoadLine_(0),
	onLoadColumn_(0),
	onLoadFocus_(false)
{
}

/**
 * Class destructor.
 */
Editor::~Editor()
{
}

/**
 * Asynchronously loads the contents of the given file into the editor.
 * Launches a thread that reads the file contents. When done, the thread signals
 * the editor with the read text.
 * During the loading process, the editor widget is disabled. Any calls to
 * setCursorPosition() or setFocus() are delayed until loading finishes.
 * @param  path  The path of the file to load
 * @return true if the loading process started successfully, false otherwise
 */
bool Editor::load(const QString& path)
{
	FileIoThread* thread;

	// Indicate that loading is in progress.
	isLoading_ = true;
	setEnabled(false);
	setText(tr("Loading..."));

	// TODO:
	// Set up the correct lexer based on the file's MIME type.
	setLexer(new QsciLexerCPP(this));

	// Create and start the loading thread.
	thread = new FileIoThread(this);
	connect(thread, SIGNAL(done(const QString&)), this,
	        SLOT(loadDone(const QString&)));
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	if (!thread->load(path)) {
		setText(tr("Loading failed"));
		return false;
	}

	// Store the path.
	path_ = path;
	return true;
}

/**
 * Moves the cursor to the requested position in the document.
 * This function translates 1-based line and column indexes into the 0-based
 * values used by Scintilla. We use 0 values as "don't cares", that do not
 * change the current value of the respective dimension (line or column).
 * @param  line    1-based line number, 0 to keep the current line
 * @param  column  1-based column number, 0 to keep the current column
 */
void Editor::setCursorPosition(uint line, uint column)
{
	int curLine, curColumn;

	// Wait for file loading to complete before setting a new position.
	if (isLoading_) {
		onLoadLine_ = line;
		onLoadColumn_ = column;
		return;
	}

	// Get current values.
	getCursorPosition(&curLine, &curColumn);

	// Determine the new line position.
	if (line == 0)
		line = curLine;
	else
		line--;

	// Determine the new column position.
	if (column == 0)
		column = curColumn;
	else
		column--;

	// Set the new cursor position.
	QsciScintilla::setCursorPosition(line, column);
}

/**
 * Returns a symbol for automatic selection.
 * If any text is selected in the editor, it is returned. Otherwise, the method
 * returns the word on which the cursor is currently positioned.
 * @return The current text
 */
QString Editor::currentSymbol() const
{
	// Return any selected text.
	// TODO: Should we test for a valid symbol here to?
	if (hasSelectedText())
		return QsciScintilla::selectedText();

	// No selected text.
	// Get the boundaries of the word from the current cursor position.
	long pos, start, end;
	pos = SendScintilla(SCI_GETCURRENTPOS);
	start = SendScintilla(SCI_WORDSTARTPOSITION, pos, 0L);
	end = SendScintilla(SCI_WORDENDPOSITION, pos, 0L);

	// Return an empty string if no word is found.
	if (start >= end)
		return QString();

	// Extract the word's text using its position boundaries.
	QByteArray curText;
	curText.resize(end - start );
	SendScintilla(SCI_GETTEXTRANGE, start, end, curText.data());

	// NOTE: Scintilla's definition of a "word" does not correspond to a
	// "symbol". Make sure the result contains only alpha-numeric characters
	// or an underscore.
	QString symbol(curText);
	if (!QRegExp("\\w+").exactMatch(symbol))
		return QString();

	return symbol;
}

/**
 * Sets the input focus to the editor.
 * If the editor is currently loading a file, the focus will be set when the
 * process finishes.
 */
void Editor::setFocus()
{
	if (isLoading_)
		onLoadFocus_ = true;
	else
		QsciScintilla::setFocus();
}

/**
 * Updates the editor widget to use a new set of configuration parameters.
 * @param  config  The configuration parameters
 */
void Editor::applyConfig(const Config& config)
{
	QsciLexer* lex = lexer();
	if (lex)
		lex->setFont(config.font_);
	else
		setFont(config.font_);

	setIndentationsUseTabs(config.indentTabs_);
	setTabWidth(config.tabWidth_);
	setCaretLineVisible(config.hlCurLine_);
}

/**
 * Fills a Config structure with the current configuration options.
 * Used to get QScintilla's default values.
 * @param config
 */
void Editor::getConfig(Config& config)
{
	QsciLexer* lex = lexer();
	if (lex)
		config.font_ = lex->defaultFont();
	else
		config.font_ = font();

	config.indentTabs_ = indentationsUseTabs();
	config.tabWidth_ = tabWidth();
	config.hlCurLine_ = false;
}

/**
 * Fills a Location structure with information on the current file name, line
 * and column.
 * @param  loc  The structure to fill
 */
void Editor::getCurrentLocation(Core::Location& loc)
{
	int line, col;

	// Get the current cursor position.
	getCursorPosition(&line, &col);

	// Fill the structure.
	loc.file_ = path_;
	loc.line_ = line + 1;
	loc.column_ = col + 1;
	loc.text_ = text(line).trimmed();
}

/**
 * Searches for text inside the document.
 * Prompts the user for the text to find.
 */
void Editor::search()
{
	// Prompt for text to find.
	QString pattern = currentSymbol();
	SearchOptions options;
	if (FindTextDialog::promptPattern(pattern, options, this)
	    != QDialog::Accepted) {
		return;
	}

	// Find the first occurrence of the searched text.
	if (!QsciScintilla::findFirst(pattern, options.regExp_,
	                              options.caseSensitive_,
	                              options.wholeWordsOnly_,
	                              options.wrap_, !options.backward_)) {
		QString msg = tr("'%1' could not be found in the document")
		              .arg(pattern);
		QMessageBox::warning(this, tr("Pattern not found"), msg);
	}
}

/**
 * Searches for the next occurrence of the previously-selected text inside the
 * document.
 */
void Editor::searchNext()
{
	if (!QsciScintilla::findNext()) {
		QString msg = tr("No more matches.");
		QMessageBox::warning(this, tr("Pattern not found"), msg);
	}
}

/**
 * Loads editor configuration parameters.
 * @param  settings  The QSettings object to use for loading
 */
void Editor::Config::load(QSettings& settings)
{
	Editor::Config config;

	Editor().getConfig(config);
	font_ = settings.value("Font", config.font_).value<QFont>();
	hlCurLine_
		= settings.value("HighlightCurrentLine", config.hlCurLine_).toBool();
	indentTabs_ = settings.value("IndentWithTabs", config.indentTabs_).toBool();
	tabWidth_ = settings.value("TabWidth", config.tabWidth_).toInt();
}

/**
 * Stores editor configuration parameters.
 * @param  settings  The QSettings object to use for storing
 */
void Editor::Config::store(QSettings& settings)
{
	settings.setValue("Font", font_);
	settings.setValue("HighlightCurrentLine", hlCurLine_);
	settings.setValue("IndentWithTabs", indentTabs_);
	settings.setValue("TabWidth", tabWidth_);
}

/**
 * Called when the thread loading the file for the editor terminates.
 * Invokes any methods that were deferred while the file was loading.
 * @param  text  The contents of the file
 */
void Editor::loadDone(const QString& text)
{
	setText(text);
	isLoading_ = false;
	setCursorPosition(onLoadLine_, onLoadColumn_);
	setEnabled(true);

	if (onLoadFocus_) {
		setFocus();
		onLoadFocus_ = false;
	}
}

} // namespace App

} // namespace KScope
