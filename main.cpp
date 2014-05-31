#include <iostream>
#include <string>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <sstream>
#include "formula.h"

using namespace std;
// BEGINNING OF APPLICATION
// ==========================================================

// Converts a character to a string
string charToStr(char c)
{
	stringstream ss;
	string s;
	ss << c;
	ss >> s;
	return s;
}

/**
* Removes right parentheses
* @param text 	string contaning a well formed formula
*/
void removeRightParentheses(string * text)
{
	// Remove the right parentheses
	bool rightParentheses = true;
	while (rightParentheses)
	{
		if (text->size() > 0)
		{
			if (text->at(0) == ')')
				text->erase(0, 1);
			else
				rightParentheses = false;
		}
		else
			rightParentheses = false;
	}
}

/**
* Converts the string into a Formula representation
* @param text	string contaning a well formed formula
* @return Formula
*/
Formula * parse(string * text)
{
	Formula * result = NULL;
	Formula * left = NULL;
	Formula * right = NULL;

	if (text->size() >= 1)
	{
		if ( text->at(0) == '(' )
		{
			text->erase(0, 1);
			left = parse(text);
			if ( text->at(0) == '&' )
			{
				text->erase(0, 1);
				right = parse(text);
				return new Formula(left, right, Formula::AND);
			}
			else if ( text->at(0) == '|' )
			{
				text->erase(0, 1);
				right = parse(text);
				return new Formula(left, right, Formula::OR);
			}
			else if ( text->at(0) == '>' )
			{
				text->erase(0, 1);
				right = parse(text);
				return new Formula(left, right, Formula::IMPLICATION);
			}
			else if ( text->at(0) == '=' )
			{
				text->erase(0, 1);
				right = parse(text);
				return new Formula(left, right, Formula::BICONDITIONAL);
			}
			return new Formula(left, right, Formula::AND);
		}
		else if ( text->at(0) != '(' && text->at(0) != '&' && text->at(0) != '|' && text->at(0) != '>' && text->at(0) != '=' && text->at(0) != '~' )
		{
			result = new Formula(charToStr(text->at(0)));
			text->erase(0, 1);
			removeRightParentheses(text);

		}
		else if (text->at(0) == '~')
		{
			text->erase(0, 1);
			result = new Formula(parse(text));
			removeRightParentheses(text);
		}
	}
	return result;
}


/**
* Application's Point of Entry
*/
int main(int argc, char ** argv)
{
	ifstream inputFile;
	ofstream outputFile;
	vector<string> text_file;
	vector<string> text_file_2;

	inputFile.open("input.txt");

	// Create an output file
	outputFile.open("output.txt");

	if (!inputFile)
    {
		// File was not able to be opened
        cerr << "Unable to open file" << endl;
        exit(1);
    }

    string temp;


    while( getline( inputFile, temp ) )
    {
     	text_file.push_back( temp );
     	text_file_2.push_back( temp );
 	}

	cout << endl;
	cout << "======= Clausal Form Converter =======" << endl;
	cout << endl;
	cout << "Input: " << text_file[0] << endl;
	Formula * parsed = parse(&text_file[0]);
	cout << "Formula: ";
	parsed->print();
	cout<<endl;
	parsed = Formula::conjunctiveNormalForm(parsed);

	cout << endl << endl;
	cout << "Output:" << endl << endl;
	cout << "CNF: ";
	parsed->print();
	cout<<""<<endl;
	cout<<"Clausal: {";
	parsed->printC();
	cout <<"}"<< endl;



	// Output file
	outputFile << "CNF: ";
	parsed->printToFile(&outputFile);
	outputFile << endl;

	system ("PAUSE");

	return 0;
}
