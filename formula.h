#ifndef _FORMULA_H
#define _FORMULA_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

class Formula
{
public:

	static const int ATOM = 0;
	static const int NEGATION = 1;
	static const int AND = 2;
	static const int OR = 3;
	static const int IMPLICATION = 4;
	static const int BICONDITIONAL = 5;
	
	/**
	* Converts to Conjunctive Normal Form
	* @param formula 	Formula to convert to CNF
	* @return formula in CNF
	*/
	static Formula * conjunctiveNormalForm(Formula * formula);
	
	/**
	* Converts to Disjunctive Normal Form
	* @param formula 	Formula to convert to DNF
	* @return formula in DNF
	*/
//	static Formula * disjunctiveNormalForm(Formula * formula);
	
	/**
	* Constructor used when the formula is AND or OR or IMPLICATION or BICONDITIONAL
	* @param left	Left formula
	* @param right	Right formula
	* @param type	Type of formula (AND, OR, IMPLICATION, BICONDITIONAL)
	*/
	Formula(Formula * left, Formula * right, int type);
	/**
	* Constructor used when the formula is a NEGATION
	* @param formula	Formula to negate
	*/
	Formula(Formula * formula);
	/**
	* Constructor used when the formula is and atom
	* @param atom	Atom
	*/
	Formula(string atom);
	
	/**
	* Gets the formula's type
	* @return int	Formula's type (ATOM, NEGATION, AND, OR, IMPLICATION, BICONDITIONAL)
	*/
	int getType();
	
	/**
	* Gets the left formula
	* @return formula	Left formula
	*/
	Formula * getLeft();
	
	/**
	* Gets the right formula
	* @return formula	Right formula
	*/
	Formula * getRight();
	
	/**
	* Gets the inner formula from a negated formula
	* @return formula	The formula that is negated
	*/
	Formula * getFormula();
	
	/**
	* Gets the atom
	* @return string	The atom in string form
	*/
	string getAtom();
	
	/**
	* Prints the CNF  formula to the console
	*/
	void print();
	
	/**
	* Prints  the CLAUSAL formula to the console
	*/
	void printC();
	
	/**
	* Prints to the specified file
	* @return ofstream 	Pointer to the output file
	*/
	void printToFile(ofstream * outputFile);
	
private:

	int type;
	Formula * left;
	Formula * right;
	Formula * formula;
	string atom;
	
	static Formula * replaceBinaryOperators(Formula * formula);
	static Formula * driveInNegation(Formula * formula);
	static Formula * removeDoubleNegation(Formula * formula);
	static Formula * distributeORoverAND(Formula * formula);
	//static Formula * distributeANDoverOR(Formula * formula);

};

#endif
