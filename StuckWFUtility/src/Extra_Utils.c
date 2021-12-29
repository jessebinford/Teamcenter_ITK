#include "Extra_Utils.h"


void write_to_file_wf_report (EXUFileWriter *fileWriterObj, EXUStuckWorkflowDetail *workflowData)
{
	if (fileWriterObj->exportFilePtr == NULL)
	{
		printf("\nCreating output file...");
		fileWriterObj->exportFilePtr = fopen(fileWriterObj->fullFileLocation, "w");
		fprintf(fileWriterObj->exportFilePtr,"Delim: |\n");
		fprintf(fileWriterObj->exportFilePtr,"WF Template Name|Workflow Name|Task Type|Task Name|Days In Starting\n");
	}
	
	if (fileWriterObj->exportFilePtr != NULL)
	{
		fprintf(fileWriterObj->exportFilePtr,"%s|%s|%s|%s|%d\n", workflowData->wfTemplateName, workflowData->workflowName, workflowData->stuckTaskType, workflowData->stuckTaskName, workflowData->totalDaysInTask);
	}
}

void close_file_writer(EXUFileWriter *fileWriterObj)
{
	printf("\nClosing output file...\n");
	fclose(fileWriterObj->exportFilePtr);
}