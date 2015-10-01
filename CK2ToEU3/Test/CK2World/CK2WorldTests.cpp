/*Copyright (c) 2015 The Paradox Game Converters Project

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

#include <map>
#include "LogBase.h"
#include "Parsers\Object.h"
#include "Mocks\LoggerMock.h"
#include "CK2World\CK2Title.h"
#include "CK2World\CK2World.h"
#include "CK2World\Character\CK2Character.h"

using namespace testing;

namespace ck2
{
namespace unittests
{	
	
class CK2WorldShould : public Test
{
protected:
	CK2WorldShould() : titleName("k_sample"), deJureLIegeTitleName("e_sample"), color(),
		world(new CK2World(boost::make_shared<LoggerMock>()))
	{
	}

	virtual void SetUp()
	{
		sampleDeJureLiege = new CK2Title(deJureLIegeTitleName, color);
		sampleDeJureLieges.insert(make_pair(deJureLIegeTitleName, sampleDeJureLiege));
		newTitle = new CK2Title(titleName, color);
		newTitle->setDeJureLiege(sampleDeJureLieges);

		world->addTitle(std::make_pair(titleName, newTitle));
	}

	virtual void TearDown()
	{
		delete newTitle;
		delete sampleDeJureLiege;
	}

	int color[3];
	string titleName;
	string deJureLIegeTitleName;
	title_map_t sampleDeJureLieges;
	CK2Title* sampleDeJureLiege;
	CK2Title* newTitle;
	boost::shared_ptr<CK2World> world;
};

TEST_F(CK2WorldShould, RemoveTitlesWithoutCurrentHolderAndHistory)
{
	TitleFilter(&(*world)).removeDeadTitles();

	ASSERT_THAT(world->getAllTitles()[titleName], IsNull());
}

TEST_F(CK2WorldShould, NotFailWhilePassingUsedTitleToWorld)
{
	CK2Character* holder = new CK2Character();
	newTitle->setHolder(holder);

	TitleFilter(&(*world)).removeDeadTitles();

	ASSERT_THAT(world->getAllTitles()[titleName], NotNull());
	delete holder;
}

} // namespace unittests
} // namespace ck2