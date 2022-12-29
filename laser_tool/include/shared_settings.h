#ifndef SHARED_SETTINGS_H
#define SHARED_SETTINGS_H

#define laser_tool_EXPORT 1

#ifdef laser_tool_EXPORT
	#define  LIB_EXPORT __declspec(dllexport)
#else
	#define  LIB_EXPORT __declspec(dllimport)
#endif

#endif