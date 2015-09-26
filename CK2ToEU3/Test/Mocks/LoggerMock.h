#include "gmock/gmock.h"

class LoggerMock : public LogBase
{
public:
	MOCK_METHOD0(Write, void());
};