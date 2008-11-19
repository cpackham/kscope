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

#ifndef __CORE_STATEMACHINE_H
#define __CORE_STATEMACHINE_H

#include <QDebug>
#include "parser.h"

namespace KScope
{

namespace Core
{

/**
 * My attempt at a generic, fancy-looking, state-machine.
 * The main goal is to be able to describe a state machine implementation as
 * simply and elegantly as possible.
 * The machine is a set of states and a transition function. Given an input
 * string, the current state is checked for all outgoing edges, which hold
 * statically built parser objects. If the input string is matched by the
 * parser, that edge's in-vertex is set as the current state.
 * @author Elad Lahav
 */
class StateMachine
{
public:
	struct TransitionBase;

	/**
	 * A single state in the machine.
	 * The entire logic of the state machine is implemented in the list of
	 * Transition objects held by each state.
	 */
	struct State
	{
		State(QString name = "") : name_(name) {}
		State(const State& other) : name_(other.name_),
			transList_(other.transList_) {}

		bool isError() const { return transList_.isEmpty(); }

		QString name_;
		QList<TransitionBase*> transList_;
	};

	/**
	 * Default action type for matching transitions.
	 * Does nothing.
	 */
	struct NoAction
	{
		void operator()(const Parser::CapList& caps) const {
			(void)caps;
		}
	};

	/**
	 * Abstract base class for transitions.
	 * Since transition objects are created at compile time, we need this
	 * base class in order to be able to specify a list of pointers to
	 * transitions in the State class.
	 */
	struct TransitionBase
	{
		TransitionBase(const State& nextState) : nextState_(nextState) {}

		virtual bool run(const QString& input) const = 0;

		const State& nextState_;
	};

	template<class ParserT, class ActionT = NoAction>
	struct Transition : public TransitionBase
	{
		Transition(const State& nextState, const ParserT& parser)
			: TransitionBase(nextState), parser_(parser) {}
		Transition(const State& nextState, const ParserT& parser,
		           ActionT action)
			: TransitionBase(nextState), parser_(parser), action_(action) {}

		bool run(const QString& input) const {
			Parser::CapList caps;
			int pos = 0;

			if (!parser_.match(input, pos, caps))
				return false;

			action_(caps);
			return true;
		}

		ParserT parser_;
		ActionT action_;
	};

	/**
	 * Class constructor.
	 */
	StateMachine() : curState_(&initState_) {}

	/**
	 * Class destructor.
	 */
	~StateMachine() {
		while (!transList_.isEmpty())
			delete transList_.takeFirst();
	}

	bool step(const QString& input) {
		QList<TransitionBase*>::ConstIterator itr;

		if (curState_->isError()) {
			qDebug() << "Error state!";
			return false;
		}

		// Iterate over the list of transitions.
		for (itr = curState_->transList_.begin();
		     itr != curState_->transList_.end();
		     ++itr) {
			// Check for a matching regular expression.
			if ((*itr)->run(input)) {
				curState_ = &(*itr)->nextState_;
				return true;
			}
		}

		qDebug() << "Parse error!" << curState_->name_ << input;

		// Set the current state to NULL if matching failed.
		curState_ = &initState_;
		return false;
	}

	/**
	 * Sets the current state of the machine.
	 * @param  state  The new state
	 */
	void setState(const State& state) { curState_ = &state; }

	/**
	 * Sets the default state as the current one.
	 */
	void reset() { curState_ = &initState_; }

	template<class ParserT, class ActionT>
	void addRule(State& from, const ParserT& parser, const State& to,
	             const ActionT& action) {
		typedef Transition<ParserT, ActionT> TransT;
		TransT* trans = new TransT(to, parser, action);
		from.transList_.append(trans);
		transList_.append(trans);
	}

	template<class ParserT>
	void addRule(State& from, const ParserT& parser, const State& to) {
		typedef Transition<ParserT> TransT;
		TransT* trans = new TransT(to, parser);
		from.transList_.append(trans);
		transList_.append(trans);
	}

protected:
	State initState_;

private:
	const State* curState_;
	QList<TransitionBase*> transList_;
};

}

}

#endif // __CORE_STATEMACHINE_H
