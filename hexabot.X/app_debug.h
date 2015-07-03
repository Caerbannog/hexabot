
#ifndef APP_DEBUG_H
#define	APP_DEBUG_H

#define WARN(str_then_args...)        LOG("WARNING " str_then_args)
#define ERROR(str_then_args...)       LOG("ERROR " str_then_args)
#define LOG(args...)     debug_log(__FILE__, __LINE__, args)

void debug_log(const char * file, int line, const char * format, ...);

#endif	/* APP_DEBUG_H */
