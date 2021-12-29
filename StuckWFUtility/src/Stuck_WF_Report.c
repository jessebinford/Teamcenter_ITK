

#include <stdlib.h>
#include <string.h>
#include <tccore/workspaceobject.h>
#include <sa/sa.h> 
#include <sa/user.h> 
#include <sa/group.h> 
#include <sa/tcfile.h>
#include <tc/folder.h>
#include <tccore/item.h>
#include <tccore/aom.h>
#include <tc/tc_util.h>
#include <tc/emh.h>
#include <tc/emh_errors.h>
#include <tc/tc_util.h>
#include <cfm/cfm.h>
#include <me/me.h>
#include <epm/epm.h>
#include <tccore/aom_prop.h>
#include <fclasses/tc_string.h>
#include <fclasses/tc_date.h>
#include <time.h>
#include <math.h>
#include <custVectorArray.c>
#include <Extra_Utils.h>


#define EXIT_FAILURE 1
#define ITK_CALLER(x) {           \
    int stat;                     \
    char *err_str;             \
    if( (stat = (x)) != ITK_ok)   \
    { \
		EMH_get_error_string (NULLTAG, stat, &err_str); \
		printf ("ERROR: %d ERROR MSG: %s.\n", stat, err_str); \
		printf ("FUNCTION: %s\nFILE: %s LINE: %d\n",#x, __FILE__, __LINE__); \
		if(err_str) MEM_free(err_str); \
		exit (EXIT_FAILURE); \
    } \
}

char* queryName = "__active_workflow_query",
		*exportFileName = NULL;

custVectorArray badTaskStartArray;
EXUStuckWorkflowDetail workflowDetailStruct;
EXUFileWriter fileWriter;

logical showOutputs = false;

int n_openWorkflows,
	n_totalTasksAsAutoStuck = 0,
	n_maxDaysAllowedInStartedTask = 180;

tag_t tQueryObject = NULLTAG,
		tDoTaskType = NULLTAG,
		*tOpenWorkflowJobs = NULL;
		
		
logical isTimeInTaskExceedingLimit(const char* c_taskStartDate, int *totalDaysInStartedTask)
{
	logical result = false;
    time_t currentUTCTime, taskStartTime;
    struct tm startTimeInfo;
	
	time(&currentUTCTime);
  
	int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;
  
	if (sscanf(c_taskStartDate, "%4d.%2d.%2d %2d:%2d.%2d", &year, &month, &day, &hour, &min, &sec) == 6) 
	{
		startTimeInfo.tm_year = year - 1900; /* years since 1900 */
		startTimeInfo.tm_mon = month - 1;
		startTimeInfo.tm_mday = day;
		startTimeInfo.tm_hour = hour;
		startTimeInfo.tm_min = min;
		startTimeInfo.tm_sec = sec;
		taskStartTime = mktime(&startTimeInfo);
		
		
		double d_totalSecondsInTask = difftime(currentUTCTime,taskStartTime);
		if (d_totalSecondsInTask)
		{
			int i_totalSecondsInProcess = ceil(d_totalSecondsInTask);
			int i_totalDaysInStartedTask = ((i_totalSecondsInProcess / 60) / 60) / 24;
			workflowDetailStruct.totalDaysInTask = i_totalDaysInStartedTask;
			
			if (i_totalDaysInStartedTask > n_maxDaysAllowedInStartedTask)
			{
				*totalDaysInStartedTask = i_totalDaysInStartedTask;
				result = true;
			}
		}
		
	}
	else {
		printf("Error parsing date... \n");
	}
	
	
	return result;
}

/// isTaskAutoStuck
/// Determine if the task is automatically considered stuck based on config file.
/// Sometimes if a task is in the started condition, it is probably stuck. Ex: Or Task
logical isTaskAutoStuck(tag_t task)
{
	logical resultValue = false;
	if (n_totalTasksAsAutoStuck > 0)
	{
		tag_t tActiveTaskType = NULLTAG;
		ITK_CALLER(TCTYPE_ask_object_type(task,&tActiveTaskType));
		if (tActiveTaskType != NULLTAG)
		{
			const int *tagArrayData = custVectorArray_data(&badTaskStartArray);
			for(int i = 0; i < custVectorArray_size(&badTaskStartArray); i++)
			{
				logical isTaskMatching;
				ITK_CALLER(TCTYPE_is_type_of(tActiveTaskType, tagArrayData[i], &isTaskMatching));
				if (isTaskMatching)
				{
					resultValue = true;
					break;
				}
			}
		}
	}
	
	return resultValue;
}

void setStuckTaskPropValues(tag_t tTask)
{
	char *object_name_value = NULL,
		*object_type_name = NULL;
	ITK_CALLER(AOM_ask_value_string(tTask, "object_name", &object_name_value));
	ITK_CALLER(WSOM_ask_object_type2(tTask, &object_type_name));
	
	char *newObjName = (char *)malloc(strlen(object_name_value)+1);
	strcpy(newObjName,object_name_value);
	
	char *newTypeName = (char *)malloc(strlen(object_type_name)+1);
	strcpy(newTypeName,object_type_name);
	
	workflowDetailStruct.stuckTaskName = newObjName;
	workflowDetailStruct.stuckTaskType = newTypeName;
	
	if(object_name_value) 
	{
		MEM_free(object_name_value);
	}
	
	if(object_type_name) 
	{
		MEM_free(object_type_name);
	}
	
}

logical hasOpenStartedTask(tag_t tTask, int *totalSubTasks)
{
	logical result = false;
	int n_totalSubTasks = 0;
	tag_t *tSubTasks = NULL;
	
	ITK_CALLER(EPM_ask_sub_tasks(tTask, &n_totalSubTasks, &tSubTasks));
	
	(*totalSubTasks) = n_totalSubTasks;
	
	for(int idx = 0; idx < n_totalSubTasks; idx++)
	{
		EPM_state_t e_taskState;
		EPM_ask_state(tSubTasks[idx], &e_taskState);
		if (e_taskState == EPM_started)
		{
			int totalChildSubTasks = 0;
			logical hasStartedChildTask = false;
			// Determine if the task that is in the started state is one we marked as auto fail.
			logical isTaskAutoFail = isTaskAutoStuck(tSubTasks[idx]);
			
			if (!isTaskAutoFail)
			{
				hasStartedChildTask = hasOpenStartedTask(tSubTasks[idx], &totalChildSubTasks);
			}
			
			if (totalChildSubTasks == 0)
			{
				
				// We want to see if this has been open > the max number of allowed days. If so, consider it stuck...
				if (n_maxDaysAllowedInStartedTask > 0)
				{
					date_t d_startDate;
					char *converted_Date = NULL;
					ITK_CALLER(AOM_ask_value_date(tSubTasks[idx], "fnd0StartDate", &d_startDate));
					ITK_CALLER(DATE_date_to_string ( d_startDate, "%Y.%m.%d %H:%M.%S", &converted_Date ));
					
					int n_TotalDaysInStartedTask = 0;
					if (isTimeInTaskExceedingLimit(converted_Date, &n_TotalDaysInStartedTask) || isTaskAutoFail)
					{
						if (showOutputs){
							printf("Total days in task: %d \n", n_TotalDaysInStartedTask);
						}

						MEM_free(converted_Date);
						
						setStuckTaskPropValues(tSubTasks[idx]);
						result = false;
						break;
					}
					
					if(converted_Date) 
					{
						MEM_free(converted_Date);
					}
				}
				
				
				result = true;
				break;
			}
			else
			{
				if (hasStartedChildTask)
				{
					result = true;
					break;
				}
			}
			
		}
	}
	
	if (tSubTasks)
	{
		MEM_free(tSubTasks);
	}
	
	return result;
}

static void performTaskCheck(void)
{
	fileWriter = (struct EXUFileWriter){0};
	fileWriter.fullFileLocation = exportFileName;
	for(int idx = 0; idx < n_openWorkflows; idx++)
	{
		tag_t tRootTask = NULLTAG;
		
		if (workflowDetailStruct.stuckTaskName != NULL)
		{
			free(workflowDetailStruct.stuckTaskName);
			free(workflowDetailStruct.stuckTaskType);
		}
		
		workflowDetailStruct = (struct EXUStuckWorkflowDetail){0};
		
		
	    ITK_CALLER(EPM_ask_root_task(tOpenWorkflowJobs[idx], &tRootTask));
		if (tRootTask != NULLTAG)
		{
			int totalSubTasks = 0;
			logical hasStartedTasks = hasOpenStartedTask(tRootTask, &totalSubTasks);
			
			if (!hasStartedTasks)
			{
				// Load Template Name...
				char *procName = NULL;
				ITK_CALLER(EPM_ask_procedure_name2(tOpenWorkflowJobs[idx], &procName));
				workflowDetailStruct.wfTemplateName = procName;
				
				
				// Load workflow object name...
				char *object_name_value = NULL;
				ITK_CALLER(AOM_ask_value_string(tOpenWorkflowJobs[idx], "object_name", &object_name_value));
				if (showOutputs){
					printf("Stuck WF Name: %s \n", object_name_value);
				}
				workflowDetailStruct.workflowName = object_name_value;
				write_to_file_wf_report(&fileWriter, &workflowDetailStruct);
				
				if(object_name_value) 
				{
					MEM_free(object_name_value);
				}
				
				if(procName) 
				{
					MEM_free(procName);
				}
				
			}
		}
	}
	
	close_file_writer(&fileWriter);
}

logical performQuery()
{
	char *qry_entries[1] = {"State"}, 
			*qry_values[1] = {"4"};
			
    ITK_CALLER(QRY_execute(tQueryObject, 1, qry_entries, qry_values, &n_openWorkflows, &tOpenWorkflowJobs));
    printf("Total Open Workflow Jobs: %d \n", n_openWorkflows);
	if (n_openWorkflows == 0)
	{
		if (tOpenWorkflowJobs) 
		{
			MEM_free(tOpenWorkflowJobs);
		}
		return false;
	}
	
	return true;
}

logical findQuery()
{
	ITK_CALLER(QRY_find2(queryName, &tQueryObject));
	
	if (tQueryObject != NULLTAG)
	{
		printf("Query found... \n");
	}
	else
	{
		printf("No Query found... \n");
	}
	
	return tQueryObject != NULLTAG ? true : false;
}

static void clearWorkflowJobObjects(void)
{
	if (tOpenWorkflowJobs) 
	{
		MEM_free(tOpenWorkflowJobs);
	}
}

void loadTaskTypes(const char* epmTaskType)
{
	tag_t taskTypeObject = NULLTAG;
	
	ITK_CALLER(TCTYPE_find_type(epmTaskType, "EPMTask", &taskTypeObject));
	if (taskTypeObject != NULLTAG)
	{
		n_totalTasksAsAutoStuck++;
		custVectorArray_append(&badTaskStartArray, taskTypeObject);
	}
}

int ITK_user_main(int argc, char* argv[])
{
    int status = 0;
	
    custVectorArray_create(&badTaskStartArray);
    
    ITK_initialize_text_services( 0 );
	status = ITK_auto_login();
	
    exportFileName = ITK_ask_cli_argument("-output_file=");
	char* newQueryName = ITK_ask_cli_argument("-query_name=");
	
	for(int idx = 0; idx < argc; idx++){
		if (strcmp(argv[idx], "-showOutputs") == 0){
			// Show outputs
			showOutputs = true;
		}
	}
	
	if (newQueryName != NULL){
		queryName = ITK_ask_cli_argument("-query_name=");
	}
	
	if (exportFileName == NULL){
		printf("Missing -output_file value...\n");
	}
	else if (queryName == NULL){
		printf("Invalid Query Name Value...\n");
	}
	else {
		if (  (status != ITK_ok)) printf("Login NOT successful.\n");
		else
		{
			printf("Login successful.\n");
			ITK_CALLER(ITK_set_journalling(TRUE));
			
			// Set our default task failures
			loadTaskTypes("EPMOrTask");
			loadTaskTypes("EPMValidationTask");
			
			if (findQuery())
			{
				if (performQuery())
				{
					performTaskCheck();
				}
			}
			
			clearWorkflowJobObjects();
		}
	}
	
	printf("Done...\n");
    custVectorArray_destroy(&badTaskStartArray);
	
    ITK_exit_module(TRUE);
    return status;
}