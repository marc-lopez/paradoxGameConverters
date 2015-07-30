#include <sstream>
#include "CppUnitTest.h"
#include "Parsers/Parser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(ParserTests)
	{
	public:
		
		TEST_METHOD(ParserShouldRecognizeUnicodeSWithCaronInNestedAssignments)
		{
			initParser();

			std::istringstream buffer("e_scandinavia = {\n\
	ugricbaltic = Ikškila\n\
}");

			Assert::IsTrue(validateBuffer(buffer));
		}

	};
}