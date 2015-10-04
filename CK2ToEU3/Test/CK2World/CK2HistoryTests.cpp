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

#include <utility>
#include <gtest/gtest.h>
#include "Parsers/Object.h"
#include "CK2World/Character/CK2Character.h"
#include "Mocks/ObjectMock.h"
#include "CK2World/CK2History.h"

using namespace testing;

namespace ck2
{
namespace unittests
{
using namespace mocks;

class CK2HistoryShould : public Test
{
protected:
};

TEST_F(CK2HistoryShould, SetVersion2Point2SaveFormatHolder)
{
    auto CHARACTER_KEY = "character";
    auto HOLDER_KEY = "holder";
    auto SAMPLE_CHARACTER_ID = 1;

	ObjectMock historyDataMock;
	Object holderInnerObj(CHARACTER_KEY);
	holderInnerObj.setValue(std::to_string(SAMPLE_CHARACTER_ID));
	Object holderObj(HOLDER_KEY);
	holderObj.setValue(&holderInnerObj);
	std::vector<IObject*> historyObj = {&holderObj};
	CK2Character* sampleCharacter = new CK2Character();

	map<int, CK2Character*> characterMapping
	{
        std::make_pair(SAMPLE_CHARACTER_ID, sampleCharacter)
    };

	EXPECT_CALL(historyDataMock, getKey()).WillRepeatedly(Return(std::string()));
	EXPECT_CALL(historyDataMock, getLeaves()).WillRepeatedly(Return(historyObj));

	CK2History sampleHistory(&historyDataMock, characterMapping);

    ASSERT_EQ(sampleHistory.getHolder(), sampleCharacter);
}

} // namespace unittests
} // namespace ck2
