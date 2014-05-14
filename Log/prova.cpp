#include <log.h>

int main()
{
	Log::setFrameNumber(19);
	Log::setInfoLogFile("info.log");
	Log::info() << "prova prova" << endl;
	Log::closeLogs();
}
