//@<COPYRIGHT>@
//==================================================
//Copyright $2022.
//Siemens Product Lifecycle Management Software Inc.
//All Rights Reserved.
//==================================================
//@<COPYRIGHT>@

// 
//  @file
//  This file contains the implementation for the Business Object ItemRevisionImpl
//

#include <zy1_CustomExtensions/ItemRevisionImplExt.hxx>

#include <fclasses/tc_string.h>
#include <tcinit/tcinit.h>
#include <tccore/grm.h>
#include <tccore/tctype.h>

using namespace zy1_::zy1prod;

//----------------------------------------------------------------------------------
// ::zy1_::zy1prod::ItemRevisionImpl::ItemRevisionImpl(ItemRevision& busObj)
// Constructor for the class
//----------------------------------------------------------------------------------
::zy1_::zy1prod::ItemRevisionImpl::ItemRevisionImpl( ItemRevision& busObj )
   : ItemRevisionGenImpl( busObj )
{
}

//----------------------------------------------------------------------------------
// ::zy1_::zy1prod::ItemRevisionImpl::~ItemRevisionImpl()
// Destructor for the class
//----------------------------------------------------------------------------------
::zy1_::zy1prod::ItemRevisionImpl::~ItemRevisionImpl()
{
}

//----------------------------------------------------------------------------------
// ::zy1_::zy1prod::ItemRevisionImpl::initializeClass
// This method is used to initialize this Class
//----------------------------------------------------------------------------------
int ::zy1_::zy1prod::ItemRevisionImpl::initializeClass()
{
    int ifail = ITK_ok;
    static bool initialized = false;

    if( !initialized )
    {
        ifail = ItemRevisionGenImpl::initializeClass( );
        if ( ifail == ITK_ok )
        {
            initialized = true;
        }
    }
    return ifail;
}


///
/// Getter for a Tag Array Property
/// @version Teamcenter 13.3
/// @param values - Parameter value
/// @param isNull - Returns true for an array element if the parameter value at that location is null
/// @return - Status. 0 if successful
///
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
