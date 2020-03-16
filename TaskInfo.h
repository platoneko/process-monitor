#ifndef TASKINFO_H
#define TASKINFO_H

#include <string>

struct TaskInfo {
    unsigned short pid;
    unsigned short ppid;
    unsigned short uid;
    char pri;
    char ni;
    unsigned long virt;
    unsigned long res;
    unsigned long shr;
    char s;
    float cpu;
    float mem;
    unsigned long time;
    std::string comm;
    bool valid = true;
    bool matched = false;
    TaskInfo() {}
    TaskInfo(unsigned short pid, unsigned short ppid, unsigned short uid, char pri, char ni,
             unsigned long virt,
             unsigned long res,
             unsigned long shr,
             char s, float mem,
             unsigned long time,
             std::string comm):
        pid(pid),
        ppid(ppid),
        uid(uid),
        pri(pri),
        ni(ni),
        virt(virt),
        res(res),
        shr(shr),
        s(s),
        cpu(0),
        mem(mem),
        time(time),
        comm(comm) {
    }
};
#endif // TASKINFO_H
