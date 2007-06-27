#ifndef __LIBADB_H__
#define __LIBADB_H__

#include "chord_types.h"
#include "adb_prot.h"

#include <qhash.h>

class aclnt;
class location;
class chord_trigger_t;
struct block_info;

typedef callback<void, adb_status, chordID, str>::ptr cb_fetch;
typedef callback<void, adb_status>::ptr cb_adbstat;
typedef callback<void, adb_status, u_int32_t, vec<adb_keyaux_t> >::ptr cb_getkeys;
typedef callback<void, adb_status, vec<chordID>, vec<u_int32_t> >::ptr cb_getkeyson;

typedef callback<void, clnt_stat, adb_status, vec<block_info> >::ref cbvblock_info_t;
typedef callback<void, clnt_stat, adb_status, block_info>::ref cbblock_info_t;

typedef callback<void, adb_status, str, bool>::ptr cb_getspace_t;

struct block_info {
  chordID k;
  vec<chord_node> on;
  vec<u_int32_t> aux;
  block_info () {};
  block_info (chordID k) : k (k) {};
  block_info (const block_info &b) : k (b.k), on (b.on), aux (b.aux) {};

  block_info& operator= (const block_info &b) {
    if( this != &b ) {
      k = b.k;
      on = b.on;
      aux = b.aux;
    }
    return *this;
  }
};

class adb {
  ptr<aclnt> c;
  str dbsock_;
  str name_space;
  bool hasaux_;

  qhash<u_int32_t, chordID> getkeystab;

  enum {
    UPDATE_BATCH_SECS = 1,
    UPDATE_BATCH_MAX_SIZE = 128
  };

  bool connecting;
  void connect (ptr<chord_trigger_t> t = NULL);
  void handle_eof ();

  void initspace_cb (ptr<chord_trigger_t> t, adb_status *astat, clnt_stat stat);
  void generic_cb (adb_status *res, cb_adbstat cb, clnt_stat err);
  void fetch_cb (adb_fetchres *res, chordID key, cb_fetch cb, clnt_stat err);
  void getkeys_cb (bool getaux, adb_getkeysres *res, cb_getkeys cb, clnt_stat err);
  void getspaceinfocb (ptr<adb_getspaceinfores> res, cb_getspace_t cb, clnt_stat err);

  void batch_update ();

public:
  adb (str sock_name, str name = "default", bool hasaux = false,
      ptr<chord_trigger_t> t = NULL);

  str name () const { return name_space; }
  str dbsock () const { return dbsock_; } 
  bool hasaux () const { return hasaux_; }

  void store (chordID key, str data, u_int32_t auxdata, cb_adbstat cb);
  void store (chordID key, str data, cb_adbstat cb);
  void fetch (chordID key, cb_fetch cb);
  void fetch (chordID key, bool nextkey, cb_fetch cb);
  void remove (chordID key, cb_adbstat cb);
  void remove (chordID key, u_int32_t auxdata, cb_adbstat cb);
  void getkeys (u_int32_t id, cb_getkeys cb, bool ordered = false, u_int32_t batchsize = 16384, bool getaux = false);
  void sync (cb_adbstat cb);

  void getspaceinfo (cb_getspace_t cb);
};

#endif
