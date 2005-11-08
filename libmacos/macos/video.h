/*
 *
 * (c) 2004,2005 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

#ifndef __MACOS_VIDEO_H__
#define __MACOS_VIDEO_H__
typedef struct VDFlagRec
{
        char flag;
} VDFlagRec;
typedef VDFlagRec *VDFlagPtr;

typedef struct VDParamBlock
{
	COMMON_PARAMS
        short ioRefNum;
        short csCode;
        void* csParam;
} VDParamBlock;
typedef VDParamBlock *VDParamBlockPtr;
#endif /* __MACOS_VIDEO_H__ */
