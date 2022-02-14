
#include <stdlib.h>
#include <sa/sa.h>
#include <qry/qry.h>
#include <tcinit/tcinit.h>
#include <tc/tc_util.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tc/emh_errors.h>

#include <string>

#include <metaframework/BusinessObjectRef.hxx>
#include <base_utils/IFail.hxx>
#include <base_utils/TcResultStatus.hxx>
#include <base_utils/ScopedSmPtr.hxx>
#include <mld/logging/TcMainLogger.hxx>

using namespace std;
using namespace Teamcenter;
using Teamcenter::Main::logger;


char *targetUser, *targetGroup, *newUserOwner, *newGroupOwner;
scoped_smptr<tag_t> tQueryObjectsFound;
tag_t tNewOwningUser = NULLTAG, tNewOwningGroup = NULLTAG;
int nTotalQueryObjectsFound = 0;


int performQuery()
{
    int status = ITK_ok;
    tag_t tQueryObj = NULLTAG;
    status = QRY_find2("Admin - Object Ownership", &tQueryObj);

    int n_entries = 2;
    char *qry_entries[2] = {"Owning User", "Owning Group"}, 
            *qry_values[2] = {targetUser, targetGroup};
    status = QRY_execute(tQueryObj, n_entries, qry_entries, qry_values, &nTotalQueryObjectsFound, &tQueryObjectsFound);

    return status;
}

void changeOwnershipOfObjects()
{
    // Configure environment to not update mod date and also ignore ACL's to ensure
    // we will be able to update the owner.
    POM_set_env_info(POM_bypass_attr_update, FALSE, 0, 0.0, NULL_TAG, NULL);
    POM_set_env_info(POM_bypass_access_check, TRUE, 0, 0.0, NULL_TAG, NULL);
    
    printf("Changing Owner of %d total objects...\n", nTotalQueryObjectsFound);

    for (int i = 0; i < nTotalQueryObjectsFound; i++)
    {
        AOM_set_ownership(tQueryObjectsFound[i], tNewOwningUser, tNewOwningGroup);
    }

    printf("Done changing ownership...\n");

    // Reset the environment flags.
    POM_set_env_info(POM_bypass_attr_update, TRUE, 0, 0.0, NULL_TAG, NULL);
    POM_set_env_info(POM_bypass_access_check, FALSE, 0, 0.0, NULL_TAG, NULL);
}

int loadNewUserGroup()
{
    int status = ITK_ok;
    SA_find_user2(newUserOwner, &tNewOwningUser);
    SA_find_group(newGroupOwner, &tNewOwningGroup);

    if (tNewOwningUser == NULLTAG || tNewOwningGroup == NULLTAG)
    {
        status = -1;
    }

    return status;
}


int ITK_user_main(int argc, char* argv[])
{
    int status = 0;

    // Load all of the params
    targetUser = ITK_ask_cli_argument("-targetUser=");
    targetGroup = ITK_ask_cli_argument("-targetGroup=");
    newUserOwner = ITK_ask_cli_argument("-newOwningUser=");
    newGroupOwner = ITK_ask_cli_argument("-newOwningGroup=");
    
    if (targetUser == NULL)
    {
        printf("Missing targetUser param...\n");
        return -1;
    }
    if (targetGroup == NULL)
    {
        printf("Missing targetGroup param...\n");
        return -1;
    }
    if (newUserOwner == NULL)
    {
        printf("Missing newOwningUser param...\n");
        return -1;
    }
    if (newGroupOwner == NULL)
    {
        printf("Missing newOwningGroup param...\n");
        return -1;
    }

    ITK_initialize_text_services( 0 );
	status = ITK_auto_login();
   
	if ((status != ITK_ok)) printf("Login Failed...\n");
	else
	{
		printf("Login Successful...\n");
		ITK_set_journalling(TRUE);
        
        int functionStatus = ITK_ok;
        functionStatus = loadNewUserGroup();

        if (functionStatus != ITK_ok)
        {
            printf("Error loading the new user/group specified...\n");
        }
        else
        {
            functionStatus = performQuery();

            if (functionStatus != ITK_ok)
            {
                printf("Error loading the query...\n");
            }
            else
            {
                if (nTotalQueryObjectsFound > 0)
                {
                    changeOwnershipOfObjects();
                }
                else
                {
                    printf("Nothing found for the user/group...\n");
                }
            }

        }
	}
	
    printf("Finished...\n");
    ITK_exit_module(TRUE);
    return status;
}
