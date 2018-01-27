#include "Editor.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

Editor::Editor()
{
	x=0;y=0;mode='n';
	status = "Normal Mode";
	filename = "untitled";

	/* Initializes buffer and appends line to
	prevent seg. faults */
	buff = new Buffer();
	buff->appendLine("");
}

Editor::Editor(string fn)
{
	x=0;y=0;mode='n';
	status = "Normal Mode";
	filename = fn;

	buff = new Buffer();

	ifstream infile(fn.c_str());
	
	if(infile.is_open())
	{
		while(!infile.eof())
		{
			string temp;
			getline(infile, temp);
			buff->appendLine(temp);
		}
	}
	else
	{
		cerr << "Cannot open file: '" << fn << "'\n";
		buff->appendLine("");
	}
}

void Editor::updateStatus()
{
	switch(mode)
	{
		case 'n':
			// Normal mode
			status = "Normal Mode";
			break;
		case 'i':
			// Insert mode
			status = "Insert Mode";
			break;
		case 'x':
			// Exiting
			status = "Exiting";
			break;
	}

	status += "\tCOL: " + tos(x) + "\tROW: " + tos(y);
}

string Editor::tos(int i)
{
	stringstream ss;
	ss << i;
	return ss.str();
}
void Editor::handleInput(int c)
{
	switch(c)
	{
		case KEY_LEFT:
			moveLeft();
			return;
		case KEY_RIGHT:
			moveRight();
			return;
		case KEY_UP:
			moveUp();
			return;
		case KEY_DOWN:
			moveDown();
			return;
	}

	switch(mode)
	{
		case 'n':
			if(c == 'x')
					mode = 'x';
			if(c == 'i')
					mode = 'i';
			if(c == 's')
					saveFile();
			break;
		case 'i':	
			if(c == 27) {
				handleEscape(); //returning to normal mode but bar not updating until next keypress. 
			} else if(c == 127 ||c == KEY_BACKSPACE) {
				handleBckspce();
			} else if(c == KEY_DC) {
				handleDelete();
			} else if(c == KEY_ENTER || c == 10) {
				handleEnter();
			}
			else if(c == KEY_BTAB || c == KEY_CTAB || c == KEY_STAB || c == KEY_CATAB || c == 9) {
				handleTab();
			} else {
				buff->lines[y].insert(x, 1, char(c));	
				x++;
			}
			break;
	}
}
void Editor::handleEscape()
{
	mode = 'n';
}
void Editor::handleBckspce()
{
	// The Backspace key
	if(x == 0 && y > 0)
	{
		x = buff->lines[y-1].length();
		// Bring the line down
		buff->lines[y-1] += buff->lines[y];
		// Delete the current line
		deleteLine();
		moveUp();
	}
	else
	{
		// Removes a character
		buff->lines[y].erase(--x, 1);
	}
				
}
void Editor::handleDelete()
{
// The Delete key
	if(x == buff->lines[y].length() && y != buff->lines.size() - 1)
	{
		// Bring the line down
		buff->lines[y] += buff->lines[y+1];
		// Delete the line
		deleteLine(y+1);
	}
	else
	{
		buff->lines[y].erase(x, 1);
	}
}
void Editor::handleEnter()
{
	// The Enter key
	// Bring the rest of the line down
	if(x < buff->lines[y].length())
	{
		// Put the rest of the line on a new line
		buff->insertLine(buff->lines[y].substr(x, buff->lines[y].length() - x), y + 1);
		// Remove that part of the line
		buff->lines[y].erase(x, buff->lines[y].length() - x);
	}
	else
	{
		buff->insertLine("", y+1);
	}
	x = 0;
	moveDown();
				
}
void Editor::handleTab()
{
	// The Tab key
	buff->lines[y].insert(x, 4, ' ');
	x += 4;
				
		
}
void Editor::moveLeft()
{
	if(x-1 >= 0)
	{
		x--;
		move(y, x);
	}
}

void Editor::moveRight()
{
	if(x+1 < COLS && x+1 <= buff->lines[y].length())
	{
		x++;
		move(y, x);
	}
}

void Editor::moveUp()
{
	if(y-1 >= 0)
		y--;
	if(x >= buff->lines[y].length())
		x = buff->lines[y].length();
	move(y, x);
}

void Editor::moveDown()
{
	if(y+1 < LINES-1 && y+1 < buff->lines.size())
		y++;
	if(x >= buff->lines[y].length())
		x = buff->lines[y].length();
	move(y, x);
}

void Editor::printBuff()
{
	for(int i=0; i<LINES-1; i++)
	{
		if(i >= buff->lines.size())
		{
			move(i, 0);
			clrtoeol();
		}
		else
		{
			mvprintw(i, 0, buff->lines[i].c_str());
		}
		clrtoeol();
	}
	move(y, x);
}

void Editor::printStatusLine()
{
	attron(A_REVERSE);
	mvprintw(LINES-1, 0, status.c_str());
	clrtoeol();
	attroff(A_REVERSE);
}

void Editor::deleteLine()
{
	buff->removeLine(y);
}

void Editor::deleteLine(int i)
{
	buff->removeLine(i);
}

void Editor::saveFile()
{
	if(filename == "")
	{
		// Set filename to untitled
		filename = "untitled";
	}

	ofstream f(filename.c_str());
	if(f.is_open())
	{
		for(int i=0; i<buff->lines.size(); i++)
		{
			f << buff->lines[i] << endl;
		}
		status = "Saved to file!";
	}
	else
	{
		status = "Error: Cannot open file for writing!";
	}
	f.close();
}

