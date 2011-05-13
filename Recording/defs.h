/**
 * @file defs.h
 * @brief General definition file
 * @author David Dutch, Vincent Ischi
 * @version 1.0
 *
 * General definition file with types and constants that a global to the whole program
 */

#ifndef DEFS_H
#define DEFS_H


#define START_CAPTURE_CHECK_RC(rc, what)												\
        if (rc != XN_STATUS_OK)														\
{																					\
        printf("Failed to %s: %s\n", what, xnGetStatusString(rc));				\
        endRecording();															\
        return ;																	\
}
#define SAMPLE_XML_PATH "Sample-User.xml"

#define CHECK_RC(rc, what)											\
        if (rc != XN_STATUS_OK)	{																\
            printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
            return ;													\
        }

#endif // DEFS_H
