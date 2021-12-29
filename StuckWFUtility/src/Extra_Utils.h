#ifndef Extra_Utils_H
#define Extra_Utils_H

#include <stdio.h>

typedef struct EXUFileWriter {
	FILE *exportFilePtr;
	char *fullFileLocation;
} EXUFileWriter;

typedef struct EXUStuckWorkflowDetail {
	char *workflowName;
	char *wfTemplateName;
	char *stuckTaskName;
	char *stuckTaskType;
	char *workflowTemplateName;
    int totalDaysInTask;
} EXUStuckWorkflowDetail;



#ifdef __cplusplus
extern "C"{
#endif

void write_to_file_wf_report(EXUFileWriter *fileWriterObj, EXUStuckWorkflowDetail *workflowData);
void close_file_writer(EXUFileWriter *fileWriterObj);

#ifdef __cplusplus
}
#endif


#endif