/*Copyright (c) 2014 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/


/*Copyright (c) 2010 Rolf Andreassen

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "Object.h"
#include "Parser.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <assert.h>



Object::Object(string k) :
strVal(),
objects(),
leaf(false),
isObjList(false)
{
	key = k;
}


Object::~Object() {
	for (objiter i = objects.begin(); i != objects.end(); ++i)
	{
		delete (*i);
	}
	if (br == this)
	{
		br = 0;
	}
}


Object::Object(IObject* other) :
strVal(other->getLeaf()),
objects(),
leaf(other->isLeaf()),
isObjList(other->isList())
{
	key = other->getKey();
	for (vector<IObject*>::iterator i = other->getLeaves().begin(); i != other->getLeaves().end(); ++i)
	{
		objects.push_back(new Object(*i));
	}
}


void Object::setValue(string val)
{
	strVal = val;
	leaf = true;
}


void Object::setValue(Object* val)
{
	objects.push_back(val);
	leaf = false;
}


void Object::unsetValue(string val)
{
	for (unsigned int i = 0; i < objects.size(); ++i)
	{
		if (objects[i]->getKey() != val)
		{
			continue;
		}
		objects[i] = objects.back();
		objects.pop_back();
	}
}


void Object::setLeaf(string key, string val)
{
	Object* leaf = new Object(key);	// an object to hold the leaf
	leaf->setValue(val);
	setValue(leaf);
}


void Object::setValue(vector<IObject*> val)
{
	objects = val;
}


void Object::addToList(string val)
{
	isObjList = true;
	if (strVal.size() > 0)
	{
		strVal += " \"";
	}
	else
	{
		strVal += "\"";
	}
	strVal += val;
	strVal += "\"";
	tokens.push_back(val);
}


void Object::addToList(vector<string>::iterator begin, vector<string>::iterator end)
{
	isObjList = true;
	for (vector<string>::iterator itr = begin; itr != end; ++itr)
	{
		if (strVal.size() > 0)
		{
			strVal += "\" \"";
		}
		else
		{
			strVal += "\"";
		}
		strVal += *itr;
	}
	strVal += "\"";
	tokens.insert(tokens.end(), begin, end);
}


vector<IObject*> Object::getValue(string key) const
{
	vector<IObject*> ret;	// the objects to return
	for (vector<IObject*>::const_iterator i = objects.begin(); i != objects.end(); ++i)
	{
		if ((*i)->getKey() != key)
		{
			continue;
		}
		ret.push_back(*i);
	}
	return ret;
}


string Object::getToken(const int index)
{
	if (!isObjList)
	{
		return "";
	}
	if (index >= (int)tokens.size())
	{
		return "";
	}
	if (index < 0)
	{
		return "";
	}
	return tokens[index];
}


int Object::numTokens()
{
	if (!isObjList)
	{
		return 0;
	}
	return tokens.size();
}


vector<string> Object::getKeys() {
	vector<string> ret;	// the keys to return
	for (vector<IObject*>::iterator i = objects.begin(); i != objects.end(); ++i)
	{
		string curr = (*i)->getKey();	// the current key
		if (find(ret.begin(), ret.end(), curr) != ret.end())
		{
			continue;
		}
		ret.push_back(curr);
	}
	return ret;
}


string Object::getLeaf(string leaf) const
{
	vector<IObject*> leaves = getValue(leaf); // the objects to return
	if (0 == leaves.size())
	{
		cout << "Error: Cannot find leaf " << leaf << " in object " << endl << *this;
		assert(leaves.size());
	}
	return leaves[0]->getLeaf();
}

string Object::getTitle(string key) const
{
    return getStringOrDefault(key, [&](const IObject* obj) { return getTitleValue(obj); },
                                [&](const IObject* obj) { return string(); });
}

string Object::getTitleValue(const IObject* titleObj) const
{
    return titleObj->getLeafValueOrThisValue("title");
}

string Object::getLeafValueOrThisValue(string key) const
{
    auto leafGetter = [&](const IObject* obj) { return obj->getLeaf(); };
    return getStringOrDefault(key, leafGetter, leafGetter);
}

string Object::getStringOrDefault(string key, std::function<string(const IObject*)> valueGetter,
                                  std::function<string(const IObject*)> defaultValueGetter) const
{
    auto innerObj = this->getValue(key);
    return innerObj.empty() ? defaultValueGetter(this) : valueGetter(innerObj[0]);
}

string Object::print() const
{
	stringstream stringBuilder;

	static int indent = 0; // the level of indentation to output to
	for (int i = 0; i < indent; i++)
	{
		stringBuilder << "\t";
	}
	if (leaf) {
		stringBuilder << key << "=" << strVal << "\n";
		return stringBuilder.str();
	}
	if (isObjList)
	{
		stringBuilder << key << "={" << strVal << " }\n";
		return stringBuilder.str();
	}

	if (this != getTopLevel())
	{
		stringBuilder << key << "=\n";
		for (int i = 0; i < indent; i++)
		{
			stringBuilder << "\t";
		}
		stringBuilder << "{\n";
		indent++;
	}
	for (vector<IObject*>::const_iterator i = objects.begin(); i != objects.end(); ++i)
	{
		stringBuilder << *(*i);
	}
	if (this != getTopLevel())
	{
		indent--;
		for (int i = 0; i < indent; i++)
		{
			stringBuilder << "\t";
		}
		stringBuilder << "}\n";
	}

	return stringBuilder.str();
}

void Object::keyCount()
{
	if (leaf)
	{
		cout << key << " : 1\n";
		return;
	}

	map<string, int> refCount;	// the count of the references
	keyCount(refCount);
	vector<pair<string, int> > sortedCount; // an organized container for the counts
	for (map<string, int>::iterator i = refCount.begin(); i != refCount.end(); ++i)
	{
		pair<string, int> curr((*i).first, (*i).second);
		if (2 > curr.second)
		{
			continue;
		}
		if ((0 == sortedCount.size()) || (curr.second <= sortedCount.back().second))
		{
			sortedCount.push_back(curr);
			continue;
		}

		for (vector<pair<string, int> >::iterator j = sortedCount.begin(); j != sortedCount.end(); ++j)
		{
			if (curr.second < (*j).second)
			{
				continue;
			}
			sortedCount.insert(j, 1, curr);
			break;
		}
	}

	for (vector<pair<string, int> >::iterator j = sortedCount.begin(); j != sortedCount.end(); ++j)
	{
		cout << (*j).first << " : " << (*j).second << "\n";
	}
}


void Object::keyCount(map<string, int>& counter)
{
	for (vector<IObject*>::iterator i = objects.begin(); i != objects.end(); ++i)
	{
		counter[(*i)->getKey()]++;
		if ((*i)->isLeaf())
		{
			continue;
		}
		(*i)->keyCount(counter);
	}
}


void Object::printTopLevel()
{
	for (vector<IObject*>::iterator i = objects.begin(); i != objects.end(); ++i)
	{
		cout << (*i)->getKey() << endl;
	}
}


void Object::removeObject(Object* target)
{
	vector<IObject*>::iterator pos = find(objects.begin(), objects.end(), target);	// the position of the object to be removed
	if (pos == objects.end())
	{
		return;
	}
	objects.erase(pos);
}


void Object::addObject(Object* target)
{
	objects.push_back(target);
}

void Object::addObjectAfter(Object* target, string key)
{
	vector<IObject*>::iterator i;

	for (i = objects.begin(); i != objects.end(); ++i)
	{
		if ((*i)->getKey() == key)
		{
			objects.insert(i, target);
			break;
		}
	}

	if (i == objects.end())
	{
		objects.push_back(target);
	}
}



Object* br = 0;	// the branch being set
void setVal(string name, const string val, Object* branch)
{
	if ((branch) && (br != branch))
	{
		br = branch;
	}
	Object* b = new Object(name);	// the new object to add to the branch
	b->setValue(val);
	br->setValue(b);
}


void setInt(string name, const int val, Object* branch)
{
	if ((branch) && (br != branch))
	{
		br = branch;
	}
	static char strbuffer[1000];	// the text to add to the branch
	sprintf_s(strbuffer, 1000, "%i", val);
	Object* b = new Object(name);	// the new object to add to the branch
	b->setValue(strbuffer);
	br->setValue(b);
}


void setFlt(string name, const double val, Object* branch)
{
	if ((branch) && (br != branch))
	{
		br = branch;
	}
	static char strbuffer[1000];	// the text to add to the branch
	sprintf_s(strbuffer, 1000, "%.3f", val);
	Object* b = new Object(name);	// the new object to add to the branch
	b->setValue(strbuffer);
	br->setValue(b);
}

double Object::safeGetFloat(string k, const
	double def) {
	objvec vec = getValue(k);	// the objects with the keys to be returned
	if (0 == vec.size()) return def;
	return atof(vec[0]->getLeaf().c_str());
}

string Object::safeGetString(string k, string def)
{
	objvec vec = getValue(k);	// the objects with the strings to be returned
	if (0 == vec.size())
	{
		return def;
	}
	return vec[0]->getLeaf();
}

int Object::safeGetInt(string k, const int def)
{
	objvec vec = getValue(k);	// the objects with the ints to be returned
	if (0 == vec.size())
	{
		return def;
	}
	return atoi(vec[0]->getLeaf().c_str());
}

IObject* Object::safeGetObject(string k, IObject* def)
{
	objvec vec = getValue(k);	// the objects with the objects to be returned
	if (0 == vec.size())
	{
		return def;
	}
	return vec[0];
}


string Object::toString() const
{
	ostringstream blah;	// the output string
	blah << *(this);
	return blah.str();
}
