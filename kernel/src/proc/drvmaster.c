#include <proc/drvmaster.h>
#include <proc/proc.h>
#include <stddef.h>

#define DRIVERTYPE_ARR_INDEX(driver_type) driver_type - 1


extern struct Process* current_task;


// If value in here is 1 that driver has been claimed, otherwise 0.
static uint8_t drivertypes[DRIVERTYPE_COUNT];


ERRNO_T drv_claim(DRIVER_TYPE_T driver_type) { 
    if (DRIVERTYPE_ARR_INDEX(driver_type) >= DRIVERTYPE_COUNT || DRIVERTYPE_ARR_INDEX(driver_type) < 0) {
        // Invalid driver_type.
        return -EXIT_FAILURE;
    }

    if (drivertypes[DRIVERTYPE_ARR_INDEX(driver_type)] != 0) {
        // Already claimed, maybe a double claim occured (i.e trying to claim twice).
        return -EDRVCLAIMED;
    }

    for (size_t i = 0; i < current_task->n_slave_driver_groups; ++i) {
        if (current_task->slave_driver_groups[i] == driver_type) {
            // Already owns the driver.
            return -EXIT_FAILURE;
        }
    }

    if (current_task->n_slave_driver_groups == DRVMASTER_MAX_SLAVES - 1) {
        // No space left for any slave drivers thus return -NOBUFS
        return -ENOBUFS;
    }

    if (!(current_task->perm_mask & PPERM_DRVCLAIM)) {
        // Does not have PPERM_DRVCLAIM.
        return -EPERM;
    }

    drivertypes[DRIVERTYPE_ARR_INDEX(driver_type)] = 1;
    current_task->slave_driver_groups[current_task->n_slave_driver_groups++] = driver_type;
    return EXIT_SUCCESS;
}


ERRNO_T verify_drvmaster(DRIVER_TYPE_T driver_type) {  
    for (size_t i = 0; i < current_task->n_slave_driver_groups; ++i) {
        if (current_task->slave_driver_groups[i] == driver_type) {
            // This process does indeed own the driver.
            return EXIT_SUCCESS;
        }
    }

    // Process does not own the group.
    return -EXIT_FAILURE;
}
