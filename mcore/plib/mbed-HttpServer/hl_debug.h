#ifndef __DEBUG_H__
#define __DEBUG_H__


#ifdef DEBUG
#define INFO(x, ...) std::printf("[INFO: %s:%d]"x"\r\n", __FILE__, __LINE__, ##__VA_ARGS__);
#define WARN(x, ...) std::printf("[WARN: %s:%d]"x"\r\n", __FILE__, __LINE__, ##__VA_ARGS__);
#define ERR(x, ...) std::printf("[ERR: %s:%d]"x"\r\n", __FILE__, __LINE__, ##__VA_ARGS__);
#else
#define INFO(x, ...)
#define WARN(x, ...)
#define ERR(x, ...)
#endif


#endif //   __DEBUG_H__#ifndef __DEBUG_H__
