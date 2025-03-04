#ifndef __RTPLOTVARS_H__
#define __RTPLOTVARS_H__

// Window name
constexpr const char* RTPLOT_WINDOW_NAME = "RTPlot - by AdriTeixeHax";

// Logo path
constexpr const char* RTPLOT_LOGO_PATH = "res/logos/RTPlot.png";

// Font paths and sizes
constexpr const char* RTPLOT_SOURCESANS_PATH = "res/fonts/SourceSans/SourceSans3-Regular.ttf";
constexpr const char* RTPLOT_CONSOLA_PATH    = "res/fonts/Consola/Consola.ttf";
constexpr uint8_t RTPLOT_CONSOLA_SIZE          = 15;
constexpr uint8_t RTPLOT_SOURCESANS_SIZE       = 20;
constexpr uint8_t RTPLOT_SOURCESANS_SIZE_LARGE = 40;

// UI Parameters
constexpr uint8_t RTPLOT_WINDOW_RADIUS = 5;
constexpr uint8_t RTPLOT_MAIN_RADIUS   = 3;

// Serial parameters
constexpr uint8_t  RTPLOT_MAX_PORT_NUMBER = 255;
constexpr uint32_t RTPLOT_MSG_SIZE        = 256;
constexpr uint8_t  RTPLOT_MAX_DATA_NUM    = 16;
constexpr uint8_t  RTPLOT_DATA_SIZE       = 16;
constexpr uint8_t  RTPLOT_READING         = 1;
constexpr uint8_t  RTPLOT_FINISHED        = 0;
constexpr uint8_t  RTPLOT_OK	          = 0;
constexpr  int8_t  RTPLOT_ERROR           = -1;
constexpr uint8_t  RTPLOT_READING_DELAY   = 0;
constexpr  double  RTPLOT_TOLERANCE_UPPER = 1e6;
constexpr  double  RTPLOT_TOLERANCE_LOWER = -1e6;

#endif