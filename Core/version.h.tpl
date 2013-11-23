#ifndef VERSION_H
#define VERSION_H
 
    #define VERSION_MAJOR     0
    #define VERSION_MINOR     0
 
    // replaced with a value from SVN automatically
    #define VERSION_BUILD     $WCREV$
 
    // if the working copy is modified (not synchronized with SVN) the value == 1
    // otherwise the value == 0. Filled automatically.
    #define VERSION_MODIFIED  $WCMODS?1:0$
 
    // if the working copy contains mixed SVN trees the value == 1 (should not be mixed in nornal case)
    #define VERSION_MIXED     $WCMIXED?1:0$
 
#endif /* VERSION_H */