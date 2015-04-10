/*
** $Id: lstate.c,v 2.36.1.2 2008/01/03 15:20:39 roberto Exp $
** Global State
** See Copyright Notice in lua.h
*/


#include <stddef.h>

#define lstate_c
#define LUA_CORE

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "llex.h"
#include "lmem.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"


#define state_size(x)	(sizeof(x) + LUAI_EXTRASPACE)
#define fromstate(l)	(cast(lu_byte *, (l)) - LUAI_EXTRASPACE)
#define tostate(l)   (cast(lua_State *, cast(lu_byte *, l) + LUAI_EXTRASPACE))


/*
** Main thread combines a thread state and the global state
*/
typedef struct LG {
  lua_State l;
  global_State g;
} LG;
  


static void stack_init (lua_State *L1, lua_State *L) {
  /* initialize CallInfo array */
  L1->base_ci = luaM_newvector(L, BASIC_CI_SIZE, CallInfo);
  L1->ci = L1->base_ci;
  L1->size_ci = BASIC_CI_SIZE;
  L1->end_ci = L1->base_ci + L1->size_ci - 1;
  /* initialize stack array */
  L1->stack = luaM_newvector(L, BASIC_STACK_SIZE + EXTRA_STACK, TValue);
  L1->stacksize = BASIC_STACK_SIZE + EXTRA_STACK;
  L1->top = L1->stack;
  L1->stack_last = L1->stack+(L1->stacksize - EXTRA_STACK)-1;
#if LUA_REFCOUNT
  {
    TValue *v, *e = L1->stack + L1->stacksize;
    for (v=L1->stack; v<e; v++)
      setnilvalue2n(v);
  }
#endif 
  /* initialize first ci */
  L1->ci->func = L1->top;
  setnilvalue2n(L1->top++);  /* `function' entry for this `ci' */
  L1->base = L1->ci->base = L1->top;
  L1->ci->top = L1->top + LUA_MINSTACK;
#if LUA_PROFILE
  G(L)->threadbytes += L1->stacksize*sizeof(TValue) \
			+ L1->size_ci*sizeof(CallInfo);
#endif
}


static void freestack (lua_State *L, lua_State *L1) {
  luaM_freearray(L, L1->base_ci, L1->size_ci, CallInfo);
  luaM_freearray(L, L1->stack, L1->stacksize, TValue);
#if LUA_PROFILE
  G(L)->threadbytes -= L1->stacksize*sizeof(TValue) \
			+ L1->size_ci*sizeof(CallInfo);
#endif
}


/*
** open parts that may cause memory-allocation errors
*/
static void f_luaopen (lua_State *L, void *ud) {
  global_State *g = G(L);
  UNUSED(ud);
  stack_init(L, L);  /* init stack */
  sethvalue2n(L, gt(L), luaH_new(L, 0, 2));  /* table of globals */
  sethvalue2n(L, registry(L), luaH_new(L, 0, 2));  /* registry */
  luaS_resize(L, MINSTRTABSIZE);  /* initial size of string table */
  luaT_init(L);
  luaX_init(L);
  luaS_fix(luaS_newliteral(L, MEMERRMSG));
  g->GCthreshold = 4*g->totalbytes;
}


static void preinit_state (lua_State *L, global_State *g) {
  G(L) = g;
  L->stack = NULL;
  L->stacksize = 0;
  L->errorJmp = NULL;
  L->hook = NULL;
  L->hookmask = 0;
  L->basehookcount = 0;
  L->allowhook = 1;
  resethookcount(L);
  L->openupval = NULL;
  L->size_ci = 0;
  L->nCcalls = L->baseCcalls = 0;
  L->status = 0;
  L->base_ci = L->ci = NULL;
  L->savedpc = NULL;
  L->errfunc = 0;
  setnilvalue2n(gt(L));
#if LUA_REFCOUNT
  setnilvalue2n(&L->env);
#endif
#if LUA_PROFILE
  L->stackresizecount = 0;
  L->ciresizecount = 0;
#endif
}


static void close_state (lua_State *L) {
  global_State *g = G(L);
  lualog(L, 1, "lua closed");
  luaF_close(L, L->stack);  /* close all upvalues for this thread */
  luaC_freeall(L);  /* collect all objects */
  lua_assert(g->rootgc == obj2gco(L));
  lua_assert(g->strt.nuse == 0);
  luaM_freearray(L, G(L)->strt.hash, G(L)->strt.size, TString *);
  luaZ_freebuffer(L, &g->buff);
  freestack(L, L);
  lua_assert(g->totalbytes == sizeof(LG));
  (*g->frealloc)(g->ud, fromstate(L), state_size(LG), 0);
}


lua_State *luaE_newthread (lua_State *L) {
  lua_State *L1 = tostate(luaM_malloc(L, state_size(lua_State)));
  luaC_link(L, obj2gco(L1), LUA_TTHREAD);
#if LUA_REFCOUNT
  L1->ref = 0;
#endif /* LUA_REFCOUNT */
  preinit_state(L1, G(L));
  stack_init(L1, L);  /* init stack */
  setobj2n(L, gt(L1), gt(L));  /* share table of globals */
  L1->hookmask = L->hookmask;
  L1->basehookcount = L->basehookcount;
  L1->hook = L->hook;
  resethookcount(L1);
  lua_assert(iswhite(obj2gco(L1)));
#if LUA_PROFILE
  G(L)->threadcount++;
  G(L)->threadbytes += state_size(lua_State);
  lualog(L, 1, "create thread");
#endif
  return L1;
}


void luaE_freethread (lua_State *L, lua_State *L1) {
  luaF_close(L1, L1->stack);  /* close all upvalues for this thread */
  lua_assert(L1->openupval == NULL);
  luai_userstatefree(L1);
  freestack(L, L1);
#if LUA_PROFILE
  G(L)->threadcount--;
  G(L)->threadbytes -= state_size(lua_State);
  lualog(L, 1, "free thread");
#endif
  luaM_freemem(L, fromstate(L1), state_size(lua_State));
}

#if LUA_PROFILE
#if defined(LUA_WIN)
#include <windows.h>
static lua_Number clockfreq () {
  LARGE_INTEGER freq;
  if (QueryPerformanceFrequency(&freq))
    return (lua_Number)freq.QuadPart/(1000*1000*1000);
  return (lua_Number)0;
}
#endif
#endif


LUA_API lua_State *lua_newstate (lua_Alloc f, void *ud) {
  int i;
  lua_State *L;
  global_State *g;
  void *l = (*f)(ud, NULL, 0, state_size(LG));
  if (l == NULL) return NULL;
  L = tostate(l);
  g = &((LG *)L)->g;
  L->next = NULL;
  L->tt = LUA_TTHREAD;
  g->currentwhite = bit2mask(WHITE0BIT, FIXEDBIT);
  L->marked = luaC_white(g);
  set2bits(L->marked, FIXEDBIT, SFIXEDBIT);
  preinit_state(L, g);
  g->frealloc = f;
  g->ud = ud;
  g->mainthread = L;
  g->uvhead.u.l.prev = &g->uvhead;
  g->uvhead.u.l.next = &g->uvhead;
  g->GCthreshold = 0;  /* mark it as unfinished state */
  g->strt.size = 0;
  g->strt.nuse = 0;
  g->strt.hash = NULL;
  setnilvalue2n(registry(L));
  luaZ_initbuffer(L, &g->buff);
  g->panic = NULL;
  g->gcstate = GCSpause;
  g->rootgc = obj2gco(L);
  g->sweepstrgc = 0;
  g->sweepgc = &g->rootgc;
  g->gray = NULL;
  g->grayagain = NULL;
  g->weak = NULL;
  g->tmudata = NULL;
  g->totalbytes = sizeof(LG);
  g->gcpause = LUAI_GCPAUSE;
  g->gcstepmul = LUAI_GCMUL;
  g->gcdept = 0;
#if LUA_PROFILE
  statinit(&g->gcsteps);
  statinit(&g->marksteps);
  statinit(&g->sweepstringsteps);
  statinit(&g->sweepsteps);
  statinit(&g->finalizesteps);
  statinit(&g->gcperiod);
  statinit(&g->nogcperiod);
  statacc1(&g->nogcperiod, 0);
  g->allocbytes = state_size(LG);
  g->freebytes = 0;
  g->tablebytes = 0;
  g->protobytes = 0;
  g->lclosurebytes = 0;
  g->cclosurebytes = 0;
  g->threadbytes = state_size(LG);
  g->upvalbytes = 0;
  g->udatabytes = 0;
  g->stringbytes = 0;
  g->tablecount = 0;
  g->protocount = 0;
  g->lclosurecount = 0;
  g->cclosurecount = 0;
  g->threadcount = 1;
  g->openupvalcount = 0;
  g->closeupvalcount = 0;
  g->udatacount = 0;
  g->stringcount = 0;
#if defined(LUA_WIN)
  g->clockfreq = clockfreq();
#else
  g->clockfreq = (lua_Number)0;
#endif
  g->starttime = (lua_Number)0;
  g->loglevel = 0;
  
  /* on Windows, lua_nanosecond needs g->clockfreq */
  /* the first invocation of lua_nanosecond() should be used to
   * initialize g->starttime which should be 0 then */
  g->starttime = lua_nanosecond(L);

  lualog(L, 1, "lua created");
#endif

  for (i=0; i<NUM_TAGS; i++) g->mt[i] = NULL;
  if (luaD_rawrunprotected(L, f_luaopen, NULL) != 0) {
    /* memory allocation error: free partial state */
    close_state(L);
    L = NULL;
  }
  else
    luai_userstateopen(L);
  return L;
}


static void callallgcTM (lua_State *L, void *ud) {
  UNUSED(ud);
  luaC_callGCTM(L);  /* call GC metamethods for all udata */
}


LUA_API void lua_close (lua_State *L) {
  L = G(L)->mainthread;  /* only the main thread can be closed */
  lua_lock(L);
  luaF_close(L, L->stack);  /* close all upvalues for this thread */
  luaC_separateudata(L, 1);  /* separate udata that have GC metamethods */
  L->errfunc = 0;  /* no error function during GC metamethods */
  do {  /* repeat until no more errors */
    L->ci = L->base_ci;
    L->base = L->top = L->ci->base;
    L->nCcalls = L->baseCcalls = 0;
  } while (luaD_rawrunprotected(L, callallgcTM, NULL) != 0);
  lua_assert(G(L)->tmudata == NULL);
  luai_userstateclose(L);
  close_state(L);
}

