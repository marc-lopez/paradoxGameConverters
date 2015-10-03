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
#include "Configuration.h"
#include "Parsers\Object.h"
#include "Mocks\ObjectMock.h"
#include "CK2World\Character\CK2Character.h"
#include "CK2World\CK2Title.h"

using namespace testing;

namespace ck2
{
namespace unittests
{
namespace character
{

using namespace mocks;

class CK2CharacterShould : public Test
{
protected:
	CK2CharacterShould() : SAMPLE_TITLE_NAME("e_sample"), DEMESNE_KEY("demesne")
	{
	}

	std::vector<IObject*> getSampleDemsneData()
	{
	    auto TITLE_INNER_KEY = "title";
	    auto PRIMARY_TITLE_KEY = "primary";

		Object *primaryTitleInnerObj = new Object(TITLE_INNER_KEY);
		primaryTitleInnerObj->setValue(SAMPLE_TITLE_NAME);
		Object *primaryTitleObj = new Object(PRIMARY_TITLE_KEY);
		primaryTitleObj->setValue(primaryTitleInnerObj);
		Object *demesneObj = new Object(DEMESNE_KEY);
		demesneObj->setValue(primaryTitleObj);
		std::vector<IObject*> demesneCollection;
		demesneCollection.push_back(demesneObj);

		return demesneCollection;
	}

	const std::string SAMPLE_TITLE_NAME;
	const std::string DEMESNE_KEY;
};

TEST_F(CK2CharacterShould, SetVersion2Point2SaveFormatPrimaryTitle)
{
	map<int, CK2Dynasty*> dynasties;
	map<int, CK2Trait*> traits;

	int color[3];
	CK2Title* sampleTitle = new CK2Title(SAMPLE_TITLE_NAME, color);

	map<string, CK2Title*> titleMap;
	titleMap.insert(std::pair<string, CK2Title*>(SAMPLE_TITLE_NAME, sampleTitle));

	std::vector<IObject*> demesneData = getSampleDemsneData();

	ObjectMock *configurationMock = new ObjectMock();
	ObjectMock *saveDataMock = new ObjectMock();

	EXPECT_CALL(*configurationMock, getLeaf(_)).WillRepeatedly(Return(std::string()));
	EXPECT_CALL(*saveDataMock, getKey()).WillRepeatedly(Return(std::string()));
	EXPECT_CALL(*saveDataMock, getLeaf(_)).WillRepeatedly(Return(std::string()));
	EXPECT_CALL(*saveDataMock, getValue(_)).WillRepeatedly(Return(std::vector<IObject*>()));
	EXPECT_CALL(*saveDataMock, getValue(DEMESNE_KEY)).WillRepeatedly(Return(demesneData));

	Configuration::setConfiguration(configurationMock);
	CK2Character* sampleCharacter = new CK2Character(saveDataMock, dynasties, traits, date());
	sampleTitle->setHolder(sampleCharacter);
	sampleCharacter->setPrimaryTitle(titleMap);
	CK2Title* calculatedPrimaryTitle = sampleCharacter->getPrimaryTitle();

	ASSERT_THAT(calculatedPrimaryTitle, NotNull());
	ASSERT_EQ(SAMPLE_TITLE_NAME, calculatedPrimaryTitle->getTitleString());

	delete sampleTitle;
	delete sampleCharacter;
	delete demesneData[0];
	delete configurationMock;
	delete saveDataMock;
}

} // namespace character
} // namespace unittests
} // namespace ck2
