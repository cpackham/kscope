/*
 * parser.h
 *
 *  Created on: Oct 9, 2008
 *      Author: elahav
 */

#ifndef __CORE_PARSER_H__
#define __CORE_PARSER_H__

#include <QString>
#include <QVariant>

namespace KScope
{

namespace Parser
{

typedef QList<QVariant> CapList;

template<class Exp1T, class Exp2T>
struct Concat;

template<class ExpT>
struct Kleene;

/**
 * Syntactic-sugar operators for building parsers out of the basic blocks.
 * Each parser class T should inherit from Operators<T>.
 */
template<class ExpT>
struct Operators
{
	template<class Exp2T>
	Concat<ExpT, Exp2T> operator<<(const Exp2T&) const;

	Kleene<ExpT> operator*() const;
};

/**
 * Matches a fixed-string.
 */
struct Literal : public Operators<Literal>
{
	/**
	 * Class constructor.
	 * @param  str  The string to match.
	 */
	Literal(const QString& str) : str_(str) {}

	/**
	 * Matches the object's string with a prefix of the input.
	 * @param   input  The input string
	 * @param   pos    The current position in the input string
	 * @param   caps   An ordered list of captured values
	 * @return  true if the input has a mathcing prefix, false otherwise
	 */
	bool match(const QString& input, int& pos, CapList& caps) const {
		(void)caps;

		if (input.mid(pos, str_.length()) == str_) {
			pos += str_.length();
			return true;
		}

		return false;
	}

private:
	/** The string to match. */
	const QString str_;
};

/**
 * Captures a base-10 numeric value.
 */
struct Number : public Operators<Number>
{
	/**
	 * Matches a non-empty sequence of digits, up to the first non-digit
	 * character (or the end of the input).
	 * @param   input  The input string
	 * @param   pos    The current position in the input string
	 * @param   caps   An ordered list of captured values
	 * @return  true if matched a number, false otherwise
	 */
	bool match(const QString& input, int& pos, CapList& caps) const {
		int digit, number = 0;
		bool result = false;

		 // Iterate to the end of the input, or until a non-digit character is
		 // found.
		while ((pos < input.size())
		       && ((digit = input[pos].digitValue()) != -1)) {
			// At least one digit.
			// Update the captured numeric value, the position and indicate that
			// a number has been found.
			number = (number * 10) + digit;
			pos++;
			result = true;
		}

		if (!result)
			return false;

		caps << number;
		return true;
	}
};

/**
 * Captures a string delimited by a single character.
 * The default delimiter causes the string to match to the end of the input.
 */
struct String : public Operators<String>
{
	/**
	 * Class constructor.
	 * @param  delimiter   The delimiting character
	 * @param  allowEmpty  Whether empty strings should be accepted
	 */
	String(QChar delimiter = 0, bool allowEmpty = false)
		: delimiter_(delimiter), allowEmpty_(allowEmpty) {}

	/**
	 * Matches a string up to the object's delimiter.
	 * @param   input  The input string
	 * @param   pos    The current position in the input string
	 * @param   caps   An ordered list of captured values
	 * @return  true if matched a non-empty string, false otherwise
	 */
	bool match(const QString& input, int& pos, CapList& caps) const {
		QString result;

		// Get a substring up to the object's delimiter.
		if (delimiter_ != 0)
			result = input.mid(pos).section(delimiter_, 0, 0);
		else
			result = input.mid(pos);

		if (!allowEmpty_ && (result.length() == 0))
			return false;

		// Update position and captured values list.
		pos += result.length();
		caps << result;
		return true;
	}

private:
	/** The delimiting character. */
	QChar delimiter_;

	/** Whether empty strings should be accepted. */
	bool allowEmpty_;
};

/**
 * Swallows whitespace.
 */
struct Whitespace : public Operators<Number>
{
	/**
	 * Matches a (possibly empty) sequence of any space characters.
	 * @param   input  The input string
	 * @param   pos    The current position in the input string
	 * @param   caps   An ordered list of captured values
	 * @return  Always true
	 */
	bool match(const QString& input, int& pos, CapList& caps) const {
		(void)caps;
		while ((pos < input.size()) && (input[pos].isSpace()))
			pos++;

		return true;
	}
};

/**
 * Concatenates two parsers.
 * Matches input that is matched first by one parser, and then by the other.
 */
template<class Exp1T, class Exp2T>
struct Concat : public Operators< Concat<Exp1T, Exp2T> >
{
	Concat(Exp1T exp1, Exp2T exp2) : exp1_(exp1), exp2_(exp2) {}

	bool match(const QString& input, int& pos, CapList& caps) const {
		return exp1_.match(input, pos, caps)
		       && exp2_.match(input, pos, caps);
	}

private:
	Exp1T exp1_;
	Exp2T exp2_;
};

/**
 * A Kleene-star closure.
 * Matches input matched by zero or more instances of a parser.
 */
template<class ExpT>
struct Kleene : public Operators< Kleene<ExpT> >
{
	Kleene(ExpT exp) : exp_(exp) {}

	bool match(const QString& input, int& pos, CapList& caps) const {
		while (pos < input.length()) {
			if (!exp_.match(input, pos, caps))
				return false;
		}

		return true;
	}

private:
	ExpT exp_;
};

/**
 * Implements the concatenation operator (<<) for building parsers.
 */
template<class ExpT>
template<class Exp2T>
Concat<ExpT, Exp2T> Operators<ExpT>::operator<<(const Exp2T& exp2) const {
	return Concat<ExpT, Exp2T>(*static_cast<ExpT const*>(this), exp2);
}

/**
 * Implements the Kleene-star operator (*) for building parsers.
 */
template<class ExpT>
Kleene<ExpT> Operators<ExpT>::operator*() const {
	return Kleene<ExpT>(*static_cast<ExpT const*>(this));
}

}

}

#endif // __CORE_PARSER_H__
