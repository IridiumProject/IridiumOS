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

#endif
