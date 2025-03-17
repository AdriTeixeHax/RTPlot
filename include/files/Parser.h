#ifndef _PARSER__H_
#define _PARSER__H_

#include <string>
#include <unordered_map>

#include <DeviceManager.h>

enum RTPlotParseCodes
{
	Parse_Value = 0,
	Parse_COMPort,
	Parse_killFlag,
	Parse_serialDevice,
	Parse_realTimePlotter,
	Parse_p,
	Parse_true,
	Parse_false,
	Parse_verboseData,
	Parse_plotters,
	Parse_basicData,
	Parse_serialOptionsFlag,
	Parse_data,
	Parse_name,
	Parse_plotFlag,
	Parse_history,
	Parse_plotColor,
	Parse_plotData,
	Parse_dataName,
	Parse_plottable,
	Parse_id,
	Parse_color,
	Parse_x,
	Parse_y,
	Parse_z,
	Parse_w,
	Parse_visibleVarsNum
};

const std::unordered_map<std::string, uint8_t> RTPLOT_COMMAND_MAP =
{
	{"Value",               Parse_Value             },
	{"COMPort",           	Parse_COMPort			},
	{"killFlag",          	Parse_killFlag			},
	{"serialDevice",      	Parse_serialDevice		},
	{"realTimePlotter",   	Parse_realTimePlotter	},
	{"p",                 	Parse_p					},
	{"data",              	Parse_data				},
	{"true",              	Parse_true				},
	{"false",             	Parse_false				},
	{"verboseData",       	Parse_verboseData		},
	{"plotters",		  	Parse_plotters			},
	{"basicData",		  	Parse_basicData			},
	{"serialOptionsFlag", 	Parse_serialOptionsFlag	},
	{"data",              	Parse_data				},
	{"name",              	Parse_name				},
	{"plotFlag",          	Parse_plotFlag			},
	{"history",           	Parse_history			},
	{"plotColor",         	Parse_plotColor			},
	{"plotData",          	Parse_plotData			},
	{"dataName",          	Parse_dataName			},
	{"plottable",         	Parse_plottable			},
	{"id",                	Parse_id				},
	{"color",             	Parse_color				},
	{"x",                 	Parse_x					},
	{"y",                 	Parse_y					},
	{"z",                 	Parse_z					},
	{"w",                 	Parse_w					},
	{"visibleVarsNum",      Parse_visibleVarsNum	}
};

namespace RTPlot
{
	class Parser
	{
		std::unordered_map<std::string, uint8_t> map;

	public:
		Parser(void) = delete;
		Parser(const std::unordered_map<std::string, uint8_t>& _map);

		std::vector<std::uint8_t> Parse(const std::vector<std::string>& separatedCommands);
		void ExecuteCommand(const std::vector<uint8_t>& commands, const std::vector<std::string>& separatedCommands, SerialPlotter& serialPlotter);
	};
}

#endif