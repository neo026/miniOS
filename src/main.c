
#include "config.h"
#include "private.h"
#include "./ports/bsp.h"

#include "os_core.h"

//
application_type gProject;

//edit on the 'development' branch
void main(void)
{
    /* hardware initialization. */
    bspInit();

	//Initial global variable
    osMemset(&gProject, 0, sizeof(application_type));
    
    /* disable watch dog first. */
    osWatchDogDisable();
    
	/* os initialization */
    osInit();
    
    /* start initial the user's data */
    //userInit();

    /* enable watch dog */
    osWatchDogEnable();
    
    /* start to run all the loops*/
    osRunning();

    /* never come here */
}
