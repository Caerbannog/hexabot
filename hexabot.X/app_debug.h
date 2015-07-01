
#ifndef APP_DEBUG_H
#define	APP_DEBUG_H

#define WARN(str)        LOG("WARNING %s", str)
#define ERROR(str)       LOG("ERROR %s", str)
#define LOG(args...)     debug_log(__FILE__, __LINE__, args)

void debug_log(const char * file, int line, const char * format, ...);

#endif	/* APP_DEBUG_H */
