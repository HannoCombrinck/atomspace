/*
 * BackwardChainer.h
 *
 * Copyright (C) 2015 OpenCog Foundation
 *
 * Author: Misgana Bayetta <misgana.bayetta@gmail.com>  October 2014
 *         William Ma <https://github.com/williampma>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef BACKWARDCHAINER_H_
#define BACKWARDCHAINER_H_

#include <opencog/rule-engine/Rule.h>
#include <opencog/rule-engine/UREConfigReader.h>

#include "Target.h"

class BackwardChainerUTest;

namespace opencog
{

/**
 * TODO: update that comment
 *
 * Backward chaining falls into two cases
 *
 * 1. Truth value query - Given a target atom whose truth value is not
 *    known and a pool of atoms, find a way to estimate the truth
 *    value of the target Atom, via combining the atoms in the pool
 *    using the inference rule. Eg. The target is "Do people breath"
 *
 *    (InheritanceLink people breath)
 *
 *    the truth value of the target is estimated via doing the
 *    inference "People are animals, animals breathe, therefore people
 *    breathe."
 *
 * 2. Variable fulfillment query - Given a target Link (Atoms may be
 *    Nodes or Links) with one or more VariableAtoms among its
 *    targets, figure what atoms may be put in place of these
 *    VariableAtoms, so as to give the grounded targets a high
 *
 *    strength * confidence
 *
 *    Eg. What breathes (InheritanceLink $X breath) can be fulfilled
 *    by pattern matching, whenever there are are multiple values to
 *    fill $X we will use fitness value measure to choose the best
 *    other compound example is what breathes and adds ANDLink
 *    InheritanceLink $X Breath InheritanceLink $X adds
 *
 * Anatomy of current implementation
 * =================================
 *
 * 1. First check if the target matches to something in the knowledge base
 *    already
 *
 * 2. If not, choose an inference Rule R (using some Rule selection criteria)
 *    whose output can unify to the target
 *
 * 3. Reverse ground R's input to restrict the permises search
 *
 * 4. Find all premises that matches the restricted R's input by Pattern
 *    Matching
 *
 * 5. For each set of premises, Forward Chain (a.k.a apply the rule, or
 *    Pattern Matching) on the R to see if it can solve the target.
 *
 * 6. If not, add the permises to the targets list (in addition to some
 *    permise selection criteria)
 *
 * 7. Do target selection and repeat.
 */

class BackwardChainer
{
    friend class ::BackwardChainerUTest;

public:
	BackwardChainer(AtomSpace& as, const Handle& rbs,
	                const Handle& htarget,
	                const Handle& hfocus_set = Handle::UNDEFINED,
	                const TargetFitness& fitness = TargetFitness());

	UREConfigReader& get_config();
	const UREConfigReader& get_config() const;

	/**
	 * Perform a single backward chaining inference step.
	 */
	void do_step();

	/**
	 * Perform backward chaining inference till the termination
	 * criteria have been met.
	 */
	void do_chain();

	/**
	 * @return true if the termination criteria have been met.
	 */
	bool termination();

	HandleMultimap get_chaining_result();

private:
	// Select the target to expand
	TargetPtr select_target();

	// Fulfill, apply possible inferences in a forward way and pattern
	// matchings in order to fulfill the given target
	void fulfill_target(Target& target);

	// Select a valid rule given a target. The selected is a new
	// object because a new rule is created, its variables are
	// uniquely renamed, possibly some partial substitutions are
	// applied.
	Rule select_rule(const Target& target);

	// Return all valid rules, in the sense that these rules may
	// possibly be used to infer the target.
	RuleSeq get_valid_rules(const Target& target);

	// Expand the back-inference tree of a target
	void expand_bit(Target& target, const Rule& rule);

	AtomSpace& _as;
	UREConfigReader _configReader;
	Handle _init_target;
	AtomSpace _focus_space;
	int _iteration;

	HandleTargetPtrMap _h2t;

	const std::vector<Rule>& _rules;
};


} // namespace opencog

#endif /* BACKWARDCHAINER_H_ */
