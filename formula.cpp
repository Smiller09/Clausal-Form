
#include "formula.h"

// ========== STATIC FUNCTIONS ===========

// PRIVATE

/**
* Replaces binary operators into AND or OR equivalent form
*/
Formula * Formula::replaceBinaryOperators(Formula * formula)
{
	Formula * result = formula;
	Formula * left;
	Formula * right;
	int type = formula->getType();
	
	if (type == Formula::IMPLICATION)
	{
		// Replaces the conditional statements to the form (¬a OR b) AND (¬b OR A)
		left = replaceBinaryOperators(formula->getLeft());
		right = replaceBinaryOperators(formula->getRight());
		
		Formula * negatedLeft = new Formula(left);
		result = new Formula(negatedLeft, right, Formula::OR);
	}
	else if (type == Formula::BICONDITIONAL)
	{
		// Replaces the biconditional to the form (a=>b) AND (b=>a)
		// and then replaces the conditional statements to the form (¬a OR b) AND (¬b OR A)
		left = replaceBinaryOperators(formula->getLeft());
		right = replaceBinaryOperators(formula->getRight());
		
		Formula * newLeft = replaceBinaryOperators(new Formula(left, right, Formula::IMPLICATION));
		Formula * newRight = replaceBinaryOperators(new Formula(right, left, Formula::IMPLICATION));
		result = new Formula(newLeft, newRight, Formula::AND);
	}
	else if (type == Formula::NEGATION)
	{
		// Creates a new negated formula with the inside formula expanded to include only AND or OR
		result = new Formula(replaceBinaryOperators(formula->getFormula()));
	}
	
	return result;
}

/**
* Drives in the negation
*/
Formula * Formula::driveInNegation(Formula * formula)
{
	Formula * result = formula;
	Formula * left = NULL;
	Formula * right = NULL;
	int type = formula->getType();
	
	if (type == Formula::NEGATION)
	{
		Formula * insideFormula = formula->getFormula();
		Formula * negatedLeft;
		Formula * negatedRight;
		
		if (insideFormula->getType() == Formula::ATOM)
		{
			result = new Formula(insideFormula);
		}
		else if (insideFormula->getType() == Formula::AND)
		{
			negatedLeft = new Formula(insideFormula->getLeft());
			negatedRight = new Formula(insideFormula->getRight());
			result = new Formula(driveInNegation(negatedLeft), driveInNegation(negatedRight), Formula::OR);
		}
		else if (insideFormula->getType() == Formula::OR)
		{
			negatedLeft = new Formula(insideFormula->getLeft());
			negatedRight = new Formula(insideFormula->getRight());
			result = new Formula(driveInNegation(negatedLeft), driveInNegation(negatedRight), Formula::AND);
		}
	}
	else if (type == Formula::OR)
	{
		left = formula->getLeft();
		right = formula->getRight();
		result = new Formula(driveInNegation(left), driveInNegation(right), Formula::OR);
	}
	else if (type == Formula::AND)
	{
		left = formula->getLeft();
		right = formula->getRight();
		result = new Formula(driveInNegation(left), driveInNegation(right), Formula::AND);
	}
	return result;
}

/**
* Removes double negations
*/
Formula * Formula::removeDoubleNegation(Formula * formula)
{
	Formula * result = formula;
	Formula * left;
	Formula * right;
	int type = formula->getType();
	
	if (type == Formula::AND)
	{
		left = removeDoubleNegation(formula->getLeft());
		right = removeDoubleNegation(formula->getRight());
		result = new Formula(left, right, Formula::AND);
	}
	else if (type == Formula::OR)
	{
		left = removeDoubleNegation(formula->getLeft());
		right = removeDoubleNegation(formula->getRight());
		result = new Formula(left, right, Formula::OR);
	}
	else if (type == Formula::NEGATION)
	{
		if (formula->getFormula()->getType() == Formula::NEGATION)
		{
			result = formula->getFormula()->getFormula();
		}
	}
	return result;
}

/**
* Distribute the OR over the AND
* @param formula	Formula to distribute
* @return distributed formula
*/
Formula * Formula::distributeORoverAND(Formula * formula)
{
	Formula * result = formula;
	int type = formula->getType();
	Formula * left = formula->getLeft();
	Formula * right = formula->getRight();
	
	// If the formula is of the form (p OR q) then check whether there are any ANDs inside of p or q
	if (type == Formula::OR)
	{
		// If the right formula is an atom or the negation and the left formula is an AND, the distribute it according to (p OR (q AND r)) = (p OR q) AND (p OR r)
		if ( (right->getType() == Formula::ATOM || right->getType() == Formula::NEGATION) && left->getType() == Formula::AND)
		{
			Formula * leftOr = new Formula(right, left->getLeft(), Formula::OR);
			Formula * rightOr = new Formula(right, left->getRight(), Formula::OR);
			result = new Formula(leftOr, rightOr, Formula::AND);
		}
		// This one is analogous to the one above but instead of checking if the right formula is an atom, it checks whether the left formula is an atom
		else if ( (left->getType() == Formula::ATOM || left->getType() == Formula::NEGATION ) && right->getType() == Formula::AND)
		{
			Formula * leftOr2 = new Formula(left, right->getLeft(), Formula::OR);
			Formula * rightOr2 = new Formula(left, right->getRight(), Formula::OR);
			result = new Formula(leftOr2, rightOr2, Formula::AND);
		}
		
		// If both formulas are ANDs (p AND q) OR (r AND s) then transform to (p OR (r AND s)) AND (q OR (r AND s))
		// Then call the distribution again to get the full distribution like (p OR r) AND (p OR s) AND (q OR r) AND (q OR s)
		else if ( left->getType() == Formula::AND && right->getType() == Formula::AND)
		{
			Formula * form1 = distributeORoverAND(new Formula(left->getLeft(), right, Formula::OR));
			Formula * form2 = distributeORoverAND(new Formula(left->getRight(), right, Formula::OR));
			result = new Formula(form1, form2, Formula::AND);
		}
		
		// If the left formula is an AND and the right formula is an OR
		else if ( left->getType() == Formula::AND && right->getType() == Formula::OR)
		{
			Formula * form3 = distributeORoverAND(right); // AQUI el 1
			Formula * form3_1 = distributeORoverAND(new Formula(left->getLeft(), form3, Formula::OR));
			Formula * form3_2 = distributeORoverAND(new Formula(left->getRight(), form3, Formula::OR));
			result = distributeORoverAND(new Formula(form3_1, form3_2, Formula::AND));
		}
		
		// If the right formula is an AND and the left formula is an OR
		else if ( right->getType() == Formula::AND && left->getType() == Formula::OR)
		{
			Formula * form4 = distributeORoverAND(left);
			Formula * form4_1 = distributeORoverAND(new Formula(right->getLeft(), form4, Formula::OR));
			Formula * form4_2 = distributeORoverAND(new Formula(right->getRight(), form4, Formula::OR));
			result = distributeORoverAND(new Formula(form4_1, form4_2, Formula::AND));
		}
	}
	else if (type == Formula::AND)
	{
		Formula * newLeft = distributeORoverAND(left);
		Formula * newRight = distributeORoverAND(right);
		result = new Formula(newLeft, newRight, Formula::AND);
	}
	return result;
}


// PUBLIC 

/**
* Converts to Conjunctive Normal Form
* @param formula 	Formula to convert to CNF
* @return formula in CNF
*/
Formula * Formula::conjunctiveNormalForm(Formula * formula)
{
	return Formula::distributeORoverAND(Formula::removeDoubleNegation(Formula::driveInNegation(Formula::replaceBinaryOperators(formula))));
}


// ======= END OF STATIC FUNCTIONS =======

// ========== REGULAR METHODS ============

Formula::Formula(Formula * left, Formula * right, int type)
{
	this->left= left;
	this->right = right;
	this->type = type;
	this->atom = "";
	this->formula = NULL;
}

Formula::Formula(Formula * formula)
{
	this->formula = formula;
	this->type = Formula::NEGATION;
	this->left = NULL;
	this->right = NULL;
	this->atom = "";
}

Formula::Formula(string atom)
{
	this->atom = atom;
	this->left = NULL;
	this->right = NULL;
	this->type = Formula::ATOM;
	this->formula = NULL;
}

int Formula::getType()
{
	return type;
}

Formula * Formula::getLeft()
{
	return left;
}

Formula * Formula::getRight()
{
	return right;
}

Formula * Formula::getFormula()
{
	return formula;
}

string Formula::getAtom()
{
	return atom;
}

void Formula::print()
{
	switch(type)
	{
		case Formula::AND:
		{
			if (right != NULL)
			{
				cout << "(";
				left->print();
				cout << " & ";
				right->print();
				cout << ")";
			}
			else
			{
				cout << "(";
				left->print();
				cout << ")";
			}
			break;
		}
		case Formula::OR:
		{
			if (right != NULL)
			{
				cout << "(";
				left->print();
				cout << " | ";
				right->print();
				cout << ")";
			}
			else
			{
				cout << "(";
				left->print();
				cout << ")";
			}
			break;
		}
		case Formula::IMPLICATION:
		{
			if (right != NULL)
			{
				cout << "(";
				left->print();
				cout << " => ";
				right->print();
				cout << ")";
			}
			else
			{
				cout << "(";
				left->print();
				cout << ")";
			}
			break;
		}
		case Formula::BICONDITIONAL:
		{
			if (right != NULL)
			{
				cout << "(";
				left->print();
				cout << " <=> ";
				right->print();
				cout << ")";
			}
			else
			{
				cout << "(";
				left->print();
				cout << ")";
			}
			break;
		}
		case Formula::NEGATION:
			cout << "~";
			formula->print();
		break;
		case Formula::ATOM:
			cout << atom;
		break;
	}
}

void Formula::printC()
{
	switch(type)
	{
		case Formula::AND:
		{
			if (right != NULL)
			{
				left->printC();
				cout << " , ";
				right->printC();
			}
			else
			{
				left->printC();
			}
			break;
		}
		case Formula::OR:
		{
			if (right != NULL)
			{
				left->printC();
				cout << "";
				right->printC();
			}
			else
			{
				left->printC();
			}
			break;
		}
		case Formula::NEGATION:
			cout << "~";
			formula->printC();
		break;
		case Formula::ATOM:
			cout << atom;
		break;
	}
}

void Formula::printToFile(ofstream * outputFile)
{
	switch(type)
	{
		case Formula::AND:
		{
			if (right != NULL)
			{
				* outputFile << "(";
				left->printToFile(outputFile);
				* outputFile << " & ";
				right->printToFile(outputFile);
				* outputFile << ")";
			}
			else
			{
				* outputFile << "(";
				left->printToFile(outputFile);
				* outputFile << ")";
			}
			break;
		}
		case Formula::OR:
		{
			if (right != NULL)
			{
				* outputFile << "(";
				left->printToFile(outputFile);
				* outputFile << " | ";
				right->printToFile(outputFile);
				* outputFile << ")";
			}
			else
			{
				* outputFile << "(";
				left->printToFile(outputFile);
				* outputFile << ")";
			}
			break;
		}
		case Formula::IMPLICATION:
		{
			if (right != NULL)
			{
				* outputFile << "(";
				left->printToFile(outputFile);
				* outputFile << " => ";
				right->printToFile(outputFile);
				* outputFile << ")";
			}
			else
			{
				* outputFile << "(";
				left->printToFile(outputFile);
				* outputFile << ")";
			}
			break;
		}
		case Formula::BICONDITIONAL:
		{
			if (right != NULL)
			{
				* outputFile << "(";
				left->printToFile(outputFile);
				* outputFile << " <=> ";
				right->printToFile(outputFile);
				* outputFile << ")";
			}
			else
			{
				* outputFile << "(";
				left->printToFile(outputFile);
				* outputFile << ")";
			}
			break;
		}
		case Formula::NEGATION:
			* outputFile << "~";
			formula->printToFile(outputFile);
		break;
		case Formula::ATOM:
			* outputFile << atom;
		break;
	}
}
