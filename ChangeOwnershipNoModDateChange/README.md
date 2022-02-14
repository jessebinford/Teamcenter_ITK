<h1>Change Ownership without Last Mod Date Changing</h1>

This utility will change the ownership on objects found via a query but will NOT change the last mod date on the objects. This utility uses the OOTB query 'Admin - Object Ownership' to find all objects owned by the defined user and group to perform the change against.

**This has been built and tested on TC 13.2 and 13.3.**

<h1>How do I build this?</h1>

First, you need to save the cxx file out to a folder on a computer with a TCROOT with the folders include, include_cpp, sample.

_If you do not have a sample folder, you can install it via TEM_

Edit the compile.bat in TCROOT\dample folder to include the includes for CPP. Change the $INCLUDES section

    # Set all include directories.
    $INCLUDES = "-I\"$ENV{USER_INCLUDE}\" " .
                "-I\"$ENV{TC_INCLUDE}\" " .
                "-I\"E:\\TC13\\TCROOT\\include_cpp\"";

Run a TC Command Prompt from your folder containing your cxx file.

Enter the following commands - *Your locations may be different than mine*

    "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
    SET MSDEV_HOME=%VCINSTALLDIR%
    SET MSVCDir=%VCINSTALLDIR%

Now you want to build your EXE

    %TC_ROOT%\sample\compile -DIPLIB=none changeowner.cxx
    %TC_ROOT%\sample\linkitk -o ChangeOwnerUtil changeowner.obj

Now you can run the utility with the following command

    ChangeOwnerUtil.exe -u=infodba -p=infodba -g=dba -targetUser=currentowneruserid -targetGroup=currentownergroup -newOwningUser=newowneruserid -newOwningGroup=newownergroup

The new utility will use the **targetUser** and **targetGroup** value in the query we listed above. The result set will then be taken and swapped to the **newOwningUser** and **newOwningGroup** values.
