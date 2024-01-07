#include "../userprog/syscall.h"

int main()
{
    char* argv1[] = {"createfile"};
    SpaceId t1, t2;
    t1 = ExecV(1, argv1);
    Join(t1);
}