/*
** $Id: lobject.h,v 2.20.1.2 2008/08/06 13:29:48 roberto Exp $
** Type definitions for Lua objects
** See Copyright Notice in lua.h
*/


#ifndef lobject_h
#define lobject_h


#include <stdarg.h>


#include "llimits.h"
#include "lua.h"


/* tags for values visible from Lua */
#define LAST_TAG	LUA_TTHREAD

#define NUM_TAGS	(LAST_TAG+1)


/*
** Extra tags for non-values
*/
#define LUA_TPROTO	(LAST_TAG+1)
#define LUA_TUPVAL	(LAST_TAG+2)
#define LUA_TDEADKEY	(LAST_TAG+3)


/*
** Union of all collectable objects
*/
typedef union GCObject GCObject;


/*
** Common Header for all collectable objects (in macro form, to be
** included in other objects)
*/
#if LUA_REFCOUNT
#define CommonHeader	GCObject *next; GCObject *prev; long ref; \
                        lu_byte tt; lu_byte marked

#define TraversableCommonHeader \
  CommonHeader; GCObject *gcnext; GCObject *gcprev

#else /* !LUA_REFCOUNT */
#define CommonHeader	GCObject *next; lu_byte tt; lu_byte marked 

#endif /* LUA_REFCOUNT */


/*
** Common header in struct form
*/
typedef struct GCheader {
  CommonHeader;
} GCheader;

#if LUA_REFCOUNT
/*
** Traversable GC Object Common header in struct form
*/
typedef struct TraversableGCheader {
  TraversableCommonHeader;
} TraversableGCheader;

#endif



/*
** Union of all Lua values
*/
typedef union {
  GCObject *gc;
  void *p;
  lua_Number n;
  int b;
} Value;


/*
** Tagged Values
*/

#define TValuefields	Value value; int tt

typedef struct lua_TValue {
  TValuefields;
} TValue;


/* Macros to test type */
#define ttisnil(o)	(ttype(o) == LUA_TNIL)
#define ttisnumber(o)	(ttype(o) == LUA_TNUMBER)
#define ttisstring(o)	(ttype(o) == LUA_TSTRING)
#define ttistable(o)	(ttype(o) == LUA_TTABLE)
#define ttisfunction(o)	(ttype(o) == LUA_TFUNCTION)
#define ttisboolean(o)	(ttype(o) == LUA_TBOOLEAN)
#define ttisuserdata(o)	(ttype(o) == LUA_TUSERDATA)
#define ttisthread(o)	(ttype(o) == LUA_TTHREAD)
#define ttislightuserdata(o)	(ttype(o) == LUA_TLIGHTUSERDATA)

/* Macros to access values */
#define ttype(o)	((o)->tt)
#define gcvalue(o)	check_exp(iscollectable(o), (o)->value.gc)
#define pvalue(o)	check_exp(ttislightuserdata(o), (o)->value.p)
#define nvalue(o)	check_exp(ttisnumber(o), (o)->value.n)
#define rawtsvalue(o)	check_exp(ttisstring(o), &(o)->value.gc->ts)
#define tsvalue(o)	(&rawtsvalue(o)->tsv)
#define rawuvalue(o)	check_exp(ttisuserdata(o), &(o)->value.gc->u)
#define uvalue(o)	(&rawuvalue(o)->uv)
#define clvalue(o)	check_exp(ttisfunction(o), &(o)->value.gc->cl)
#define hvalue(o)	check_exp(ttistable(o), &(o)->value.gc->h)
#define bvalue(o)	check_exp(ttisboolean(o), (o)->value.b)
#define thvalue(o)	check_exp(ttisthread(o), &(o)->value.gc->th)

#define l_isfalse(o)	(ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))

/*
** for internal debug only
*/
#define checkconsistency(obj) \
  lua_assert(!iscollectable(obj) || (ttype(obj) == (obj)->value.gc->gch.tt))

#define checkliveness(g,obj) \
  lua_assert(!iscollectable(obj) || \
  ((ttype(obj) == (obj)->value.gc->gch.tt) && !isdead(g, (obj)->value.gc)))


#if LUA_REFCOUNT

/* begin is the first one to be check
 * end is one beyond the last one to be check
 */
int rangeisnil (const TValue *begin, const TValue *end);

#define checkrangeisnil(begin, end) lua_assert(rangeisnil((begin), (end)))

void luarc_releaseobj (lua_State *L, GCObject *obj);

/* Internal RefCount manipulation macros */
#define luarc_addobjref(obj) (obj->gch.ref++)

#define luarc_addvalref(val) (val->value.gc->gch.ref++)

#define luarc_subobjref(L,obj) \
  { if (--obj->gch.ref <= 0) luarc_releaseobj(L,obj); }

#define luarc_subvalref(L,val) \
  { if (--val->value.gc->gch.ref <= 0) luarc_releaseobj(L,val->value.gc); }

/* do not use i_v as temporary local variable, which may be used outside */
#define luarc_addref(val) \
  { TValue *v1=(TValue *)(val); \
    if (iscollectable(v1)) luarc_addvalref(v1); }

#define luarc_subref(L,val) \
  { TValue *v1=(TValue *)(val); \
    if (iscollectable(v1) && --v1->value.gc->gch.ref <= 0) \
      luarc_releaseobj(L,v1->value.gc); }

#define luarc_addstringref(obj) luarc_addobjref(cast(GCObject *, (obj)))
#define luarc_addudataref(obj)  luarc_addobjref(cast(GCObject *, (obj)))
#define luarc_addclosureref(obj)  luarc_addobjref(cast(GCObject *, (obj)))
#define luarc_addtableref(obj)  luarc_addobjref(cast(GCObject *, (obj)))
#define luarc_addprotoref(obj)  luarc_addobjref(cast(GCObject *, (obj)))
#define luarc_addupvalref(obj)  luarc_addobjref(cast(GCObject *, (obj)))
#define luarc_addthreadref(obj) luarc_addobjref(cast(GCObject *, (obj)))

#define luarc_substringref(L,obj) \
  { GCObject *i_o=cast(GCObject *, (obj)); luarc_subobjref(L, i_o); }

#define luarc_subudataref(L,obj) \
  { GCObject *i_o=cast(GCObject *, (obj)); luarc_subobjref(L, i_o); }

#define luarc_subclosureref(L,obj) \
  { GCObject *i_o=cast(GCObject *, (obj)); luarc_subobjref(L, i_o); }

#define luarc_subtableref(L,obj) \
  { GCObject *i_o=cast(GCObject *, (obj)); luarc_subobjref(L, i_o); }

#define luarc_subprotoref(L,obj) \
  { GCObject *i_o=cast(GCObject *, (obj)); luarc_subobjref(L, i_o); }

#define luarc_subupvalref(L,obj) \
  { GCObject *i_o=cast(GCObject *, (obj)); luarc_subobjref(L, i_o); }

#define luarc_subthreadref(L,obj) \
  { GCObject *i_o=cast(GCObject *, (obj)); luarc_subobjref(L, i_o); }


/* Macros to set values */

/* Set nil to obj first, then subref it.
 * Subref may cause object release and GCTM execution
 * which in turn may check this tvalue.
 */
#define setnilvalue(L,obj) \
  { TValue *i_v=(obj); TValue dup=*i_v; \
    i_v->tt=LUA_TNIL; luarc_subref(L,&dup); }

#define setnvalue(L,obj,x) \
  { TValue *i_v=(obj); TValue dup=*i_v; \
    i_v->value.n=(x); i_v->tt=LUA_TNUMBER; \
    luarc_subref(L,&dup); }

#define setpvalue(L,obj,x) \
  { TValue *i_v=(obj); TValue dup=*i_v; \
    i_v->value.p=(x); i_v->tt=LUA_TLIGHTUSERDATA; \
    luarc_subref(L,&dup); }

#define setbvalue(L,obj,x) \
  { TValue *i_v=(obj); TValue dup=*i_v; \
    i_v->value.b=(x); i_v->tt=LUA_TBOOLEAN; \
    luarc_subref(L,&dup); }

#define setsvalue(L,obj,x) \
  { TValue *i_v=(obj); GCObject *i_o=cast(GCObject *, (x)); \
    TValue dup=*i_v; i_v->value.gc=i_o; i_v->tt=LUA_TSTRING; \
    luarc_addobjref(i_o); luarc_subref(L,&dup); \
    checkliveness(G(L),i_v); }

#define setuvalue(L,obj,x) \
  { TValue *i_v=(obj); GCObject *i_o=cast(GCObject *, (x)); \
    TValue dup=*i_v; i_v->value.gc=i_o; i_v->tt=LUA_TUSERDATA; \
    luarc_addobjref(i_o); luarc_subref(L,&dup); \
    checkliveness(G(L),i_v); }

#define setthvalue(L,obj,x) \
  { TValue *i_v=(obj); GCObject *i_o=cast(GCObject *, (x)); \
    TValue dup=*i_v; i_v->value.gc=i_o; i_v->tt=LUA_TTHREAD; \
    luarc_addobjref(i_o); luarc_subref(L,&dup); \
    checkliveness(G(L),i_v); }

#define setclvalue(L,obj,x) \
  { TValue *i_v=(obj); GCObject *i_o=cast(GCObject *, (x)); \
    TValue dup=*i_v; i_v->value.gc=i_o; i_v->tt=LUA_TFUNCTION; \
    luarc_addobjref(i_o); luarc_subref(L,&dup); \
    checkliveness(G(L),i_v); }

#define sethvalue(L,obj,x) \
  { TValue *i_v=(obj); GCObject *i_o=cast(GCObject *, (x)); \
    TValue dup=*i_v; i_v->value.gc=i_o; i_v->tt=LUA_TTABLE; \
    luarc_addobjref(i_o); luarc_subref(L,&dup); \
    checkliveness(G(L),i_v); }

#define setptvalue(L,obj,x) \
  { TValue *i_v=(obj); GCObject *i_o=cast(GCObject *, (x)); \
    TValue dup=*i_v; i_v->value.gc=i_o; i_v->tt=LUA_TPROTO; \
    luarc_addobjref(i_o); luarc_subref(L,&dup); \
    checkliveness(G(L),i_v); }

#define setobj(L,obj1,obj2) \
  { TValue *o2=(TValue *)(obj2); TValue *o1=(obj1); \
    TValue dup=*o1; o1->value = o2->value; o1->tt=o2->tt; \
    luarc_addref(o2); luarc_subref(L,&dup); \
    checkliveness(G(L),o1); }

/* to new object */
#define setnilvalue2n(obj) ((obj)->tt=LUA_TNIL)

#define setnvalue2n(obj,x) \
  { TValue *i_o=(obj); i_o->value.n=(x); i_o->tt=LUA_TNUMBER; }

#define setpvalue2n(obj,x) \
  { TValue *i_o=(obj); i_o->value.p=(x); i_o->tt=LUA_TLIGHTUSERDATA; }

#define setbvalue2n(obj,x) \
  { TValue *i_o=(obj); i_o->value.b=(x); i_o->tt=LUA_TBOOLEAN; }

#define setsvalue2n(L,obj,x) \
  { TValue *i_v=(obj); GCObject *i_o=cast(GCObject *, (x)); \
    luarc_addobjref(i_o); i_v->value.gc=i_o; i_v->tt=LUA_TSTRING; \
    checkliveness(G(L),i_v); }

#define setuvalue2n(L,obj,x) \
  { TValue *i_v=(obj); GCObject *i_o=cast(GCObject *, (x)); \
    luarc_addobjref(i_o); i_v->value.gc=i_o; i_v->tt=LUA_TUSERDATA; \
    checkliveness(G(L),i_v); }

#define setthvalue2n(L,obj,x) \
  { TValue *i_v=(obj); GCObject *i_o=cast(GCObject *, (x)); \
    luarc_addobjref(i_o); i_v->value.gc=i_o; i_v->tt=LUA_TTHREAD; \
    checkliveness(G(L),i_v); }

#define setclvalue2n(L,obj,x) \
  { TValue *i_v=(obj); GCObject *i_o=cast(GCObject *, (x)); \
    luarc_addobjref(i_o); i_v->value.gc=i_o; i_v->tt=LUA_TFUNCTION; \
    checkliveness(G(L),i_v); }

#define sethvalue2n(L,obj,x) \
  { TValue *i_v=(obj); GCObject *i_o=cast(GCObject *, (x)); \
    luarc_addobjref(i_o); i_v->value.gc=i_o; i_v->tt=LUA_TTABLE; \
    checkliveness(G(L),i_v); }

#define setptvalue2n(L,obj,x) \
  { TValue *i_v=(obj); GCObject *i_o=cast(GCObject *, (x)); \
    luarc_addobjref(i_o); i_v->value.gc=i_o; i_v->tt=LUA_TPROTO; \
    checkliveness(G(L),i_v); }

#define setobj2n(L,obj1,obj2) \
  { TValue *o2=(TValue *)(obj2); TValue *o1=(obj1); \
    o1->value = o2->value; o1->tt=o2->tt; \
    luarc_addref(o2); checkliveness(G(L),o1); }

/* no ref count version of set function, sometimes these are useful */
#define setnilvaluenrc(obj) ((obj)->tt=LUA_TNIL)

#define setnvaluenrc(obj,x) \
  { TValue *i_o=(obj); i_o->value.n=(x); i_o->tt=LUA_TNUMBER; }

#define setpvaluenrc(obj,x) \
  { TValue *i_o=(obj); i_o->value.p=(x); i_o->tt=LUA_TLIGHTUSERDATA; }

#define setbvaluenrc(obj,x) \
  { TValue *i_o=(obj); i_o->value.b=(x); i_o->tt=LUA_TBOOLEAN; }

#define setsvaluenrc(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TSTRING; \
    checkliveness(G(L),i_o); }

#define setuvaluenrc(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TUSERDATA; \
    checkliveness(G(L),i_o); }

#define setthvaluenrc(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TTHREAD; \
    checkliveness(G(L),i_o); }

#define setclvaluenrc(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TFUNCTION; \
    checkliveness(G(L),i_o); }

#define sethvaluenrc(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TTABLE; \
    checkliveness(G(L),i_o); }

#define setptvaluenrc(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TPROTO; \
    checkliveness(G(L),i_o); }

#define setobjnrc(L,obj1,obj2) \
  { const TValue *o2=(obj2); TValue *o1=(obj1); \
    o1->value = o2->value; o1->tt=o2->tt; \
    checkliveness(G(L),o1); }

#else /* !LUA_REFCOUNT */

/* Macros to set values */
#define setnilvalue(obj) ((obj)->tt=LUA_TNIL)

#define setnvalue(obj,x) \
  { TValue *i_o=(obj); i_o->value.n=(x); i_o->tt=LUA_TNUMBER; }

#define setpvalue(obj,x) \
  { TValue *i_o=(obj); i_o->value.p=(x); i_o->tt=LUA_TLIGHTUSERDATA; }

#define setbvalue(obj,x) \
  { TValue *i_o=(obj); i_o->value.b=(x); i_o->tt=LUA_TBOOLEAN; }

#define setsvalue(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TSTRING; \
    checkliveness(G(L),i_o); }

#define setuvalue(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TUSERDATA; \
    checkliveness(G(L),i_o); }

#define setthvalue(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TTHREAD; \
    checkliveness(G(L),i_o); }

#define setclvalue(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TFUNCTION; \
    checkliveness(G(L),i_o); }

#define sethvalue(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TTABLE; \
    checkliveness(G(L),i_o); }

#define setptvalue(L,obj,x) \
  { TValue *i_o=(obj); \
    i_o->value.gc=cast(GCObject *, (x)); i_o->tt=LUA_TPROTO; \
    checkliveness(G(L),i_o); }




#define setobj(L,obj1,obj2) \
  { const TValue *o2=(obj2); TValue *o1=(obj1); \
    o1->value = o2->value; o1->tt=o2->tt; \
    checkliveness(G(L),o1); }

/* to new object */
#define setnilvalue2n	setnilvalue
#define setnvalue2n	setnvalue
#define setpvalue2n	setpvalue
#define setbvalue2n	setbvalue
#define setsvalue2n	setsvalue
#define setuvalue2n	setuvalue
#define setthvalue2n	setthvalue
#define setclvalue2n	setclvalue
#define sethvalue2n	sethvalue
#define setptvalue2n	setptvalue
#define setobj2n	setobj

/* no ref count version */
#define setnilvaluenrc	setnilvalue
#define setnvaluenrc	setnvalue
#define setpvaluenrc	setpvalue
#define setbvaluenrc	setbvalue
#define setsvaluenrc	setsvalue
#define setuvaluenrc	setuvalue
#define setthvaluenrc	setthvalue
#define setclvaluenrc	setclvalue
#define sethvaluenrc	sethvalue
#define setptvaluenrc	setptvalue
#define setobjnrc	setobj
#endif /* LUA_REFCOUNT */

/*
** different types of sets, according to destination
*/

/* from stack to (same) stack */
#define setobjs2s	setobj
/* to stack (not from same stack) */
#define setobj2s	setobj
#define setsvalue2s	setsvalue
#define sethvalue2s	sethvalue
#define setptvalue2s	setptvalue
/* from table to same table */
#define setobjt2t	setobj
/* to table */
#define setobj2t	setobj
/* to new object */
#define setobjs2sn	setobj2n
#define setobj2sn	setobj2n
#define setsvalue2sn	setsvalue2n
#define sethvalue2sn	sethvalue2n
#define setptvalue2sn	setptvalue2n
#define setobjt2tn	setobj2n
#define setobj2tn	setobj2n


#define setttype(obj, tt) (ttype(obj) = (tt))


#define iscollectable(o)	(ttype(o) >= LUA_TSTRING)



typedef TValue *StkId;  /* index to stack elements */


/*
** String headers for string table
*/
typedef union TString {
  L_Umaxalign dummy;  /* ensures maximum alignment for strings */
  struct {
    CommonHeader;
    lu_byte reserved;
    unsigned int hash;
    size_t len;
  } tsv;
} TString;


#define getstr(ts)	cast(const char *, (ts) + 1)
#define svalue(o)       getstr(rawtsvalue(o))



typedef union Udata {
  L_Umaxalign dummy;  /* ensures maximum alignment for `local' udata */
  struct {
    CommonHeader;
    struct Table *metatable;
    struct Table *env;
    size_t len;
  } uv;
} Udata;




/*
** Function Prototypes
*/
typedef struct Proto {
#if LUA_REFCOUNT
  TraversableCommonHeader;
#else
  CommonHeader;
#endif
  TValue *k;  /* constants used by the function */
  Instruction *code;
  struct Proto **p;  /* functions defined inside the function */
  int *lineinfo;  /* map from opcodes to source lines */
  struct LocVar *locvars;  /* information about local variables */
  TString **upvalues;  /* upvalue names */
  TString  *source;
  int sizeupvalues;
  int sizek;  /* size of `k' */
  int sizecode;
  int sizelineinfo;
  int sizep;  /* size of `p' */
  int sizelocvars;
  int linedefined;
  int lastlinedefined;
#if !LUA_REFCOUNT
  GCObject *gclist;
#endif
  lu_byte nups;  /* number of upvalues */
  lu_byte numparams;
  lu_byte is_vararg;
  lu_byte maxstacksize;
} Proto;


/* masks for new-style vararg */
#define VARARG_HASARG		1
#define VARARG_ISVARARG		2
#define VARARG_NEEDSARG		4


typedef struct LocVar {
  TString *varname;
  int startpc;  /* first point where variable is active */
  int endpc;    /* first point where variable is dead */
} LocVar;



/*
** Upvalues
*/

typedef struct UpVal {
  CommonHeader;
  TValue *v;  /* points to stack or to its own value */
  union {
    TValue value;  /* the value (when closed) */
    struct {  /* double linked list (when open) */
      struct UpVal *prev;
      struct UpVal *next;
    } l;
  } u;
} UpVal;


/*
** Closures
*/

#if LUA_REFCOUNT
#define ClosureHeader \
	TraversableCommonHeader; lu_byte isC; lu_byte nupvalues; \
	struct Table *env

#else /* !LUA_REFCOUNT */
#define ClosureHeader \
	CommonHeader; lu_byte isC; lu_byte nupvalues; GCObject *gclist; \
	struct Table *env
#endif

typedef struct CClosure {
  ClosureHeader;
  lua_CFunction f;
  TValue upvalue[1];
} CClosure;


typedef struct LClosure {
  ClosureHeader;
  struct Proto *p;
  UpVal *upvals[1];
} LClosure;


typedef union Closure {
  CClosure c;
  LClosure l;
} Closure;


#define iscfunction(o)	(ttype(o) == LUA_TFUNCTION && clvalue(o)->c.isC)
#define isLfunction(o)	(ttype(o) == LUA_TFUNCTION && !clvalue(o)->c.isC)


/*
** Tables
*/

typedef union TKey {
  struct {
    TValuefields;
    struct Node *next;  /* for chaining */
  } nk;
  TValue tvk;
} TKey;


typedef struct Node {
  TValue i_val;
  TKey i_key;
} Node;


typedef struct Table {
#if LUA_REFCOUNT
  TraversableCommonHeader;
#else
  CommonHeader;
#endif
  lu_byte flags;  /* 1<<p means tagmethod(p) is not present */ 
  lu_byte lsizenode;  /* log2 of size of `node' array */
  struct Table *metatable;
  TValue *array;  /* array part */
  Node *node;
  Node *lastfree;  /* any free position is before this position */
#if !LUA_REFCOUNT
  GCObject *gclist;
#endif
  int sizearray;  /* size of `array' array */
#if LUA_PROFILE
  lua_Integer resizecount;
#endif
} Table;



/*
** `module' operation for hashing (size is always a power of 2)
*/
#define lmod(s,size) \
	(check_exp((size&(size-1))==0, (cast(int, (s) & ((size)-1)))))


#define twoto(x)	((l_mem)1<<(x))
#define sizenode(t)	(twoto((t)->lsizenode))


#define luaO_nilobject		(&luaO_nilobject_)

LUAI_DATA const TValue luaO_nilobject_;

#define ceillog2(x)	(luaO_log2((x)-1) + 1)

LUAI_FUNC int luaO_log2 (unsigned int x);
LUAI_FUNC int luaO_int2fb (unsigned int x);
LUAI_FUNC int luaO_fb2int (int x);
LUAI_FUNC int luaO_rawequalObj (const TValue *t1, const TValue *t2);
LUAI_FUNC int luaO_str2d (const char *s, lua_Number *result);
LUAI_FUNC const char *luaO_pushvfstring (lua_State *L, const char *fmt,
                                                       va_list argp);
LUAI_FUNC const char *luaO_pushfstring (lua_State *L, const char *fmt, ...);
LUAI_FUNC void luaO_chunkid (char *out, const char *source, size_t len);


#endif

