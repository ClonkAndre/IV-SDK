#include "IVSDK.cpp"

void Tick()
{
	Scripting::PRINT_STRING_WITH_LITERAL_STRING_NOW("STRING", "Hello World", 1000, true);
}

void plugin::gameStartupEvent()
{
	plugin::processScriptsEvent::Add(Tick);
}