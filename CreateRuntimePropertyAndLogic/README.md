<h1>Create Runtime Property and Logic Behind</h1>

This example will show how to make a runtime property and the logic behind it.

This was based on how to pull linked PR's for all ItemRevision objects and generated a property with the values.

**This has been built and tested on TC 13.2 and 13.3**

<h1>Set Release Version</h1>

Set your released version to the proper version. This example was for TC 13.2

Add the view BMIDE, then expand down to the Releases

![image](https://user-images.githubusercontent.com/12979360/153721538-c26c7d97-6aa3-403b-98b0-a7e9757c6387.png)

Expand to your version -> right-click -> Organize -> Set as active Release

![image](https://user-images.githubusercontent.com/12979360/153721623-093e373d-b06e-4b0b-b79b-6e8b852f49db.png)

<h1>Add New Library</h1>

Expand down to the libraries branch in your BMIDE view

![image](https://user-images.githubusercontent.com/12979360/153721707-be37b283-9d4d-4394-99bd-a0937a046336.png)

Right-click -> Add New Library

Add cm and grm as extra dependancies

![image](https://user-images.githubusercontent.com/12979360/153722078-60b61d70-4523-4f1d-90e0-d192f1e3c351.png)

**Perform a BMIDE Template Save**

<h1>Create New Runtime Property</h1>

We will add the new runtime property to our ItemRevision base obejct for this example. Create a basic runtime property for 'TypedReference' as an Array with unlimited and finish it out.

![image](https://user-images.githubusercontent.com/12979360/153721890-8f9534b6-57ff-4a5d-a9b3-fef6369cfcb2.png)

**Perform a save**

Select the new property and go to 'Property Operations' and hit 'Add'

![image](https://user-images.githubusercontent.com/12979360/153722033-5d695540-0631-4eb4-990e-cab176e80808.png)

Check the 'Getter' checkbox and finish out

![image](https://user-images.githubusercontent.com/12979360/153722126-63187aca-6cfd-465b-aaba-702f13b5c8a4.png)

Now your 'Property Operations' should show you the new getter we just added.

![image](https://user-images.githubusercontent.com/12979360/153722171-1566569a-60e6-4b5a-948a-1d2aaff22263.png)

**Perform Save**

<h1>Generate Code Files</h1>

Right-click the template project -> Generate Code -> C++ Classes

![image](https://user-images.githubusercontent.com/12979360/153771626-fa068c33-c099-43c5-b994-acdf26deab47.png)

Now you should see your boiler plate code files generated in the Project Files -> Generated Source folder

![image](https://user-images.githubusercontent.com/12979360/153771697-6cf2cdd9-8537-4ebd-aca6-224a3250498a.png)

You should also see the ImplExt files in your Src -> Server -> _CustomExtensions folder

![image](https://user-images.githubusercontent.com/12979360/153771792-48bddd18-c734-4f2c-8d5f-7b53f406415b.png)

<h1>Modify code file</h1>

Edit the ItemRevisionImplExt.cxx file in your src -> server -> customextensions folder

Add additional includes at the top

    #include <tccore/grm.h>
    #include <tccore/tctype.h>

Adjust the function

    int  ::zy1_::zy1prod::ItemRevisionImpl::getZy1_AuthoringPRBase( std::vector< tag_t > & /*values*/, std::vector< int > & /*isNull*/ ) const
    
To the following:

	int  ::zy1_::zy1prod::ItemRevisionImpl::getZy1_AuthoringPRBase( std::vector<tag_t> &values, std::vector<int> &isNull ) const
	{
		int ifail = ITK_ok;

		tag_t tRevObj = ItemRevisionGenImpl::getItemRevision()->getTag();
		if (tRevObj != NULL_TAG)
		{
			tag_t tRelationTypeTag = NULLTAG,
					*secondaryObjectTags = NULL;
			int n_totalRelations = 0;
			GRM_find_relation_type("CMHasProblemItem", &tRelationTypeTag);
			GRM_list_primary_objects_only(tRevObj, tRelationTypeTag, &n_totalRelations, &secondaryObjectTags);
			if (n_totalRelations > 0)
			{
				for(int cidx = 0; cidx < n_totalRelations; cidx++)
				{
					tag_t tObjectType = NULL_TAG;
					logical hasCorrectType = false;
					TCTYPE_ask_object_type(secondaryObjectTags[cidx], &tObjectType);
					TCTYPE_is_type_of_as_str(tObjectType, "GnProblemReportRevision", &hasCorrectType);
					if (hasCorrectType)
					{
						values.push_back(secondaryObjectTags[cidx]);
						isNull.push_back(false);
					}
				}
			}
			MEM_free(secondaryObjectTags);
		}
		return ifail;
	}
  
Now perform a clean -> generate -> build

Once you are good to go, perform your deployment
