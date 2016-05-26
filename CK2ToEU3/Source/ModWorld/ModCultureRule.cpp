#include "ModCultureRule.h"
#include <boost/tokenizer.hpp>
#include <algorithm>
#include <string>
#include <sstream>
#include "..\Log.h"
#include "..\Parsers\Object.h"

using namespace boost;

#define GROUPING_SYMBOL '*'
vector<string> ModCultureRule::processRawNames(vector<IObject*> nameListObj)
{
	vector<string> nameList;
	string rawNames, name, temp;
	stringstream ss;
	typedef tokenizer<char_separator<char> > 
		tokenizer;
	char_separator<char> sep(" ");
	char first,last;
	bool group = false;
	if (nameListObj.size() > 0)
	{
		rawNames	= nameListObj[0]->getLeaf();
		tokenizer tokens(rawNames, sep);
		for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
		{
			name=tok_iter->c_str();
			first = name.front();
			last = name.back();
			if( (first==GROUPING_SYMBOL) && (last==GROUPING_SYMBOL) ) // Entry: "name"
			{
				if (nameListObj[0]->getKey() == "leader_names" || nameListObj[0]->getKey() == "ship_names" || nameListObj[0]->getKey() == "army_names" || nameListObj[0]->getKey() == "fleet_names")
				{
					replace(name.begin(), name.end(), GROUPING_SYMBOL,'"');
				}
				else
				{
					name.erase(remove(name.begin(), name.end(), GROUPING_SYMBOL), name.end()); // Strip grouping symbol
				}
				nameList.push_back(name);
			}
			else if( (first==GROUPING_SYMBOL) && (last!=GROUPING_SYMBOL) ) // Entry: "First...
			{
				ss << name;
				group = true;
			}
			else if(last==GROUPING_SYMBOL) // Entry: ...third"
			{
				ss << " " << name;
				temp = ss.str();
				ss.str(string());
				if (nameListObj[0]->getKey() == "leader_names" || nameListObj[0]->getKey() == "ship_names" || nameListObj[0]->getKey() == "army_names" || nameListObj[0]->getKey() == "fleet_names")
				{
					replace(temp.begin(), temp.end(), GROUPING_SYMBOL,'"');
				}
				else
				{
					temp.erase(remove(temp.begin(), temp.end(), GROUPING_SYMBOL), temp.end()); // Strip grouping symbol
				}
				nameList.push_back(temp);
				temp = "";
				group = false;
			}
			else if(group==true) // Entry: ...second...
			{
				ss << " " << name;
			}
			else // Entry: name
			{
				nameList.push_back(name);
			}
		}
	}
	return nameList;
}

ModCultureRule::ModCultureRule(string _key, Object* obj)
{
	key			= _key;

	vector<IObject*> graphicalCultureObj = obj->getValue("graphical_culture");
	if (graphicalCultureObj.size() > 0)
	{
		graphicalCulture	= graphicalCultureObj[0]->getLeaf();
	}
	else
	{
		graphicalCulture	= DEFAULT_GFX;
	}

	#define DEFAULT_GC "Generic"
	vector<IObject*> graphicalCultureV2Obj = obj->getValue("graphical_culture_V2");
	if (graphicalCultureV2Obj.size() > 0)
	{
		graphicalCultureV2	= graphicalCultureV2Obj[0]->getLeaf();
	}
	else
	{
		graphicalCultureV2	= DEFAULT_GC;
	}

	#define DEFAULT_BASE_V2 "CAN"
	vector<IObject*> baseCountryV2Obj = obj->getValue("base_country_V2");
	if (graphicalCultureV2Obj.size() > 0)
	{
		baseCountryV2	= baseCountryV2Obj[0]->getLeaf();
	}
	else
	{
		baseCountryV2	= DEFAULT_BASE_V2;
	}

	maleNames.clear();
	vector<IObject*> maleNameObj = obj->getValue("male_names");
	if (maleNameObj.size() > 0)
	{
		maleNames = processRawNames(maleNameObj);
	}
	

	femaleNames.clear();
	vector<IObject*> femaleNameObj = obj->getValue("female_names");
	if (femaleNameObj.size() > 0)
	{
		femaleNames = processRawNames(femaleNameObj);
	}

	leaderNames.clear();
	vector<IObject*> leaderNameObj = obj->getValue("leader_names");
	if (leaderNameObj.size() > 0)
	{
		leaderNames = processRawNames(leaderNameObj);
	}

	shipNames.clear();
	vector<IObject*> shipNameObj = obj->getValue("ship_names");
	if (shipNameObj.size() > 0)
	{
		shipNames = processRawNames(shipNameObj);
	}

	armyNames.clear();
	vector<IObject*> armyNameObj = obj->getValue("army_names");
	if (armyNameObj.size() > 0)
	{
		armyNames = processRawNames(armyNameObj);
	}

	fleetNames.clear();
	vector<IObject*> fleetNameObj = obj->getValue("fleet_names");
	if (fleetNameObj.size() > 0)
	{
		fleetNames = processRawNames(fleetNameObj);
	}
	
}

