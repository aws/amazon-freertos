/*
*  (C) Copyright 2008-2018 Marvell International Ltd. All Rights Reserved
*
*  MARVELL CONFIDENTIAL
*  The source code contained or described herein and all documents related to
*  the source code ("Material") are owned by Marvell International Ltd or its
*  suppliers or licensors. Title to the Material remains with Marvell
*  International Ltd or its suppliers and licensors. The Material contains
*  trade secrets and proprietary and confidential information of Marvell or its
*  suppliers and licensors. The Material is protected by worldwide copyright
*  and trade secret laws and treaty provisions. No part of the Material may be
*  used, copied, reproduced, modified, published, uploaded, posted,
*  transmitted, distributed, or disclosed in any way without Marvell's prior
*  express written permission.
*
*  No license under any patent, copyright, trade secret or other intellectual
*  property right is granted to or conferred upon you by disclosure or delivery
*  of the Materials, either expressly, by implication, inducement, estoppel or
*  otherwise. Any license under such intellectual property rights must be
*  express and approved by Marvell in writing.
*
*/

/*! \file mdns.h
 * \brief The mDNS/DNS-SD Responder-Querier
 *
 * mDNS (Multicast DNS) and DNS-SD (DNS based Service Discovery) together
 * enable Service Discovery in Zero configuration networks by using the
 * standard DNS protocol packets but with minor changes.
 *
 *  \section  mdns_usage Example Usage:
 *
 *  \subsection responder Working with mDNS Responder (Announcing the service)
 *
 *  The following code announces the service named
 *  wm_demo._smartenergy._tcp.local
 *  with the given TXT records' key-value pairs.
 *
 * \code
 * #include <mdns.h>
 * #include <wm_net.h>
 *
 * struct mdns_service myservice;
 * char service_name[]="wm_demo";
 * char serv_type[]="smartenergy";
 * char keyvals[]="txtvers=2|path=/wm_demo|description=Wireless" \
 *				"Microcontroller|Product=WM1";
 * char domain[]="local";
 * char dname[]="myhost";
 * int wm_mdns_start()
 * {
 *        int ret = 0;
 *        void *handle;
 *        memset(&myservice,0,sizeof(myservice));
 *        myservice.proto = MDNS_PROTO_TCP;
 *        myservice.servname = service_name;
 *        myservice.servtype = serv_type;
 *        myservice.keyvals = keyvals;
 *        myservice.port = 8888;
 *        myservice.domain = domain;
 *        handle = net_get_sta_handle();
 *        mdns_start(domain, dname);
 *        mdns_set_txt_rec(&myservice, keyvals, '|');
 *        ret = mdns_announce_service(&myservice, handle);
 *        return ret;
 * }
 *
 * int wm_mdns_stop()
 * {
 *        mdns_stop();
 *        return 0;
 * }
 * \endcode
 *
 * In order to announce a set of services at once, use
 * mdns_announce_service_arr().
 *
 * <h2>Service Discovery with Linux</h2>
 *
 * The services published in the way described above can be discovered from a
 * Linux machine connected in the same network using avahi-tools.
 * For example, if all service instances belonging to a particular type of
 * service in the network need to be discovered.
 *
 * Following command looks for service instances of the type "_smartenergy._tcp"
 * in the network. The second line shows the output for the command, which
 * discovered the service instance published with the given service type.
 * \code
 * avahi-browse _smartenergy._tcp
 * + wlan0 IPv4 wm_demo                               _smartenergy._tcp    local
 * \endcode
 *
 *  \subsection querier Working with mDNS Querier (Querying the service)
 *
 *  The following code monitors for the service of type
 *  "_smartenergy._tcp.local."
 *  Any state change to the monitored service is reported through a callback 
 *  function wm_demo_mdns_query_cb.
 *
 * \code
 * #include <mdns.h>
 * #include <wm_net.h>
 *
 * char fqst[]="_smartenergy._tcp.local";
 *
 * static int wm_demo_mdns_query_cb(void *data, const struct mdns_service *s,
 *				int status)
 * {
 *        wmprintf("Got callback for %s\r\n", s->servname);
 *
 *        if (status == MDNS_DISCOVERED)
 *        	wmprintf("DISCOVERED:\r\n");
 *        else if (status == MDNS_CACHE_FULL)
 *              wmprintf("NOT_CACHED:\r\n");
 *        else if (status == MDNS_DISAPPEARED)
 *             	wmprintf("DISAPPEARED:\r\n");
 *        else if (status == MDNS_UPDATED)
 *             	wmprintf("UPDATED:\r\n");
 *        else {
 *              wmprintf("Warning: unknown status %d\r\n", status);
 *              return WM_SUCCESS;
 *        }
 *        wmprintf("%s._%s._%s.%s.\r\n",s->servname, s->servtype,
 *		s->proto == MDNS_PROTO_UDP ? "udp" : "tcp",
 *		s->domain);
 *        wmprintf("at %d.%d.%d.%d:%d\r\n",
 *		(s->ipaddr >> 24) & 0xff, (s->ipaddr >> 16) & 0xff,
 *		(s->ipaddr >> 8) & 0xff, s->ipaddr & 0xff,
 *		htons(s->port));
 *        wmprintf("TXT : %s\r\n", s->keyvals ? s->keyvals : "no key vals");
 *
 *        return WM_SUCCESS;
 * }
 *
 * int wm_mdns_start()
 * {
 *	void *iface = net_get_sta_handle();
 *	mdns_start(NULL, NULL);
 *	mdns_query_monitor(fqst, wm_demo_mdns_query_cb, NULL, iface);
 * }
 *
 * int wm_mdns_stop()
 * {
 *        mdns_stop();
 *        return 0;
 * }
 * \endcode
 *
 *  \subsubsection mdns_subtype_support  mDNS subtype base querier support
 *
 * WMSDK mDNS querier also supports subtype base discovery. To browse for
 * a subtype base query, "fqst" should be in following format:
 * \<subtype_name\>._sub._\<user_protocol\>._\<tcp or udp\>.\<domain\>
 *
 * In order to search for "_marvell" subtype based services chanage value of
 * fqst in the querier code above to "_marvell._sub._http._tcp.local";
 *
 * <h2>Publishing Service using Linux</h2>
 *
 * Following command publishes service using avahi publish, which could be
 * queried using above code snippet.
 *
 * avahi-publish -s "MarvellService"  _smartenergy._tcp 80
 *
 * Following command publishes "subtype" based service using avahi publish,
 * which could be queried by making changes in querier code snippet as described
 * in \ref mdns_subtype_support section.
 *
 * avahi-publish -s --domain="local" --subtype="_marvell._sub._http._tcp"
 *                                            "MarvellService" "_http._tcp" 80
 *
 * \subsection build_options Build Options
 * A number of build options are available in order to optimize footprint,
 * specify the target system, etc. These are conventionally provided as config
 * options as opposed to being defined directly in a header file. They
 * include:
 *
 * <b>CONFIG_MDNS_QUERY</b>: Developers who require the ability to query for
 * services as opposed to just responding to such queries should define
 * MDNS_QUERY. This enables the mdns_query_* functions described below.  These
 * functions will return -WM_E_MDNS_NOIMPL if the mdns library was built without
 * MDNS_QUERY defined. By default this option is disabled.
 *
 * <b>CONFIG_MDNS_CHECK_ARGS</b>: Define this option to enable various error
 * checking on the input. Developers may wish to enable this during development
 * to ensure that various inputs such as host names are legal.  Then, if the
 * inputs are not going to change, this option can be turned off to save code
 * space. By default this option is disabled.
 *
 * <b>CONFIG_XMDNS</b>: Define this option to select Extended-mDNS operations.
 * Extended mDNS extends specification of mDNS to site-local scope in order to
 * support multi-hop LANs that forward multicast packets but do not provide
 * a unicast DNS service. Note that selecting Extended-mDNS requires top-level
 * domain name ".site", instead of mDNS' ".local" domain name. Both domains
 * are functionally disjoint, both from a name space and address space
 * perspective. Application could register to only one domain name i.e enabling
 * ex-mDNS would mean .local will not work for querier as well as responder.
 *
 * CONFIG_MDNS_DEBUG: Define this option to include debugging and logging.
 * By default this option is disabled.
 *
*/

#ifndef MDNS_H
#define MDNS_H

#include <mdns_port.h>
#include <wmerrno.h>
//#include <wm_net.h>
#include <compat_attribute.h>

/* mdns control socket ports
 *
 * mdns uses two control sockets to communicate between the mdns threads and
 * any API calls.  This control socket is actually a UDP socket on the loopback
 * interface.  Developers who wish to specify certain ports for this control
 * socket can do so by changing MDNS_CTRL_RESPONDER and MDNS_CTRL_QUERIER.
 */
#ifndef MDNS_CTRL_RESPONDER
#define MDNS_CTRL_RESPONDER 12345
#endif

#ifndef MDNS_CTRL_QUERIER
#define MDNS_CTRL_QUERIER  (MDNS_CTRL_RESPONDER + 1)
#endif

/** Maximum length of labels
 *
 * A label is one segment of a DNS name.  For example, "foo" is a label in the
 * name "foo.local.".  RFC 1035 requires that labels do not exceed 63 bytes.
 */
#define MDNS_MAX_LABEL_LEN	63	/* defined by the standard */

/** Maximum length of names
 *
 * A name is a list of labels such as "My Webserver.foo.local" or
 * mydevice.local.  RFC 1035 requires that names do not exceed 255 bytes.
 */
#define MDNS_MAX_NAME_LEN	255	/* defined by the standard : 255*/

/** Maximum length of key/value pair
 *
 * TXT records associated with a service are populated with key/value pairs.
 * These key/value pairs must not exceed this length.
 */
#define MDNS_MAX_KEYVAL_LEN	255	/* defined by the standard : 255*/

/** mDNS Interface State
 * mDNS interface state can be changed by using mdns_iface_state_change()
 * function. For details about when to use the enum please refer to
 * documentation for mdns_iface_state_change(). */
enum iface_state {
	/** UP the interface and announce services
	 * mDNS will probe and announce all services announced via
	 * mdns_announce_service() and/or mdns_announce_service_arr().
	 * mDNS will go through entire probing sequence explained in above
	 * functions. Interface state can be changed to UP, if its DOWN.
	 */
	UP = 0,
	/** DOWN the interface and de-announce services
	 * mDNS sends good bye packet with ttl=0 so that mDNS clients can remove
	 * the services from their mDNS cache table.
	 */
	DOWN,
	/** Forcefully re-announce services
	 * This state should be used after services are already
	 * announced and force announcement is needed due to some reason.
	 * mDNS will not perform probing sequence, as it does in case of UP, and
	 * will directly re-announce services.
	 */
	REANNOUNCE
};

/** MDNS Error Codes */
enum wm_mdns_errno {
	WM_E_MDNS_ERRNO_START = MOD_ERROR_START(MOD_MDNS),
	/** invalid argument*/
	WM_E_MDNS_INVAL,
	/** bad service descriptor*/
	WM_E_MDNS_BADSRC,
	/** not enough room for everything*/
	WM_E_MDNS_TOOBIG,
	/** unimplemented feature*/
	WM_E_MDNS_NOIMPL,
	/** insufficient memory*/
	WM_E_MDNS_NOMEM,
	/** requested resource is in use*/
	WM_E_MDNS_INUSE,
	/** requested resource is in use*/
	WM_E_MDNS_NORESP,
	/** failed to create socket for mdns*/
	WM_E_MDNS_FSOC,
	/** failed to reuse multicast socket*/
	WM_E_MDNS_FREUSE,
	/** failed to bind mdns socket to device*/
	WM_E_MDNS_FBINDTODEVICE,
	/** failed to bind mdns socket*/
	WM_E_MDNS_FBIND,
	/** failed to join multicast socket*/
	WM_E_MDNS_FMCAST_JOIN,
	/** failed to set multicast socket*/
	WM_E_MDNS_FMCAST_SET,
	/** failed to create query socket*/
	WM_E_MDNS_FQUERY_SOC,
	/** failed to create mdns thread*/
	WM_E_MDNS_FQUERY_THREAD,
};
/** service descriptor
 *
 * Central to mdns is the notion of a service.  Hosts advertise service types
 * such as a website, a printer, some custom service, etc.  Network users can
 * use an mdns browser to discover services on the network.  Internally, this
 * mdns implementation uses the following struct to describe a service.  These
 * structs can be created by a user, populated, and passed to mdns announce
 * functions to specify services that are to be advertised. When a user starts a
 * query for services, the discovered services are passed back to the user in
 * this struct.
 *
 * The members include:
 *
 * servname: string that is the service instance name that will be advertised.
 * It could be something like "Brian's Website" or "Special Service on Device
 * #123".  This is the name that is typically presented to users browsing for
 * your service.  The servname must not exceed MDNS_MAX_LABEL_LEN bytes.  The
 * MDNS specification allows servname to be a UTF8 string.  However, only the
 * ascii subset of UTF-8 has been tested.
 *
 * servtype: string that represents the service type.  This should be a type
 * registered at http://dns-sd.org/ServiceTypes.html.  For example, "http" is
 * the service type for a web server and "ssh" is for an ssh server.  You may
 * use an unregisterd service type during development, but not in released
 * products.  Consider registering any new service types at the aforementioned
 * webpage.  servtype must be non-NULL.
 *
 * domain: string that represents the domain. If this value is NULL, domain
 * name ".local" will be used. The domain must not exceed
 * \ref MDNS_MAX_LABEL_LEN bytes.
 *
 * port: the tcp or udp port on which the service named servname is available
 * in network byte order.
 *
 * proto: Either MDNS_PROTO_TCP or MDNS_PROTO_UDP depending on what protocol
 * clients should use to connect to the service servtype.
 *
 * keyvals: NULL-terminated string of colon-separated key=value pairs.  These
 * are the key/value pairs for the TXT record associated with a service type.
 * For example, the servtype "http" defines the TXT keys "u", "p", and "path"
 * for the username, password, and path to a document.  If you supplied all of
 * these, the keyvals string would be:
 *
 * "u=myusername:p=mypassword:path=/index.html"
 *
 * If keyvals is NULL, no TXT record will be advertised.  If keyvals is ":", a
 * TXT record will appear, but it will not contain any key/value pairs.  The
 * key must be present (i.e., two contiguous ':' characters should never appear
 * in the keyvals string.)  A key may appear with no value.  The interpretation
 * of this depends on the nature of the service.  The length of a single
 * key/value pair cannot exceed MDNS_MAX_KEYVAL_LEN bytes.
 * It is the responsibility of the application to verify that the keyval string
 * is a valid string. The keyval string buffer is used by the mDNS module
 * internally and it can modify it. Hence, during subsequent calls to the mDNS
 * module, it is possible that the original string has been messed up and needs
 * to be recreated.
 *
 * ipaddr, fqsn, ptrname, kvlen and flags are for internal use only and should
 * not be dereferenced by the user.
 */
struct mdns_service
{
	/** Name of MDNS service  */
	const char *servname;
	/** Type of MDNS service */
	const char *servtype;
	/** Domain for MDNS service */
	const char *domain;
	/** Port number  */
	uint16_t port;
	/** Protocol used */
	int proto;
	/** Key value pairs for TXT records*/
	char *keyvals;
	/** IP Address of device */
	uint32_t ipaddr;
#ifdef CONFIG_IPV6
	uint32_t ip6addr[4];
#endif /* CONFIG_IPV6 */

	/** The following members are for internal use only and should not be
	 * dereferenced by the user.
	 */
	uint8_t fqsn[MDNS_MAX_NAME_LEN];
	/** PTR record name */
	uint8_t *ptrname;
	/** Length of keyvals string*/
	uint16_t kvlen;
	/**  MDNS flags */
	uint32_t flags;
};

/** Maximum no. of services allowed to be announced on a single interface. */
#define MAX_MDNS_LST 5
/** Maximum no. of different interfaces supported by mDNS. */
#define MAX_INTERFACE 2

/* Total number of interface config supported by mdns */
#define MDNS_MAX_SERVICE_CONFIG 2

/** protocol values for the proto member of the mdns_service descriptor */
/** TCP Protocol */
#define MDNS_PROTO_TCP 0
/** UDP Protocol */
#define MDNS_PROTO_UDP 1

/** mdns_start
 *
 * Start the responder thread (and the querier thread if querying is enabled).
 *
 *
 * Note that the mdns_start() function must be called only after the network
 * stack is up.
 *
 * The responder thread wait for application's request to announce services.
 * Using mdns_announce_service() and mdns_announce_service_arr() call, services
 * are added to mDNS service list and are announced on a given interface.
 *
 * The querier thread just opens a socket and then waits until a request for
 * monitoring any given service type or subtype is received from the
 * application. When received, the same is queried and the responses are
 * processed, cached and monitored.
 * 
 * \param domain domain name string.  If this value is NULL, the domain ".local"
 * will be used. The domain must not exceed \ref MDNS_MAX_LABEL_LEN bytes.
 *
 * \param hostname string that is the hostname to resolve. This would be the
 * "foo" in "foo.local", for example.  The hostname must not exceed
 * \ref MDNS_MAX_LABEL_LEN bytes.  If hostname is NULL, the responder capability
 * will not be launched, only query support will be enabled.  This is useful if
 * only the query functionality is desired.
 *
 * \return WM_SUCCESS for success or mdns error code
 *
 * NOTES:
 *
 * The domain and hostname must persist and remain unchanged between calls to
 * mdns_start and mdns_stop. Hence define these variables in global memory.
 *
 * While mdns_start returns immediately, the hostname and any servnames may
 * not be unique on the network.  In the event of a conflict, the names will
 * be appended with an integer.  For example, if the hostname "foo.local" is
 * taken, mdns will attempt to claim "foo-2.local", then foo-3.local, and so on
 * until the conflicts cease.  If mdns gets all the way to foo-9.local and
 * still fail, it waits for 5 seconds (per the mDNS specification) and then
 * starts back at foo.local.  If a developer anticipates a network to have many
 * of her devices, she should devise a sensible scheme outside of mdns to
 * ensure that the names are unique.
 */
int mdns_start(const char *domain, char *hostname);

/** 
 * mdns_stop
 *
 * Halt the mDNS responder thread (and querier thread if querying is enabled),
 * delete the threads and close the sockets
 *
 * Any services being monitored will be unmonitored.
 */
void mdns_stop(void);

/** Note Configuring CONFIG_MDNS_SERVICE_CACHE_SIZE in mDNS Querier
 *
 * This is a menuconfig option and can be configured from "make menuconfig" ->
 * Modules -> MDNS -> mDNS Full-Fledged Query(monitoring) API -> Maximum number
 * of service instances that can be monitored.
 *
 * Suppose CONFIG_MDNS_SERVICE_CACHE_SIZE is 16 and that a user has invoked
 * mdns_query_monitor to monitor services of type _http._tcp.local. Assume
 * the query callback handler returns WM_SUCCESS for all the instances
 * discovered.
 *
 * Further,suppose that this particular domain has 17 instances of this type.
 * The first 16 instances to be discovered will result in 16 callbacks with the
 * status MDNS_DISCOVERED.  These instances will be cached and monitored for
 * updates, disappearance, etc.  When the 17th instance is discovered, the
 * callback will be called as usual, but the status will be MDNS_CACHE_FULL,
 * and the service will not be monitored.  While a best effort is made to
 * deliver all of the service information, the mdns_service may be incomplete.
 * Specifically, the ipaddr may be 0 and the service name may be "".  Further,
 * the callback may be called again if the 17th instance of the service
 * announces itself on the network again.  If one of the other services
 * disappears, the next announcement from the 17th instance will result in a
 * callback with status MDNS_DISCOVERED, and from that point forward it will be
 * monitored.
 *
 * So what's the "best" value for CONFIG_MDNS_SERVICE_CACHE_SIZE?  This depends
 * on the application and on the field in which the application is deployed.  If
 * a particular application knows that it will never see more than 6 instances
 * of a service, then 6 is a fine value for CONFIG_MDNS_SERVICE_CACHE_SIZE.  In
 * this case, callbacks with a status of MDNS_CACHE_FULL would represent a
 * warning or error condition.  Similarly, if an application cannot handle any
 * more than 10 instances of a service, then CONFIG_MDNS_SERVICE_CACHE_SIZE
 * should be 10 and callbacks with a status of MDNS_CACHE_FULL can be ignored.
 * If the maximum number of service instances is not known, and the application
 * retains its own state for each instance of a service, it may be able to use
 * that state to do the right thing when the callback status is MDNS_CACHE_FULL.
 *
 * For applications with constrained memory ,a point to note is that each
 * service instance requires little above 1K bytes. This should be considered
 * while deciding the CONFIG_MDNS_SERVICE_CACHE_SIZE.
 *
 * The default value of CONFIG_MDNS_SERVICE_CACHE_SIZE is set to 2.
 */


/** mdns query callback
 *
 * A user initiates a query for services by calling the mdns_query_monitor
 * function with a fully-qualified service type, an mdns_query_cb, and an
 * opaque argument.  When a service instance is discovered, the query callback
 * will be invoked with following arguments:
 *
 * \param data a void * that was passed to mdns_query_monitor().  This can be
 * anything that the user wants, such as pointer to a custom internal data
 * structure.
 *
 * \param s A pointer to the struct mdns_service that was discovered.  The struct
 * mdns_service is only valid until the callback returns.  So if attributes of
 * the service (such as IP address and port) are required by the user for later
 * use, they must be copied and preserved elsewhere.
 *
 * \param status A code that reports the status of the query.  It takes one of the
 * following values:
 *
 * MDNS_DISCOVERED: The mdns_service s has just been discovered on the network
 * and will be monitored by the mdns stack.
 *
 * MDNS_UPDATED: The mdns_service s, which is being monitored, has been updated
 * in some way (e.g., it's IP address has changed, it's key/value pairs have
 * changed.)
 *
 * MDNS_DISAPPEARED: The mdns_service has left the network.  This usually
 * happens when a service has shut down, or when it has stopped responding
 * queries.  Applications may also detect this condition by some means outside
 * of mdns, such as a closed TCP connection.
 *
 * MDNS_CACHE_FULL: The mdns_service has been discovered.  However, the number
 * of monitored service instances has exceeded CONFIG_MDNS_SERVICE_CACHE_SIZE.
 * So the returned mdns_service may not be complete.  See NOTES below on other
 * implications of an MDNS_CACHE_FULL status.
 *
 * NOTES:
 *
 * The query callback should return WM_SUCCESS in the case where it has
 * discovered service of interest (MDNS_DISCOVERED, MDNS_UPDATED status). If
 * the callback return non-zero value for MDNS_DISCOVERED and MDNS_UPDATED
 * status codes, that particular service instance is not cached by the mDNS
 * querier. This is required as each cached service instance takes little above
 * 1KB memory and the device can't monitor large number of service instances.
 *
 * Callback implementers must take care to not make any blocking calls, nor to
 * call any mdns API functions from within callbacks.
 *
 */
typedef int (* mdns_query_cb)(void *data, const struct mdns_service *s,
			int status);

#define MDNS_DISCOVERED		1
#define MDNS_UPDATED		2
#define MDNS_DISAPPEARED	3
#define MDNS_CACHE_FULL		4

/** mdns_query_monitor
 *
 * Query for and monitor instances of a service
 *
 * When instances of the specified service are discovered, the specified
 * query callback is called as described above.
 *
 * \param fqst Pointer to a null-terminated string specifying the fully-qualified
 * service type.  For example, "_http._tcp.local" would query for all http
 * servers in the ".local" domain.
 *
 * \param cb an mdns_query_cb to be called when services matching the specified fqst
 * are discovered, are updated, or disappear.  cb will be passed the opaque
 * data argument described below, a struct mdns_service that represents the
 * discovered service, and a status code.
 *
 * \param data a void * that will passed to cb when services are discovered, are
 * updated, or disappear.  This can be anything that the user wants, such as
 * pointer to a custom internal data structure.
 *
 * \param iface Interface handle on which services are to be queried. Interface
 * handle can be obtained from net_get_sta_handle() or net_get_uap_handle()
 * function calls
 *
 * \return WM_SUCCESS: the query was successfully launched.  The caller should expect
 * the mdns_query_cb to be invoked as instances of the specified service are
 * discovered.
 *
 * \return WM_E_MDNS_INVAL: cb was NULL or fqst was not valid.
 *
 * \return -WM_E_MDNS_NOMEM: CONFIG_MDNS_MAX_SERVICE_MONITORS is already being
 * monitored.  Either this value must be increased, or a service must be
 * unmonitored by calling mdns_query_unmonitor.
 *
 * \return -WM_E_MDNS_INUSE: The specified service type is already being monitored by another
 * callback, and multiple callbacks per service are not supported.
 *
 * \return -WM_E_MDNS_NORESP: No response from the querier.  Perhaps it was not launched or
 * it has crashed.
 *
 * Note: multiple calls to mdns_query_service_start are allowed.  This enables
 * the caller to query for more than just one service type.
 */
int mdns_query_monitor(char *fqst, mdns_query_cb cb, void *data, void *iface);

/** mdns_query_unmonitor
 *
 * Stop monitoring a particular service
 *
 * \param fqst The service type to stop monitoring, or NULL to unmonitor all
 * services.
 *
 * \note Suppose a service has just been discovered and is being processed
 * while the call to mdns_query_monitor is underway.  A callback may be
 * generated before the service is unmonitored.
 */
void mdns_query_unmonitor(char *fqst);

/** mdns_announce_service
 *
 * Announce single mDNS service on an interface
 *
 * This function checks validity of service and if service is invalid
 * then it will return with appropriate error code.
 * Function sends command on mDNS control socket for announcing service on
 * "iface" interface. mDNS then announces the service by following prescribed
 * set of steps mentioned below:
 *  -# Send probing packets for a given service or services 3 times, spaced 250
 *  ms apart.
 *  -# If a conflict is received for any service record, resolve them by
 *  appending a number to that particular record to make it unique. Go to step
 *  1.
 *  -# Announce the service or services.
 * In order to announce multiple services simultaneously, use
 * mdns_announce_service_arr(). This is recommended to reduce network
 * congestion. Maximum no. of services that can be announced per interface
 * is \ref MAX_MDNS_LST
 *
 * \param service Pointer to \ref mdns_service structure corresponding to
 * service to be announced. Structure must persist and remain unchanged between
 * calls to mdns_start and mdns_stop. Hence define this variable in global
 * memory.
 *
 * \param iface Pointer to interface handle on which service is to be announced.
 * Interface handle can be obtained from net_get_sta_handle or
 * net_get_uap_handle function calls.
 *
 * \return WM_SUCCESS: success
 *
 * \return -WM_E_MDNS_INVAL: input was invalid.  Perhaps a label exceeded
 * MDNS_MAX_LABEL_LEN, or a name composed of the supplied label exceeded
 * MDNS_MAX_NAME_LEN. Perhaps hostname was NULL and the query capability is not
 * compiled. Perhaps service pointer or iface pointer is NULL
 *
 * \return -WM_E_MDNS_BADSRC: one of the service descriptors in the service
 * was invalid.  Perhaps key/val pair exceeded MDNS_MAX_KEYVAL_LEN
 *
 * \return -WM_E_MDNS_TOOBIG: The combination of name information and service
 * descriptors does not fit in a single packet, which is required by this
 * implementation
 *
 * \return -WM_FAIL: No space to add new interface or a new service in a given
 * interface.
 *
 * \return -WM_E_MDNS_FSOC: failed to create loopback control socket
 *
 * \return -WM_E_MDNS_FBIND: failed to bind control socket
 *
 */
int mdns_announce_service(struct mdns_service *service, void *iface);

/** mdns_deannounce_service
 *
 * Deannounce single service from an interface
 *
 * This function sends command on mDNS control socket to deannounce a given
 * service on "iface" interface. mDNS then de-announces the service by sending
 * a good bye packet with ttl=0 so that mDNS clients can remove the services
 * from their mDNS cache table. Function removes service from a list of services
 * associated with that interface.
 * In order to de-announce multiple services simultaneously, use
 * mdns_deannounce_service_arr(). This is recommended to reduce network
 * congestion.
 *
 * \param service Pointer to \ref mdns_service structure corresponding to
 * service to be de-announced
 *
 * \param iface Pointer to interface handle on which service is to be
 * de-announced. Interface handle can be obtained from net_get_sta_handle or
 * net_get_uap_handle function calls
 *
 * \return WM_SUCCESS: success
 *
 * \return -WM_E_MDNS_INVAL: invalid parameters. Perhaps service pointer or
 * iface pointer is NULL
 *
 * \return -WM_E_MDNS_FSOC: failed to create loopback control socket
 *
 * \return -WM_E_MDNS_FBIND: failed to bind control socket
 *
 */
int mdns_deannounce_service(struct mdns_service *service, void *iface);

/* mdns_announce_service_arr
 *
 * Announce multiple services simultaneously
 *
 * This function checks validity of all services in array and if any service is
 * invalid then it will return with appropriate error code.
 * Function sends command on mDNS control socket for announcing services in a
 * single mDNS packet on "iface" interface. mDNS follows same set of steps
 * as described in mdns_announce_service(), to announce given services.
 * Maximum no. of services that can be announced per interface is
 * \ref MAX_MDNS_LST. If no. of services in an array exceeds \ref MAX_MDNS_LST,
 * no services from given array are announced.
 *
 * \param services array of pointers to \ref mdns_service structure
 * corresponding to services to be announced. Array must persist and remain
 * unchanged between calls to mdns_start and mdns_stop. Hence define this
 * variable in global memory. Array should be NULL terminated.
 *
 * \param iface Pointer to interface handle on which service is to be announced.
 * Interface handle can be obtained from net_get_sta_handle or
 * net_get_uap_handle function calls.
 *
 * \return WM_SUCCESS: success
 *
 * \return -WM_E_MDNS_INVAL: input was invalid.  Perhaps a label exceeded
 * MDNS_MAX_LABEL_LEN, or a name composed of the supplied labels exceeded
 * MDNS_MAX_NAME_LEN. Perhaps hostname was NULL and the query capability is not
 * compiled. Perhaps services pointer or iface pointer is NULL
 *
 * \return -WM_E_MDNS_BADSRC: one of the service descriptors in the services
 * list was invalid.  Perhaps one of the key/val pairs exceeded
 * MDNS_MAX_KEYVAL_LEN
 *
 * \return -WM_E_MDNS_TOOBIG: The combination of name information and service
 * descriptors does not fit in a single packet, which is required by this
 * implementation
 *
 * \return -WM_FAIL: No space to add new interface or services in a given
 * interface. Maximum no. of services that can be announced per interface is
 * \ref MAX_MDNS_LST
 *
 */
int mdns_announce_service_arr(struct mdns_service *services[], void *iface);

/* Re-probe and announce all service of given interface
 *
 * This function dispatches command to mDNS responder to begin probing of and
 * subsequently announce all previously announced services on a given interface.
 * This function must be used in cases where the host has not been continuously
 * connected and participating on the network link, it MUST first probe to
 * re-verify uniqueness of its unique records. This function must be called once
 * device is re-connected after any network abruptions such as Link Loss or when
 * device wakes up from sleep mode. Please refer to RFC 6762 section 8, Probing
 * and Announcing on Startup, for more details.
 *
 * \param iface Pointer to interface handle on which service is to be announced.
 * Interface handle can be obtained from net_get_sta_handle or
 * net_get_uap_handle function calls.
 *
 * \return WM_SUCCESS: success or failure otherwise.
 */
int mdns_announce_service_all(void *iface);

/* mdns_deannounce_service_arr
 *
 * Deannounce mulitple services simultaneously
 *
 * This function sends command on mDNS control socket to de-announce given set
 * of services in a single mDNS packet on "iface" interface.
 * mDNS then de-announce services as described in mdns_deannounce_service().
 * Function removes given set of services from a list of services associated
 * with that interface.
 *
 * \param services array of pointer to \ref mdns_service structure corresponding
 * to services to be de-announced
 *
 * \param iface Pointer to interface handle on which service is to be
 * de-announced. Interface handle can be obtained from net_get_sta_handle or
 * net_get_uap_handle function calls
 *
 * \return WM_SUCCESS: success
 *
 * \return -WM_E_MDNS_INVAL: invalid parameters. Perhaps services pointer or
 * iface pointer is NULL
 *
 * \return -WM_E_MDNS_FSOC: failed to create loopback control socket
 *
 * \return -WM_E_MDNS_FBIND: failed to bind control socket
 *
 */
int mdns_deannounce_service_arr(struct mdns_service *services[], void *iface);

/* mdns_deannounce_service_all
 *
 * Deannounce all registered services on a given interface
 *
 * This function sends command on mDNS control socket to de-announce all
 * services, in a single mDNS packet, announced on "iface" interface. mDNS then
 * de-announce services as described in mdns_deannounce_service(). Function
 * removes all services from a list of services associated with that interface.
 *
 * \param iface Pointer to interfae handle on which service is to be
 * de-announced. Interface handle can be obtained from net_get_sta_handle or
 * net_get_uap_handle function calls.
 */
int mdns_deannounce_service_all(void *iface);
int mdns_iface_state_change(void  *iface, enum iface_state state)\
	    __DEPRECATED__("Warning: This API will be removed in " \
		"the 3.4 release. Use mdns_announce_service_all for UP, "\
		"mdns_deannounce_service_all for DOWN, and " \
		"mdns_reannounce_service_all for REANNOUNCE");

/* Re-announce particular service
 *
 * This function re-announces specified service on a given interface. Note that
 * service must be previously announced on the same interface.
 * At any time, if the resource data of any of a host's Multicast DNS records
 * changes, the host MUST reannounce to update neighboring caches. This function
 * must be called if any of the resource data changes. For example, if TXT
 * record of any service changes, service must be reannounced. Please refer to
 * RFC 6762, section 8.4 Updating, for more details.
 *
 * \param service Pointer to mDNS service to be re-announced.
 *
 * \param iface Pointer to interface handle on which service is to be
 * re-announced. Interface handle can be obtained from net_get_sta_handle or
 * net_get_uap_handle function calls.
 *
 * \return WM_SUCCESS: success or failure otherwise.
 *
 */
int mdns_reannounce_service(struct mdns_service *service, void *iface);

/* Re-announce all previously announced services
 *
 * This function re-announces all services previously announced on a given
 * interface. At any time, if the resource data of any of a host's Multicast DNS
 * records changes, the host MUST reannounce to update neighboring caches. This
 * function must be called if any of the resource data changes. For example, if
 * IP address of the device changes all services of that interface must be
 * reannounced. Pleas refer to RFC 6762, section 8.4 Updating, for more details.
 *
 * \param iface Pointer to interface handle on which service is to be
 * re-announced. Interface handle can be obtained from net_get_sta_handle or
 * net_get_uap_handle function calls.
 *
 * \return WM_SUCCESS: success or failure otherwise.
 *
 */
int mdns_reannounce_service_all(void *iface);

void mdns_set_hostname(char *hostname);
int mdns_stat();

#ifdef CONFIG_MDNS_TESTS
/* mdns_tests
 *
 * Run internal mdns tests
 *
 * This function is useful to verify that various internal mdns functionality
 * is properly working after a port, or after adding new features.  If
 * MDNS_TEST is undefined, it is an empty function.  It is not meant to be
 * compiled or run in a production system.  Test output is written using
 * mdns_log().
 */
void mdns_tests(void);

int mdns_cli_init(void);
void dname_size_tests(void);
void dname_cmp_tests(void);
void increment_name_tests(void);
void txt_to_c_ncpy_tests(void);

#endif

#ifdef CONFIG_DNSSD_QUERY
/** dnssd_query_monitor
 *
 * Query for and monitor instances of a service published by unicast DNS server.
 *
 * When instances of the specified service are discovered, the specified
 * query callback is called as described above.
 *
 * \param fqst Pointer to a null-terminated string specifying the fully-
 * qualified service type.  For example, "_http._tcp.marvelltest" would query
 * for all http servers in the ".marvelltest" domain.
 *
 * \param cb an mdns_query_cb to be called when services matching the specified
 * fqst are discovered, are updated, or disappear.  Callback will be passed the
 * opaque data argument described below, a struct mdns_service that represents
 * the discovered service, and a status code.
 *
 * \param dns_addr DNS IP address in form of struct in_addr. One can get the IP
 * from DNS string using dns_gethostbyname API.
 *
 * \param data a void * that will passed to cb when services are discovered,
 * are updated, or disappear.  This can be anything that the user wants, such
 * as pointer to a custom internal data structure.
 *
 * \return WM_SUCCESS: the query was successfully launched.  The caller should
 * expect the mdns_query_cb to be invoked as instances of the specified service
 * are discovered.
 *
 * \return WM_E_MDNS_INVAL: cb was NULL or fqst was not valid.
 *
 * \return -WM_E_MDNS_NOMEM: CONFIG_MDNS_MAX_SERVICE_MONITORS is already being
 * monitored.  Either this value must be increased, or a service must be
 * unmonitored by calling mdns_query_unmonitor.
 *
 * \return -WM_E_MDNS_INUSE: The specified service type is already being
 * monitored by another callback, and multiple callbacks per service are not
 * supported.
 *
 * \return -WM_E_MDNS_NORESP: No response from the querier.  Perhaps it was not
 * launched or it has crashed.
 *
 * Note #1: Only single dnssd query monitor is allowed.
 * Note #2: Internally, state machines for DNS and mDNS queriers are same, hence
 * to save the footprint DNS and mDNS queriers are merged together.
 */
int dnssd_query_monitor(char *fqst, mdns_query_cb cb,
			struct in_addr dns_addr, void *data, void *iface);

/** dnssd_query_unmonitor
 *
 * Stop monitoring a particular DNS-SD service on unicast socket
 *
 * \param fqst The service type to stop monitoring, or NULL to unmonitor all
 * services.
 *
 * \note Suppose a service has just been discovered and is being processed
 * while the call to mdns_query_monitor is underway.  A callback may be
 * generated before the service is unmonitored.
 */
void dnssd_query_unmonitor(char *fqst);
#endif

/** Set mDNS TXT Records
 *
 * This function sets the TXT record field for a given mDNS service.
 * mDNS TXT record string is in-place converted and stored in a format defined
 * by TXT resource record format.
 *
 * \note This function MUST be called before adding or announcing
 * the mDNS service. Otherwise the resultant behavior can be erroneous.
 *
 * \param s Pointer to mDNS service
 *
 * \param keyvals TXT record string consisting of one or more key value pairs,
 * which can be separated by a separator like ':', '|'. For example,
 * \code
 * char keyvals[] = "key1=value1|key2=value2|key3=value3"
 * \endcode
 * The separator value is also passed to this function as mentioned below.
 * keyvals string must persist between the calls to mdns_start and mdns_stop.
 * Hence define these variables in global memory
 *
 *
 * \param separator The separator used to separate individual key value pairs in
 * the above mentioned TXT record string. If TXT record contains single
 * key value pair then set separator value appropriately. For example, it can
 * be set to '|' if the character '|' is not used in key value pair.
 *
 * \return WM_SUCCESS on success
 *
 * \return -WM_E_MDNS_TOOBIG if length of TXT record exceeds the permissible
 * limit as specified by \ref MDNS_MAX_KEYVAL_LEN
 *
 * \return -WM_E_MDNS_INVAL if value of TXT record string is invalid
 *
 */
int mdns_set_txt_rec(struct mdns_service *s, char *keyvals, char separator);
#endif
