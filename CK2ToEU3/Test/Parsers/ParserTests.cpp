#include <sstream>
#include <string>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include "CppUnitTest.h"
#include "Parsers/Parser.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(ParserTests)
	{
	public:

		TEST_METHOD(ParserShouldThrowExceptionIfBufferIsValidatedWithoutParserBeingInitialized)
		{
			std::istringstream buffer("sample");

			Assert::ExpectException<std::domain_error>(boost::bind(&validateBuffer, boost::ref(buffer)));
		}
		
		TEST_METHOD(ParserShouldRecognizeUnicodeSWithCaronInNestedAssignments)
		{
			initParser();

			std::istringstream buffer("e_scandinavia = {\n\
	ugricbaltic = Ikškila\n\
}");

			Assert::IsTrue(validateBuffer(buffer));
		}

		TEST_METHOD(ParserShouldRecognizeUnicodeZWithCaronInNestedAssignments)
		{
			initParser();

			std::istringstream buffer("e_scandinavia = {\n\
	polish = Limbaži\n\
}");
			
			Assert::IsTrue(validateBuffer(buffer));
		}

	};
}