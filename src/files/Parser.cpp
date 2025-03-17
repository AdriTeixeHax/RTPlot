#include <Parser.h>

#include <FileManager.h>

RTPlot::Parser::Parser(const std::unordered_map<std::string, uint8_t>& _map) :
	map(_map) 
{ }

std::vector<std::uint8_t> RTPlot::Parser::Parse(const std::vector<std::string>& separatedCommands)
{
	std::vector<std::uint8_t> translatedCommands;

	for (auto i : separatedCommands)
		translatedCommands.push_back(map.count(i) ? map.at(i) : 0);

	return translatedCommands;
}

void RTPlot::Parser::ExecuteCommand(const std::vector<uint8_t>& commands, const std::vector<std::string>& separatedCommands, SerialPlotter& serialPlotter)
{
	if (commands.empty()) return;

	switch (commands.at(0))
	{
	case Parse_serialDevice:
		switch (commands.at(1))
		{
		case Parse_verboseData:
			switch (commands.at(2))
			{
			case Parse_true:
				serialPlotter.GetSerialDevice()->SetVerbose(true);
				break;
			case Parse_false:
				serialPlotter.GetSerialDevice()->SetVerbose(false);
				break;
			}
			break;
		}
		break;

	case Parse_realTimePlotter:
		RealTimePlot* plotterPtr = serialPlotter.GetRealTimePlotter();
		switch (commands.at(1))
		{
		case Parse_visibleVarsNum:
			plotterPtr->SetVisibleVarsNum(atof(separatedCommands.at(2).c_str()));
			break;

		case Parse_serialOptionsFlag:
			switch (commands.at(2))
			{
			case Parse_true:
				plotterPtr->SetSerialOptionsFlag(true);
				break;
			case Parse_false:
				plotterPtr->SetSerialOptionsFlag(false);
				break;
			}
			break;

		case Parse_plotters:
			switch (commands.at(2))
			{
			case Parse_Value:
				while (plotterPtr->GetPlotters()->size() < atoi(separatedCommands.at(3).c_str()))
					plotterPtr->GetPlotters()->push_back(new Plotter(plotterPtr->GetBasicData()));
				break;
			case Parse_p:
				Plotter* selectedPlotter = plotterPtr->GetPlotters()->at(atoi(separatedCommands.at(3).c_str()));
				switch (commands.at(4))
				break;
			}
			break;
		}
		break;
	}
}
