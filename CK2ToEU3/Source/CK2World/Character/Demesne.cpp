/*Copyright (c) 2013 The CK2 to EU3 Converter Project

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

#include "boost\foreach.hpp"
#include "CK2World\Character\Demesne.h"

namespace ck2
{
namespace character
{

Demesne::Demesne(std::vector<IObject*> demesneObj) : capitalString(""), primaryTitleString(""), armies(),
	navies()
{
	if (demesneObj.empty())
	{
		return;
	}

	std::vector<IObject*> capitalObj = demesneObj[0]->getValue("capital");
	if (!capitalObj.empty())
	{
		capitalString = capitalObj[0]->getLeaf();
	}

	primaryTitleString = demesneObj[0]->getTitle("primary");

	BOOST_FOREACH(IObject* armyObj, demesneObj[0]->getValue("army"))
	{
		armies.push_back(new CK2Army(static_cast<Object*>(armyObj)));
	}

	BOOST_FOREACH(IObject* navyObj, demesneObj[0]->getValue("navy"))
	{
		navies.push_back(new CK2Army(static_cast<Object*>(navyObj)));
	}
}

}  // namespace character
}  // namespace ck2
