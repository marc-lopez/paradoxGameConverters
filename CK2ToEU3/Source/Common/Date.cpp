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


/*Copyright (c) 2010 The EU3 to V2 Converter Project

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



#include "Date.h"
#include <cstdlib>
#include <sstream>
#include <vector>
#include "Parsers\Object.h"
using namespace std;

namespace common
{

date::date(string _init)
{
	if (_init.length() < 1)
		return;

	if (_init[0] == '\"')
		_init = _init.substr(1, _init.length() - 2);
	int first_dot = _init.find_first_of('.');
	int last_dot = _init.find_last_of('.');
	year	= atoi( _init.substr(0, first_dot).c_str() );
	month	= atoi( _init.substr(first_dot + 1, last_dot - first_dot).c_str() );
	day		= atoi( _init.substr(last_dot + 1, 2).c_str() );
}

date::date(const date& _init)
{
	year = _init.year;
	month = _init.month;
	day = _init.day;
}

date& date::operator=(const date& _rhs)
{
	year = _rhs.year;
	month = _rhs.month;
	day = _rhs.day;
	return *this;
}

date::date(const Object* _init)
{
	vector<IObject*> dateSubObj = _init->getValue("year");
	if (dateSubObj.size() > 0)
	{
		// date specified by year=, month=, day=
		year = atoi(_init->getLeaf("year").c_str());
		month = atoi(_init->getLeaf("month").c_str());
		day = atoi(_init->getLeaf("day").c_str());
	}
	else
	{
		// date specified by year.month.day
		// build another date object via date(string&) and copy it to this one
		(*this) = date(_init->getLeaf());
	}
}

bool date::operator==(const date& _rhs) const
{
	return ((year == _rhs.year)
		 && (month == _rhs.month)
		 && (day == _rhs.day));
}

bool date::operator!=(const date& _rhs) const
{
	return !(*this == _rhs);
}

bool date::operator<(const date& _rhs) const
{
	return ((year < _rhs.year)
		|| ((year == _rhs.year) && (month < _rhs.month))
		|| ((year == _rhs.year) && (month == _rhs.month) && (day < _rhs.day)));
}

bool date::operator>(const date& _rhs) const
{
	return ((year > _rhs.year)
		|| ((year == _rhs.year) && (month > _rhs.month))
		|| ((year == _rhs.year) && (month == _rhs.month) && (day > _rhs.day)));
}

bool date::operator<=(const date& _rhs) const
{
	return ((*this == _rhs) || (*this < _rhs));
}

bool date::operator>=(const date& _rhs) const
{
	return ((*this == _rhs) || (*this > _rhs));
}

float date::diffInYears(const date& _rhs) const
{
	float years = float(year - _rhs.year);

	int lhsDays;
	switch (month)
	{
		case 1:
			lhsDays = day;
			break;
		case 2:
			lhsDays = day + 31;
			break;
		case 3:
			lhsDays = day + 59;
			break;
		case 4:
			lhsDays = day + 90;
			break;
		case 5:
			lhsDays = day + 120;
			break;
		case 6:
			lhsDays = day + 151;
			break;
		case 7:
			lhsDays = day + 181;
			break;
		case 8:
			lhsDays = day + 212;
			break;
		case 9:
			lhsDays = day + 243;
			break;
		case 10:
			lhsDays = day + 273;
			break;
		case 11:
			lhsDays = day + 304;
			break;
		case 12:
			lhsDays = day + 334;
			break;
	}

	int rhsDays;
	switch (month)
	{
		case 1:
			rhsDays = day;
			break;
		case 2:
			rhsDays = day + 31;
			break;
		case 3:
			rhsDays = day + 59;
			break;
		case 4:
			rhsDays = day + 90;
			break;
		case 5:
			rhsDays = day + 120;
			break;
		case 6:
			rhsDays = day + 151;
			break;
		case 7:
			rhsDays = day + 181;
			break;
		case 8:
			rhsDays = day + 212;
			break;
		case 9:
			rhsDays = day + 243;
			break;
		case 10:
			rhsDays = day + 273;
			break;
		case 11:
			rhsDays = day + 304;
			break;
		case 12:
			rhsDays = day + 334;
			break;
	}

	years += (lhsDays - rhsDays) / 365;

	return years;
}

bool date::isSet() const
{
	date default_date;
	return (*this != default_date);
}

string date::toString() const
{
    stringstream builder;
    builder << year << DATE_SEPARATOR << month << DATE_SEPARATOR << day;
	return builder.str();
}

constexpr char date::DATE_SEPARATOR[];

} // namespace common
