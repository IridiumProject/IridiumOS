#ifndef PERM_H
#define PERM_H

/*
 *  A task with this permission
 *  can grant/revoke other takes
 *  permissions (BE CAREFUL WITH THIS!!)
 */
#define PPERM_PERM      (1 << 0)

/*
 *  Allows a process to claim a limited amount of drivers.
 *
 */
#define PPERM_DRVCLAIM  (1 << 1)

/*
 *  Allows a process to 
 *  write to the console.
 */

#define PPERM_CONSOLE   (1 << 2)

#define PPERM_ADMIN     PPERM_CONSOLE           // Gives more permissions that a process usually lacks (Careful!).
#define PPERM_SUPER_ADMIN     PPERM_CONSOLE | PPERM_DRVCLAIM | PPERM_PERM         // Gives all permissions (CAREFUL!!!!).
#endif
