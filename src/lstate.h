/*
** $Id: lstate.h,v 2.24.1.2 2008/01/03 15:20:39 roberto Exp $
** Global State
** See Copyright Notice in lua.h
*/

#ifndef lstate_h
#define lstate_h

#include "lua.h"

#include "lobject.h"
#include "ltm.h"
#include "lzio.h"



struct lua_longjmp;  /* defined in ldo.c */


/* table of globals */
#define gt(L)	(&L->l_gt)

/* registry */
#define registry(L)	(&G(L)->l_registry)


/* extra stack space to handle TM calls and some other extras */
#define EXTRA_STACK   5


#define BASIC_CI_SIZE           8

#define BASIC_STACK_SIZE        (2*LUA_MINSTACK)



typedef struct stringtable {
  GCObject **hash;
  lu_int32 nuse;  /* number of elements */
  int size;
} stringtable;


/*
** informations about a call
*/
typedef struct CallInfo {
  StkId base;  /* base for this function */
  StkId func;  /* function index in the stack */
  StkId	top;  /* top for this function */
  const Instruction *savedpc;
  int nresults;  /* expected number of results from this function */
  int tailcalls;  /* number of tail calls lost under this entry */
} CallInfo;



#define curr_func(L)	(clvalue(L->ci->func))
#define ci_func(ci)	(clvalue((ci)->func))
#define f_isLua(ci)	(!ci_func(ci)->c.isC)
#define isLua(ci)	(ttisfunction((ci)->func) && f_isLua(ci))

#if LUA_PROFILE
/* statistic data */
typedef struct statdata {
  lua_Number acc; /* temporary accumulator */
  lua_Number max; /* maximum value */
  lua_Number min; /* minimum value */
  lua_Number avg; /* average value */
  lua_Integer cnt; /* count */
} statdata;

/* Initialize the statistics structure */
#define statinit(sd) \
  { statdata *p=(sd); p->acc=p->max=p->min=p->avg=0.0; p->cnt=0;}

/* Statistics period to count the occurrence of actions */
#define statacc(sd) { (sd)->acc++; }
#define statloop(sd) \
  { statdata *p=(sd); lua_Number acc=p->acc; \
    p->max = p->max < acc ? acc : p->max; \
    p->min = p->min > acc ? acc : p->min; \
    p->avg = (p->avg*p->cnt+acc)/(p->cnt+1); \
    p->cnt++; p->acc = 0.0; }

/* Statistics period to count delta value of begin and end */
#define statacc1(sd, v) { (sd)->acc = (v); }
#define statloop1(sd,v) \
  { statdata *p=(sd); lua_Number v1=(v); lua_Number acc=v1-p->acc; \
    p->max = p->max < acc ? acc : p->max; \
    p->min = p->min > acc ? acc : p->min; \
    p->avg = (p->avg*p->cnt+acc)/(p->cnt+1); \
    p->cnt++; p->acc = v1; }


/* performance log facilities */
#include <stdio.h>
/*
 * log level:
 * 0 - no log
 * 1 - lua begin
 *     lua end
 *     enable log
 *     disable log
 *     gc begin
 *     gc end
 * 2 - gc atomic begin
 *     gc atomic end
 * 3 - gc sweepstring begin
 *     gc sweep begin
 *     gc finalize begin
 *     barrier back
 */
#define lualog(L, level, msg, ...) \
  { if ((level) <= G(L)->loglevel) \
      fprintf(stderr, "[lualog] %14.0lf ns - " msg "\n", \
	lua_nanosecond(L), ##__VA_ARGS__); }

#else

#define lualog(L, level, msg, ...) (void)

#endif

/*
** `global state', shared by all threads of this state
*/
typedef struct global_State {
  stringtable strt;  /* hash table for strings */
  lua_Alloc frealloc;  /* function to reallocate memory */
  void *ud;         /* auxiliary data to `frealloc' */
  lu_byte currentwhite;
  lu_byte gcstate;  /* state of garbage collector */
  int sweepstrgc;  /* position of sweep in `strt' */
  GCObject *rootgc;  /* list of all collectable objects */
  GCObject **sweepgc;  /* position of sweep in `rootgc' */
  GCObject *gray;  /* list of gray objects */
  GCObject *grayagain;  /* list of objects to be traversed atomically */
  GCObject *weak;  /* list of weak tables (to be cleared) */
  GCObject *tmudata;  /* last element of list of userdata to be GC */
  Mbuffer buff;  /* temporary buffer for string concatentation */
  lu_mem GCthreshold;
  lu_mem totalbytes;  /* number of bytes currently allocated */
  lu_mem estimate;  /* an estimate of number of bytes actually in use */
  lu_mem gcdept;  /* how much GC is `behind schedule' */
  int gcpause;  /* size of pause between successive GCs */
  int gcstepmul;  /* GC `granularity' */
  lua_CFunction panic;  /* to be called in unprotected errors */
  TValue l_registry;
  struct lua_State *mainthread;
  UpVal uvhead;  /* head of double-linked list of all open upvalues */
  struct Table *mt[NUM_TAGS];  /* metatables for basic types */
  TString *tmname[TM_N];  /* array with tag-method names */
#if LUA_PROFILE
  /* GC related profiling data */
  statdata gcsteps;
  statdata marksteps;
  statdata sweepstringsteps;
  statdata sweepsteps;
  statdata finalizesteps;
  statdata gcperiod;
  statdata nogcperiod;
  lu_mem allocbytes;
  lu_mem freebytes;
  lu_mem tablebytes;
  lu_mem protobytes;
  lu_mem lclosurebytes;
  lu_mem cclosurebytes;
  lu_mem threadbytes;
  lu_mem upvalbytes;
  lu_mem udatabytes;
  lu_mem stringbytes;
  lua_Integer tablecount;
  lua_Integer protocount;
  lua_Integer lclosurecount;
  lua_Integer cclosurecount;
  lua_Integer threadcount;
  lua_Integer openupvalcount;
  lua_Integer closeupvalcount;
  lua_Integer udatacount;
  lua_Integer stringcount;
  lua_Number clockfreq; /* Cycles per nanosecond */
  lua_Number starttime; /* start time in nanosecond */
  int loglevel; /* profiling log level, 0: no log, 1-9: verbose level */
#endif
} global_State;


/*
** `per thread' state
*/
struct lua_State {
#if LUA_REFCOUNT
  TraversableCommonHeader;
#else
  CommonHeader;
#endif
  lu_byte status;
  StkId top;  /* first free slot in the stack */
  StkId base;  /* base of current function */
  global_State *l_G;
  CallInfo *ci;  /* call info for current function */
  const Instruction *savedpc;  /* `savedpc' of current function */
  StkId stack_last;  /* last free slot in the stack */
  StkId stack;  /* stack base */
  CallInfo *end_ci;  /* points after end of ci array*/
  CallInfo *base_ci;  /* array of CallInfo's */
  int stacksize;
  int size_ci;  /* size of array `base_ci' */
  unsigned short nCcalls;  /* number of nested C calls */
  unsigned short baseCcalls;  /* nested C calls when resuming coroutine */
  lu_byte hookmask;
  lu_byte allowhook;
  int basehookcount;
  int hookcount;
  lua_Hook hook;
  TValue l_gt;  /* table of globals */
  TValue env;  /* temporary place for environments */
  GCObject *openupval;  /* list of open upvalues in this stack */
#if !LUA_REFCOUNT
  GCObject *gclist;
#endif
  struct lua_longjmp *errorJmp;  /* current error recover point */
  ptrdiff_t errfunc;  /* current error handling function (stack index) */
#if LUA_PROFILE
  lua_Integer stackresizecount;
  lua_Integer ciresizecount;
#endif
};


#define G(L)	(L->l_G)


/*
** Union of all collectable objects
*/
union GCObject {
  GCheader gch;
#if LUA_REFCOUNT
  TraversableGCheader tgch;
#endif
  union TString ts;
  union Udata u;
  union Closure cl;
  struct Table h;
  struct Proto p;
  struct UpVal uv;
  struct lua_State th;  /* thread */
};


/* macros to convert a GCObject into a specific value */
#define rawgco2ts(o)	check_exp((o)->gch.tt == LUA_TSTRING, &((o)->ts))
#define gco2ts(o)	(&rawgco2ts(o)->tsv)
#define rawgco2u(o)	check_exp((o)->gch.tt == LUA_TUSERDATA, &((o)->u))
#define gco2u(o)	(&rawgco2u(o)->uv)
#define gco2cl(o)	check_exp((o)->gch.tt == LUA_TFUNCTION, &((o)->cl))
#define gco2h(o)	check_exp((o)->gch.tt == LUA_TTABLE, &((o)->h))
#define gco2p(o)	check_exp((o)->gch.tt == LUA_TPROTO, &((o)->p))
#define gco2uv(o)	check_exp((o)->gch.tt == LUA_TUPVAL, &((o)->uv))
#define ngcotouv(o) \
	check_exp((o) == NULL || (o)->gch.tt == LUA_TUPVAL, &((o)->uv))
#define gco2th(o)	check_exp((o)->gch.tt == LUA_TTHREAD, &((o)->th))

/* macro to convert any Lua object into a GCObject */
#define obj2gco(v)	(cast(GCObject *, (v)))


LUAI_FUNC lua_State *luaE_newthread (lua_State *L);
LUAI_FUNC void luaE_freethread (lua_State *L, lua_State *L1);

#endif

