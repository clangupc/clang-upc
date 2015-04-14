/*===--- gupcr_fabric.c - UPC Runtime Support Library --------------------===
|*
|*                     The LLVM Compiler Infrastructure
|*
|* Copyright 2012-2014, Intel Corporation.  All rights reserved.
|* This file is distributed under a BSD-style Open Source License.
|* See LICENSE-INTEL.TXT for details.
|*
|*===---------------------------------------------------------------------===*/

/**
 * @file gupcr_fabric.c
 * GUPC Llibfabric Initialization.
 */

#include "gupcr_config.h"
#include "gupcr_defs.h"
#include "gupcr_utils.h"
#include "gupcr_fabric.h"
#include "gupcr_runtime.h"

#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

/**
 * @addtogroup FABRIC_RUNTIME GUPCR Libfabric runtime interface
 * @{
 */

int gupcr_rank;
int gupcr_rank_cnt;
int gupcr_child[GUPCR_TREE_FANOUT];
int gupcr_child_cnt;
int gupcr_parent_thread;
struct fid_ep *gupcr_ep = NULL;

size_t gupcr_max_order_size;
size_t gupcr_max_msg_size;
size_t gupcr_max_optim_size;

/** Fabric */
static fab_t gupcr_fab;
/** Fabric info */
fab_info_t gupcr_fi;
/** Fabric domain */
fab_domain_t gupcr_fd;
/** Endpoint comm resource  */
fab_ep_t gupcr_ep;
/** Address vector for remote endpoints  */
fab_av_t gupcr_av;
/** All endpoint names in the system */
static char epname[128];
static char *epnames;


// TODO: Configure network interface name and libfabric provider.
//       Hack for getting network address.  We can get this from the
//       fi_getinfo (?) but current implementation does not fill out src_addr
/** Infiniband interface.  */
const char *ifname = "ib0";
/** Libfabric provider */
// Use 'socket' provider for now, as this needs to be configurable. 
char *prov_name = (char *) "sockets";
/** Interface IPv4 address */
in_addr_t net_addr;
/** IPv4 addresses for all ranks */
in_addr_t *net_addr_map;

/**
 * Get IPv4 address of the specified interface
 */
in_addr_t
check_ip_address (const char *ifname)
{
  int fd;
  struct ifreq devinfo;
  struct sockaddr_in *sin = (struct sockaddr_in *) &devinfo.ifr_addr;
  in_addr_t addr;

  fd = socket (AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (fd < 0)
    {
      gupcr_fatal_error ("error initializing socket");
      gupcr_abort ();
    }

  strncpy (devinfo.ifr_name, ifname, IFNAMSIZ);

  if (ioctl (fd, SIOCGIFADDR, &devinfo) == 0)
    {
      addr = sin->sin_addr.s_addr;
      gupcr_log (FC_FABRIC, "IB IPv4 address for %s is %d",
		 devinfo.ifr_name, addr);
    }
  else
    addr = INADDR_ANY;
  close (fd);
  return addr;
}

/**
 * Return Libfabric error description string.
 *
 * @param [in] errnum Libfabric error number
 * @retval Error description string
 */
const char *
gupcr_strfaberror (int errnum)
{
  static char gupcr_strfaberror_buf[64];
  switch (-errnum)
    {
    case FI_SUCCESS:
      return "Sucess";
      break;
    case FI_ENOENT:
      return "No such file or directory";
      break;
    case FI_EIO:
      return "IO error";
      break;
    case FI_E2BIG:
      return "Argument list too long";
      break;
    case FI_EBADF:
      return "Bad file number";
      break;
    case FI_EAGAIN:
      return "Try again";
      break;
    case FI_ENOMEM:
      return "Out of memory";
      break;
    case FI_EACCES:
      return "Permission denied";
      break;
    case FI_EBUSY:
      return "Device or resource busy";
      break;
    case FI_ENODEV:
      return "No such device";
      break;
    case FI_EINVAL:
      return "Invalid argument";
      break;
    case FI_EMFILE:
      return "Too many open files";
      break;
    case FI_ENOSPC:
      return "No space left on device";
      break;
    case FI_ENOSYS:
      return "Function not implemented";
      break;
    case FI_ENOMSG:
      return "No message of desired type";
      break;
    case FI_ENODATA:
      return "No data available";
      break;
    case FI_EMSGSIZE:
      return "Message too long";
      break;
    case FI_ENOPROTOOPT:
      return "Protocol not available";
      break;
    case FI_EOPNOTSUPP:
      return "Operation not supported on transport endpoint";
      break;
    case FI_EADDRINUSE:
      return "Address already in use";
      break;
    case FI_EADDRNOTAVAIL:
      return "Cannot assign requested address";
      break;
    case FI_ENETDOWN:
      return "Network is down";
      break;
    case FI_ENETUNREACH:
      return "Network is unreachable";
      break;
    case FI_ECONNABORTED:
      return "DSoftware caused connection abort";
      break;
    case FI_ECONNRESET:
      return "Connection reset by peer";
      break;
    case FI_EISCONN:
      return "Transport endpoint is already connected";
      break;
    case FI_ENOTCONN:
      return "Transport endpoint is not connected";
      break;
    case FI_ESHUTDOWN:
      return "Cannot send after transport endpoint shutdown";
      break;
    case FI_ETIMEDOUT:
      return "Connection timed out";
      break;
    case FI_ECONNREFUSED:
      return "Connection refused";
      break;
    case FI_EHOSTUNREACH:
      return "No route to host";
      break;
    case FI_EALREADY:
      return "Operation already in progress";
      break;
    case FI_EINPROGRESS:
      return "Operation now in progress";
      break;
    case FI_EREMOTEIO:
      return "Remote IO error/";
      break;
    case FI_ECANCELED:
      return "Operation Canceled";
      break;
    case FI_EKEYREJECTED:
      return "Key was rejected by service";
      break;
    case FI_EOTHER:
      return "Unspecified error";
      break;
    case FI_ETOOSMALL:
      return "Provided buffer is too small";
      break;
    case FI_EOPBADSTATE:
      return "Operation not permitted in current state";
      break;
    case FI_EAVAIL:
      return "Error available";
      break;
    case FI_EBADFLAGS	:
      return "Flags not supported";
      break;
    case FI_ENOEQ:
      return "Missing or unavailable event queue";
      break;
    case FI_EDOMAIN:
      return "Invalid resource domain";
      break;
    case FI_ENOCQ:
      return "Missing or unavailable completion queue";
      break;
    case FI_ECRC:
      return "CRC error";
      break;
    case FI_ETRUNC:
      return "Truncation error";
      break;
    case FI_ENOKEY:
      return "Required key not available";
      break;
    
    default:
      break;
    }
  sprintf (gupcr_strfaberror_buf, "unknown fabric status code: %d", errnum);
  return gupcr_strfaberror_buf;
}

/**
 * Return Event Queue type description string.
 *
 * @param [in] eqtype Event queue type
 * @retval Event queue type description string
 */
const char *
gupcr_streqtype (uint64_t eqtype)
{
  switch (eqtype)
    {
    default:
      break;
    }
  return "UNKNOWN EVENT TYPE";
}

/**
 * Return Data type description string.
 
 * @param [in] datatype Data type
 * @retval Data type description string
 */
const char *
gupcr_strdatatype (enum fi_datatype datatype)
{
  switch (datatype)
    {
    case FI_INT8:
      return "FI_INT8";
    case FI_UINT8:
      return "FI_UINT8";
    case FI_INT16:
      return "FI_INT16";
    case FI_UINT16:
      return "FI_UINT16";
    case FI_INT32:
      return "FI_INT32";
    case FI_UINT32:
      return "FI_UINT32";
    case FI_FLOAT:
      return "FI_FLOAT";
    case FI_INT64:
      return "FI_INT64";
    case FI_UINT64:
      return "FI_UINT64";
    case FI_DOUBLE:
      return "FI_DOUBLE";
    case FI_FLOAT_COMPLEX:
      return "FI_FLOAT_COMPLEX";
    case FI_DOUBLE_COMPLEX:
      return "FI_DOUBLE_COMPLEX";
    case FI_LONG_DOUBLE:
      return "FI_LONG_DOUBLE";
    case FI_LONG_DOUBLE_COMPLEX:
      return "FI_LONG_DOUBLE_COMPLEX";
    default:
      return "UNKNOWN DATA TYPE";
    }
}

/**
 * Return Atomic operation description string.
 *
 * @param [in] op Atomic operation type
 * @retval Atomic operation description string
 */
const char *
gupcr_strop (enum fi_op op)
{
  switch (op)
    {
    case FI_MIN:
      return "FI_MIN";
    case FI_MAX:
      return "FI_MAX";
    case FI_SUM:
      return "FI_SUM";
    case FI_PROD:
      return "FI_PROD";
    case FI_LOR:
      return "FI_LOR";
    case FI_LAND:
      return "FI_LAND";
    case FI_BOR:
      return "FI_BOR";
    case FI_BAND:
      return "FI_BAND";
    case FI_LXOR:
      return "FI_LXOR";
    case FI_BXOR:
      return "FI_BXOR";
    case FI_ATOMIC_READ:
      return "FI_ATOMIC_READ";
    case FI_ATOMIC_WRITE:
      return "FI_ATOMIC_WRITE";
    case FI_CSWAP:
      return "FI_CSWAP";
    case FI_CSWAP_NE:
      return "FI_CSWAP_NE";
    case FI_CSWAP_LE:
      return "FI_CSWAP_LE";
    case FI_CSWAP_LT:
      return "FI_CSWAP_LT";
    case FI_CSWAP_GE:
      return "FI_CSWAP_GE";
    case FI_CSWAP_GT:
      return "FI_CSWAP_GT";
    case FI_MSWAP:
      return "FI_MSWAP";
    default:
      return "UNKNOWN ATOMIC OPERATION TYPE";
    }
}

/**
 * Return Network Interface error description string.
 *
 * @param [in] nitype NI failure type
 * @retval NI failure description string.
 */
const char *
gupcr_nifailtype (int nitype)
{
  switch (nitype)
    {
    default:
      ;
    }
  return "NI_FAILURE_UNKNOWN";
}

/**
 * Return atomic data type from the specified size.
 */
enum fi_datatype
gupcr_get_atomic_datatype (int size)
{
  switch (size)
    {
    case 1:
      return FI_UINT8;
    case 2:
      return FI_UINT16;
    case 4:
      return FI_UINT32;
    case 8:
      return FI_UINT64;
    case 16:
      return FI_DOUBLE_COMPLEX;
    default:
      gupcr_fatal_error
	("Unable to convert size of %d into atomic data type.", size);
    }
  return -1;
}

/**
 * Return data size from the specified atomic type.
 *
 * @param [in] type atomic data type
 * @retval atomic data type size
 */
size_t
gupcr_get_atomic_size (enum fi_datatype type)
{
  switch (type)
    {
    case FI_INT8:
    case FI_UINT8:
      return 1;
    case FI_INT16:
    case FI_UINT16:
      return 2;
    case FI_INT32:
    case FI_UINT32:
      return 4;
    case FI_INT64:
    case FI_UINT64:
      return 8;
    case FI_FLOAT:
      return __SIZEOF_FLOAT__;
    case FI_FLOAT_COMPLEX:
      return 2 * __SIZEOF_FLOAT__;
    case FI_DOUBLE:
      return __SIZEOF_DOUBLE__;
    case FI_DOUBLE_COMPLEX:
      return 2 * __SIZEOF_DOUBLE__;
#ifdef __SIZEOF_LONG_DOUBLE__
    case FI_LONG_DOUBLE:
      return __SIZEOF_LONG_DOUBLE__;
    case FI_LONG_DOUBLE_COMPLEX:
      return 2 * __SIZEOF_LONG_DOUBLE__;
#endif
    default:
      gupcr_fatal_error ("unknown atomic type %d", (int) type);
    }
  return -1;
}

/**
 * @fn gupcr_process_fail_events (struct fid_eq eq)
 * Show information on failed events.
 *
 * This procedure prints the contents of the event queue.  As
 * completion events are not reported, the event queue contains
 * only failure events.  This procedure is called only if any of the
 * counting events reported a failure.
 *
 * @param [in] eq Completion Queue ID
 */
void
gupcr_process_fail_events (fab_cq_t cq)
{
  int ret;
  struct fi_cq_msg_entry cq_entry;
  gupcr_fabric_call_nc (fi_cq_read, ret, (cq, (void *) &cq_entry,
					  sizeof (cq_entry)));
  if (ret < 0)
    {
      char buf[256];
      const char *errstr;
      struct fi_cq_err_entry cq_error;
      gupcr_fabric_call_nc (fi_cq_readerr, ret,
			    (cq, (void *) &cq_error, 0));
      gupcr_fabric_call_nc (fi_cq_strerror, errstr,
			    (cq, cq_error.err, cq_error.err_data,
			     buf, sizeof (buf)));
      gupcr_error ("%s", buf);
    }
  else
    gupcr_fatal_error ("ctr reported an error, but cq has none");
}

/**
 * Get current thread rank.
 * @retval Rank of the current thread
 */
int
gupcr_get_rank (void)
{
  return gupcr_rank;
}

/**
 * Get number of running threads.
 * @retval Number of running threads
 */
int
gupcr_get_threads_count (void)
{
  return gupcr_rank_cnt;
}

/**
 * Get process PID for specified rank.
 * @param [in] rank Rank of the thread
 * @retval PID of the thread
 */
int
gupcr_get_rank_pid (int rank)
{
  return rank;
}

/**
 * Get process NID for specified rank.
 * @param [in] rank Rank of the thread
 * @retval NID of the thread
 */
int
gupcr_get_rank_nid (int rank)
{
  return (int) net_addr_map[rank];
}

/**
 * Wait for all threads to complete initialization.
 */
void
gupcr_startup_barrier (void)
{
  gupcr_runtime_barrier ();
}

/** @} */

/**
 * @addtogroup INIT GUPCR Initialization
 * @{
 */

/**
 * Initialize Libfabric.
 */
void
gupcr_fabric_init (void)
{
  struct fi_info hints = { 0 };
  struct fi_fabric_attr fi_attr = { 0 };
  av_attr_t av_attr = { 0 };
  ep_attr_t ep_attr = { 0 };
  size_t epnamelen = sizeof (epname);
  char node[16];

  /* Find the network ID (IPv4 address).  */
  net_addr = check_ip_address (ifname);
  if (net_addr == INADDR_ANY)
    {
      gupcr_fatal_error ("IPv4 is not available on interface %s", ifname);
      gupcr_abort ();
    }
  if (!inet_ntop(AF_INET, &net_addr, node, sizeof(node)))
    {
      gupcr_fatal_error (
	"Error while converting IPv4 (%x) address into a string", net_addr);
      gupcr_abort ();
    }

  /* Find fabric provider based on the hints.  */
  hints.caps = FI_RMA |		/* Request RMA capability,  */
    FI_ATOMICS | 		/* atomics capability,  */
    FI_DYNAMIC_MR;		/* MR without physical backing,  */
  ep_attr.type = FI_EP_RDM;	/* Reliable datagram message.  */
  hints.addr_format = FI_FORMAT_UNSPEC;
  ep_attr.rx_ctx_cnt = GUPCR_SERVICE_COUNT;
  ep_attr.tx_ctx_cnt = GUPCR_SERVICE_COUNT;
  hints.ep_attr = &ep_attr;

  /* Choose provider.  */
  hints.fabric_attr = &fi_attr;
  hints.fabric_attr->prov_name = prov_name;

#define __GUPCR_STR__(S) #S
#define __GUPCR_XSTR__(S) __GUPCR_STR__(S)
  /*  Hard-coded service name for Portals PTE.  */
  gupcr_fabric_call (fi_getinfo,
		     (FI_VERSION (1, 0), node, NULL,
		      FI_SOURCE, &hints, &gupcr_fi));
  gupcr_fabric_call (fi_fabric, (gupcr_fi->fabric_attr, &gupcr_fab, NULL));
  gupcr_fabric_call (fi_domain, (gupcr_fab, gupcr_fi, &gupcr_fd, NULL));

  gupcr_rank = gupcr_runtime_get_rank ();
  gupcr_rank_cnt = gupcr_runtime_get_size ();

  /* Set endpoint features.  */
  gupcr_max_msg_size = gupcr_fi->ep_attr->max_msg_size;
  gupcr_max_order_size = gupcr_fi->ep_attr->max_order_raw_size;
  gupcr_max_optim_size = gupcr_fi->tx_attr->inject_size;

  /* Create an endpoint for all UPC contexts.  */
  gupcr_fabric_call (fi_endpoint, (gupcr_fd, gupcr_fi, &gupcr_ep, NULL));

  /* Other threads' endpoints are mapped via address vector table with
     each threads' endpoint indexed by the thread number.  */
  av_attr.type = FI_AV_TABLE;
  av_attr.count = gupcr_rank_cnt;
  av_attr.name = "ENDPOINTS";
  av_attr.rx_ctx_bits = GUPCR_SERVICE_BITS;
  gupcr_fabric_call (fi_av_open, (gupcr_fd, &av_attr, &gupcr_av, NULL));
  gupcr_fabric_call (fi_ep_bind, (gupcr_ep, &gupcr_av->fid, 0));

  /* Enable endpoint.  */
  gupcr_fabric_call (fi_enable, (gupcr_ep));

  /* Exchange endpoint name with other threads.  */
  gupcr_fabric_call (fi_getname, ((fid_t) gupcr_ep, epname, &epnamelen));
  if (epnamelen > sizeof (epname))
    gupcr_fatal_error ("sizeof endpoint name greater then %lu",
		       sizeof (epname));

  epnames = calloc (gupcr_rank_cnt * epnamelen, 1);
  if (!epnames)
    gupcr_fatal_error ("cannot allocate %ld for epnames",
		       gupcr_rank_cnt * epnamelen);
  {
    int ret = gupcr_runtime_exchange ("gmem", epname, epnamelen, epnames);
    if (ret)
      {
	gupcr_fatal_error
	  ("error (%d) reported while exchanging GMEM endpoints", ret);
      }
    gupcr_log
      (FC_FABRIC, "exchanged ep names with %d threads", gupcr_rank_cnt);
  }
  /* Map endpoints.  */
  {
    int insert_cnt;
    gupcr_fabric_call_nc
      (fi_av_insert, insert_cnt,
       (gupcr_av, epnames, gupcr_rank_cnt, NULL, 0, NULL));
    if (insert_cnt != gupcr_rank_cnt)
      gupcr_fatal_error ("cannot av insert %d entries (reported %d)",
		       gupcr_rank_cnt, insert_cnt);
  }
}

/**
 * Close Libfabric.
 */
void
gupcr_fabric_fini (void)
{
  gupcr_fabric_call (fi_close, (&gupcr_av->fid));
  gupcr_fabric_call (fi_close, (&gupcr_ep->fid));
  free (epnames);
  gupcr_fabric_call (fi_close, (&gupcr_fab->fid));
}

/**
 * Initialize Networking Interface.
 */
void
gupcr_fabric_ni_init (void)
{
  int status;
  /* Allocate space for network addresses.  */
  net_addr_map = calloc (THREADS * sizeof (in_addr_t), 1);
  if (!net_addr_map)
    gupcr_fatal_error ("cannot allocate memory for net node map");
  /* Exchange network id with other nodes.  */
  status = gupcr_runtime_exchange ("NI", &net_addr, sizeof (in_addr_t),
				   net_addr_map);

}

/**
 * Close Fabric Networking Interface.
 */
void
gupcr_fabric_ni_fini (void)
{
}

/**
 * Find the node's parent and all its children.
 */
void
gupcr_nodetree_setup (void)
{
  int i;
  gupcr_log ((FC_BARRIER | FC_BROADCAST),
	     "node tree initialized with fanout of %d", GUPCR_TREE_FANOUT);
  for (i = 0; i < GUPCR_TREE_FANOUT; i++)
    {
      int child = GUPCR_TREE_FANOUT * MYTHREAD + i + 1;
      if (child < THREADS)
	{
	  gupcr_child_cnt++;
	  gupcr_child[i] = child;
	}
    }
  if (MYTHREAD == 0)
    gupcr_parent_thread = ROOT_PARENT;
  else
    gupcr_parent_thread = (MYTHREAD - 1) / GUPCR_TREE_FANOUT;
}

/** @} */
