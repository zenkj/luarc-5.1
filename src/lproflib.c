/*
** $Id: lproflib.c,v 0.0.1 2015/03/23 12:53:25 juzejian $
** Profiling library
** See Copyright Notice in lua.h
*/

#define lproflib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


#if LUA_PROFILE

static int prof_stackresizecount (lua_State *L) {
  int n = lua_gettop(L);
  if (n == 0)
    lua_pushthread(L);
  else
    luaL_checktype(L, -1, LUA_TTHREAD);
  lua_pushinteger(L, lua_stackresizecount(L, -1));
  return 1;
}

static int prof_ciresizecount (lua_State *L) {
  int n = lua_gettop(L);
  if (n == 0)
    lua_pushthread(L);
  else
    luaL_checktype(L, -1, LUA_TTHREAD);
  lua_pushinteger(L, lua_ciresizecount(L, -1));
  return 1;
}

static int prof_tableresizecount (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_pushinteger(L, lua_tableresizecount(L, 1));
  return 1;
}

static int prof_gcsteps (lua_State *L) {
  lua_Statdata sd;
  lua_gcsteps(L, &sd);
  lua_pushnumber(L, sd.max);
  lua_pushnumber(L, sd.min);
  lua_pushnumber(L, sd.avg);
  lua_pushinteger(L, sd.cnt);
  return 4;
}

static int prof_marksteps (lua_State *L) {
  lua_Statdata sd;
  lua_marksteps(L, &sd);
  lua_pushnumber(L, sd.max);
  lua_pushnumber(L, sd.min);
  lua_pushnumber(L, sd.avg);
  lua_pushinteger(L, sd.cnt);
  return 4;
}

static int prof_sweepstringsteps (lua_State *L) {
  lua_Statdata sd;
  lua_sweepstringsteps(L, &sd);
  lua_pushnumber(L, sd.max);
  lua_pushnumber(L, sd.min);
  lua_pushnumber(L, sd.avg);
  lua_pushinteger(L, sd.cnt);
  return 4;
}

static int prof_sweepsteps (lua_State *L) {
  lua_Statdata sd;
  lua_sweepsteps(L, &sd);
  lua_pushnumber(L, sd.max);
  lua_pushnumber(L, sd.min);
  lua_pushnumber(L, sd.avg);
  lua_pushinteger(L, sd.cnt);
  return 4;
}

static int prof_finalizesteps (lua_State *L) {
  lua_Statdata sd;
  lua_finalizesteps(L, &sd);
  lua_pushnumber(L, sd.max);
  lua_pushnumber(L, sd.min);
  lua_pushnumber(L, sd.avg);
  lua_pushinteger(L, sd.cnt);
  return 4;
}

static int prof_gcperiod (lua_State *L) {
  lua_Statdata sd;
  lua_gcperiod(L, &sd);
  lua_pushnumber(L, sd.max);
  lua_pushnumber(L, sd.min);
  lua_pushnumber(L, sd.avg);
  lua_pushinteger(L, sd.cnt);
  return 4;
}

static int prof_nogcperiod (lua_State *L) {
  lua_Statdata sd;
  lua_nogcperiod(L, &sd);
  lua_pushnumber(L, sd.max);
  lua_pushnumber(L, sd.min);
  lua_pushnumber(L, sd.avg);
  lua_pushinteger(L, sd.cnt);
  return 4;
}

static int prof_heapinfo (lua_State *L) {
  lua_Heapinfo hi;
  lua_heapinfo(L, &hi);
  lua_pushnumber(L, hi.allocbytes);
  lua_pushnumber(L, hi.freebytes);
  lua_pushnumber(L, hi.totalbytes);
  lua_pushinteger(L, hi.totalcount);
  return 4;
}

static int prof_tableinfo (lua_State *L) {
  lua_Objectinfo oi;
  lua_tableinfo(L, &oi);
  lua_pushinteger(L, oi.count);
  lua_pushnumber(L, oi.bytes);
  return 2;
}

static int prof_protoinfo (lua_State *L) {
  lua_Objectinfo oi;
  lua_protoinfo(L, &oi);
  lua_pushinteger(L, oi.count);
  lua_pushnumber(L, oi.bytes);
  return 2;
}

static int prof_closureinfo (lua_State *L) {
  lua_Objectinfo oi;
  lua_closureinfo(L, &oi);
  lua_pushinteger(L, oi.count);
  lua_pushnumber(L, oi.bytes);
  lua_cclosureinfo(L, &oi);
  lua_pushinteger(L, oi.count);
  lua_pushnumber(L, oi.bytes);
  lua_lclosureinfo(L, &oi);
  lua_pushinteger(L, oi.count);
  lua_pushnumber(L, oi.bytes);
  return 6;
}

static int prof_threadinfo (lua_State *L) {
  lua_Objectinfo oi;
  lua_threadinfo(L, &oi);
  lua_pushinteger(L, oi.count);
  lua_pushnumber(L, oi.bytes);
  return 2;
}

static int prof_upvalinfo (lua_State *L) {
  lua_Objectinfo oi;
  lua_upvalinfo(L, &oi);
  lua_pushinteger(L, oi.count);
  lua_pushnumber(L, oi.bytes);
  lua_openupvalinfo(L, &oi);
  lua_pushinteger(L, oi.count);
  lua_pushnumber(L, oi.bytes);
  lua_closeupvalinfo(L, &oi);
  lua_pushinteger(L, oi.count);
  lua_pushnumber(L, oi.bytes);
  return 6;
}

static int prof_userdatainfo (lua_State *L) {
  lua_Objectinfo oi;
  lua_userdatainfo(L, &oi);
  lua_pushinteger(L, oi.count);
  lua_pushnumber(L, oi.bytes);
  return 2;
}

static int prof_stringinfo (lua_State *L) {
  lua_Objectinfo oi;
  lua_stringinfo(L, &oi);
  lua_pushinteger(L, oi.count);
  lua_pushnumber(L, oi.bytes);
  return 2;
}

static int prof_nanosecond (lua_State *L) {
  lua_pushnumber(L, lua_nanosecond(L));
  return 1;
}

static int prof_help (lua_State *L) {
  lua_pushliteral(L, ""
    "stackresizecount(thread): resize count of stack array in the thread.\n"
    "ciresizecount(thread):    resize count of callinfo array in the thread.\n"
    "tableresizecount(table):  resize count of table inner data structure.\n"
    "gcsteps():                maximum/minimum/average gc steps in one gc\n"
    "                          period and number of gc period.\n"
    "marksteps():              maximum/minimum/average mark steps in one gc\n"
    "                          period and number of gc period.\n"
    "sweepstringsteps():       maximum/minimum/average sweepstring steps in\n"
    "                          on gc period and number of gc period.\n"
    "sweepsteps():             maximum/minumum/average sweep steps in one gc\n"
    "                          period and number of gc period.\n"
    "finalizesteps():          maximum/minumum/average finalize steps in one gc\n"
    "                          period and number of gc period.\n"
    "gcperiod():               maximum/minumum/average time of gc period in\n"
    "                          nanoseconds and number of gc period.\n"
    "nogcperiod():             maximum/minumum/average time between gc period in\n"
    "                          nanoseconds and number of no gc period.\n"
    "heapinfo():               allocated bytes/freed bytes/total bytes/total\n"
    "                          number of heap objects.\n"
    "tableinfo():              table object number/total table object size.\n"
    "protoinfo():              proto object number/tatal proto object size.\n"
    "closureinfo():            closure number/total closure size/\n"
    "                          c closure number/total c closure size/\n"
    "                          lua closure number/total lua closure size.\n"
    "threadinfo():             thread object number/total thread size.\n"
    "upvalinfo():              upval number/total upval size/\n"
    "                          open upval number/open upval size/\n"
    "                          closed upval number/closed upval size.\n"
    "userdatainfo():           userdata number/total userdata object size.\n"
    "stringinfo():             string number/total string object size.\n"
    "nanosecond():             nanoseconds from clock_gettime(POSIX) or \n"
    "                          QueryPerformanceCounter(Windows).\n"
    "help():                   display this help message.\n");

  return 1;
}

static const luaL_Reg proflib[] = {
  {"stackresizecount",   prof_stackresizecount},
  {"ciresizecount",      prof_ciresizecount},
  {"tableresizecount",   prof_tableresizecount},
  {"gcsteps",            prof_gcsteps},
  {"marksteps",          prof_marksteps},
  {"sweepstringsteps",   prof_sweepstringsteps},
  {"sweepsteps",         prof_sweepsteps},
  {"finalizesteps",      prof_finalizesteps},
  {"gcperiod",           prof_gcperiod},
  {"nogcperiod",         prof_nogcperiod},
  {"heapinfo",           prof_heapinfo},
  {"tableinfo",          prof_tableinfo},
  {"protoinfo",          prof_protoinfo},
  {"closureinfo",        prof_closureinfo},
  {"threadinfo",         prof_threadinfo},
  {"upvalinfo",          prof_upvalinfo},
  {"userdatainfo",       prof_userdatainfo},
  {"stringinfo",         prof_stringinfo},
  {"nanosecond",         prof_nanosecond},
  {"help",               prof_help},
  {NULL, NULL}
};


/*
** Open profiling library
*/
LUALIB_API int luaopen_profile (lua_State *L) {
  luaL_register(L, LUA_PROFLIBNAME, proflib);
  return 1;
}

#endif /* LUA_PROFILE */
