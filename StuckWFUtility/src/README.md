Stuck Workflow ITK Utility

This source code will help you find any stuck workflows.

<h2>How does this thing work?</h2>
We will perform a query in Teamcenter to find all active jobs (Covered in the Build section).

Once all the jobs are loaded, we will perform the following checks:

<ol>
  <li>Find all jobs where there is no active started task (Added to output)</li>
  <li>Find all started tasks, with no children started (It will drill down parent tasks to children and so forth to the lowest most started task)
    <ol>
      <li>Does the started task meet our automatic stuck types (Or Task and Validation Task)? If so, add to the output</li>
      <li>Does the started task meet our maximum days open threshhold (180)? If so, add to the output</li>
    </ol>
  </li>
</ol>
      
      
You can change the default max days in the Stuck_WF_Report.c file before you build it. The variable is 

    n_maxDaysAllowedInStartedTask = 180

<h2>How do I build this code?</h2>

This logic was build and ran against TC 13.2

You will need to download all 4 files first
Install Visual Studio C++ onto the server/computer you are going to build this on

    That server/computer needs to have a TC Command Prompt on it for the build

You need a query for this new command since we need to have a source for workflows

    Create a new query with whatever name
    Search Type: EPMJob
    Add a Search Critera of: root_task.state_value
    Make sure the L10N key is state and the User Entry Name is State
    This utility will call this name with a value of 4 to get all in process workflows.

Now, open a TC Command Prompt window

Set your command prompt location to the spot where you put the 4 downloaded files.

Input the following into the command line, the path may differ for you.

    "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

Input the following:

    SET MSDEV_HOME=%VCINSTALLDIR%
    SET MSVCDir=%VCINSTALLDIR%


Now we want to compile the source code into the EXE

	%TC_ROOT%\sample\compile -DIPLIB=none Stuck_WF_Report.c Extra_Utils.c
	%TC_ROOT%\sample\linkitk -o StuckWFReport Stuck_WF_Report.obj Extra_Utils.obj

Now the EXE is made, we can run it either from where you build it, or move the exe and pdb file into a new location.

	StuckWFReport.exe -u= -p= -g= -output_file=StuckWFOutputs.csv -query_name=__allactiveworkflowsquerynamehere
