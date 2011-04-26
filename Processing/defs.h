#ifndef DEFS_H
#define DEFS_H

#define SAMPLE_XML_PATH "Sample-User.xml"

#define CHECK_RC(rc, what)											\
        if (rc != XN_STATUS_OK)											\
        {		 														\
                printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
                return rc;													\
        }

#define GL_WIN_SIZE_X 720
#define GL_WIN_SIZE_Y 480


struct Rect {
    int top;
    int right;
    int bottom;
    int left;
};

struct Metric {
    float minHeight;
    float maxHeight;
    float minWidth;
    float maxWidth;
};
#endif // DEFS_H
