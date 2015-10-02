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

#include <vector>
#include "Parsers/Object.h"
#include "Mocks/ObjectMock.h"
#include "CK2World/CK2Title.h"

using namespace testing;

namespace unittests
{
namespace ck2
{

using namespace mocks;

class CK2TitleShould : public Test
{
protected:
	CK2TitleShould()
	{
	}

	virtual void SetUp()
	{
	    titleDataMock = new ObjectMock();
	}

	virtual void TearDown()
	{
        delete titleDataMock;
	}

	ObjectMock* titleDataMock;
};

TEST_F(CK2TitleShould, SetVersion2Point2SaveFormatLiege)
{
    auto SAMPLE_TITLE = "k_sample";
    auto SAMPLE_LIEGE = "e_sample";
    auto LIEGE_KEY = "liege";
    std::map<int, CK2Character*> characterMap;
    int sampleColor[3] = {0, 0, 0};
    CK2Title sampleTitle(SAMPLE_TITLE, sampleColor);

    EXPECT_CALL(*titleDataMock, getKey()).WillRepeatedly(Return(std::string()));
	EXPECT_CALL(*titleDataMock, getLeaf(_)).WillRepeatedly(Return(std::string()));
	EXPECT_CALL(*titleDataMock, getLeaves()).WillRepeatedly(Return(std::vector<IObject*>()));
	EXPECT_CALL(*titleDataMock, getValue(_)).WillRepeatedly(Return(std::vector<IObject*>()));
	EXPECT_CALL(*titleDataMock, getTitle(LIEGE_KEY)).WillOnce(Return(SAMPLE_LIEGE));

    sampleTitle.init(titleDataMock, characterMap, nullptr);

    ASSERT_EQ(SAMPLE_LIEGE, sampleTitle.getLiegeString());
}

} // namespace ck2
} // namespace unittests
