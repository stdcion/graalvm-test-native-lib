#ifndef DXFEED_GRAAL_API_DXF_GRAAL_API_H_
#define DXFEED_GRAAL_API_DXF_GRAAL_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dfx_types.h"

/** @defgroup System
*  @{
*/

/**
 * @brief Structure representing an isolate.
 */
typedef struct graal_isolate_t graal_isolate_t;

/**
 * @brief The pointer to the runtime data structure for a thread.
 */
typedef struct graal_isolate_thread_t graal_isolate_thread_t;

/**
 * @brief Sets the system property indicated by the specified key.
 * @param[in]  isolate  The pointer to the runtime data structure for a thread.
 * @param key   The name of the system property.
 * @param value The value of the system property.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_system_set_property(graal_isolate_thread_t *isolate, const char *key, const char *value);

/** @} */ // end of System

/** @defgroup Endpoint
*  @{
*/

/**
 * @brief Creates new dxf_endpoint_t instance.
 * dxf_endpoint_t is used for connect/disconnect from remote hosts, receive instances of other classes.
 * @param[in]  isolate  The pointer to the runtime data structure for a thread.
 * @param[out] endpoint The created endpoint.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_endpoint_create(graal_isolate_thread_t *isolate, dxf_endpoint_t *endpoint);

/**
 * @brief Connects to the specified remote address.
 * Previously established connections are closed if the new address is different from the old one.
 * This method does nothing if address does not change or if this endpoint is DXF_ENDPOINT_CLOSED.
 * The endpoint state immediately becomes DXF_ENDPOINT_CONNECTING otherwise.
 * The address string is provided with the market data vendor agreement.
 * Use "demo.dxfeed.com:7300" for a demo quote feed.
 * The simplest address strings have the following format:
 * <ul>
 *  <li> host:port       To establish a TCP/IP connection.
 *  <li> file:filename   Connects to a file.
 * </ul>
 * @note This method does not wait until connection actually gets established.
 * The actual connection establishment happens asynchronously after the invocation of this method.
 * However, this method waits until notification about state transition
 * from DXF_ENDPOINT_NOT_CONNECTED to DXF_ENDPOINT_CONNECTING gets processed
 * by all listeners that were installed via dxf_endpoint_add_state_change_listener() method.
 * @param[in] isolate  The pointer to the runtime data structure for a thread.
 * @param[in] endpoint The descriptor that was created by dxf_endpoint_create.
 * @param[in] address  The address string.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_endpoint_connect(graal_isolate_thread_t *isolate, dxf_endpoint_t endpoint, const char *address);

/**
 * @brief Terminates all established network connections and initiates connecting again with the same address.
 * The effect of the method is alike to invoking dxf_endpoint_disconnect
 * and dxf_endpoint_connect with the current address,
 * but internal resources used for connections may be reused by implementation.
 * @note The method will not connect endpoint that was not initially connected
 * with dxf_endpoint_connect method or was disconnected with dxf_endpoint_disconnect() method.
 * The method initiates a short-path way for reconnecting, so whether observers
 * will have a chance to see an intermediate state DXF_ENDPOINT_NOT_CONNECTED depends on the implementation.
 * @param[in] isolate  The pointer to the runtime data structure for a thread.
 * @param[in] endpoint The descriptor that was created by dxf_endpoint_create.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_endpoint_reconnect(graal_isolate_thread_t *isolate, dxf_endpoint_t endpoint);

/**
 * @brief Terminates all remote network connections.
 * This method does nothing if this endpoint is DXF_ENDPOINT_CLOSED.
 * The endpoint state immediately becomes DXF_ENDPOINT_NOT_CONNECTED otherwise.
 * This method does not release all resources that are associated with this endpoint.
 * Use dxf_endpoint_close() method to release all resources.
 * @param[in] isolate  The pointer to the runtime data structure for a thread.
 * @param[in] endpoint The descriptor that was created by dxf_endpoint_create.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_endpoint_disconnect(graal_isolate_thread_t *isolate, dxf_endpoint_t endpoint);

/**
 * @brief Closes this endpoint.
 * All network connection are terminated as with disconnect method and no further connections can be established.
 * The endpoint state immediately becomes DXF_ENDPOINT_CLOSED. All resources associated with this endpoint are released.
 * Once closed, the dxf_endpoint_t descriptor cannot be used in other functions.
 * Removes all internal ref to DXEndpoint.
 * @param[in] isolate  The pointer to the runtime data structure for a thread.
 * @param[in] endpoint The descriptor that was created by dxf_endpoint_create.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_endpoint_close(graal_isolate_thread_t *isolate, dxf_endpoint_t endpoint);

/**
 * @brief Gets feed that is associated with this endpoint.
 * @param[in] isolate  The pointer to the runtime data structure for a thread.
 * @param[in] endpoint The descriptor that was created by dxf_endpoint_create.
 * @param[out] feed The dxf_feed_t. Is used for retrieve events, create subscriptions.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_endpoint_get_feed(graal_isolate_thread_t *isolate, dxf_endpoint_t endpoint, dxf_feed_t *feed);

/**
 * @brief Returns the state of this endpoint.
 * @param[in] isolate The pointer to the runtime data structure for a thread.
 * @param[in] endpoint The descriptor that was created by dxf_endpoint_create.
 * @return The state.
 */
dxf_endpoint_state_t dxf_endpoint_get_state(graal_isolate_thread_t *isolate, dxf_endpoint_t endpoint);

/**
 * @brief Adds listener that is notified about changes in state connections.
 * Installed listener can be removed with dxf_endpoint_remove_state_change_listener method.
 * @param[in] isolate  The pointer to the runtime data structure for a thread.
 * @param[in] endpoint The descriptor that was created by dxf_endpoint_create.
 * @param[in] listener The listener to add.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_endpoint_add_state_change_listener(graal_isolate_thread_t *isolate,
                                                  dxf_endpoint_t endpoint,
                                                  dxf_endpoint_on_changing_state listener);

/**
 * @brief Removes listener that is notified about changes in state property.
 * It removes the listener that was previously installed with addStateChangeListener method
 * @param[in] isolate  The pointer to the runtime data structure for a thread.
 * @param[in] endpoint The descriptor that was created by dxf_endpoint_create.
 * @param[in] listener The listener to remove.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_endpoint_remove_state_change_listener(graal_isolate_thread_t *isolate,
                                                     dxf_endpoint_t endpoint,
                                                     dxf_endpoint_on_changing_state listener);

/** @} */ // end of Endpoint


/** @defgroup Subscription
*  @{
*/

/**
 * @brief Creates new subscription for a events type that is attached to this feed.
 * @param[in]  isolate    The pointer to the runtime data structure for a thread.
 * @param[in]  feed       The feed.
 * @param[in]  events     The array of type market events.
 * @param[in] eventsCount The events count.
 * @param[out] sub        The subscription.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_subscription_create(graal_isolate_thread_t *isolate,
                                   dxf_feed_t feed,
                                   dxf_market_events_t *events,
                                   size_t eventsCount,
                                   dxf_subscription_t *sub);

/**
 * @brief Adds listener for events. Event lister can be added only when subscription is not producing any events.
 * @param[in] isolate  The pointer to the runtime data structure for a thread.
 * @param[in] sub      The subscription.
 * @param listener     The event listener.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_subscription_add_event_listener(graal_isolate_thread_t *isolate,
                                               dxf_subscription_t sub,
                                               dxf_subscription_event_listener listener);

/**
 * @brief Removes listener for events.
 * @param[in] isolate  The pointer to the runtime data structure for a thread.
 * @param[in] sub      The subscription.
 * @param listener     The event listener.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_subscription_remove_event_listener(graal_isolate_thread_t *isolate,
                                                  dxf_subscription_t sub,
                                                  dxf_subscription_event_listener listener);

/**
 * @brief Adds the specified array of symbols to the set of subscribed symbols.
 * @param[in] isolate  The pointer to the runtime data structure for a thread.
 * @param[in] sub      The subscription.
 * @param[in] symbols  An array of symbols to add.
 * @param[in] count    The count remove symbols.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_subscription_add_symbols(graal_isolate_thread_t *isolate,
                                        dxf_subscription_t sub,
                                        const char *symbols[],
                                        size_t count);

/**
 * @brief Adds the specified array of indexed symbols to the set of subscribed symbols.
 * @param[in] isolate  The pointer to the runtime data structure for a thread.
 * @param[in] sub      The subscription.
 * @param[in] symbols  An array of symbols to add.
 * @param[in] count    The count remove symbols.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_subscription_add_indexed_symbols(graal_isolate_thread_t *isolate,
                                                dxf_subscription_t sub,
                                                dxf_indexed_symbol_t *symbols,
                                                size_t count);

/**
 * @brief Adds the specified array of time series symbols to the set of subscribed symbols.
 * @param[in] isolate  The pointer to the runtime data structure for a thread.
 * @param[in] sub      The subscription.
 * @param[in] symbols  An array of symbols to add.
 * @param[in] count    The count remove symbols.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_subscription_add_time_series_symbols(graal_isolate_thread_t *isolate,
                                                    dxf_subscription_t sub,
                                                    dxf_time_series_symbol_t *symbols,
                                                    size_t count);

/**
 * @brief Removes the specified array of symbols from the set of subscribed symbols.
 * @param[in] isolate  The pointer to the runtime data structure for a thread.
 * @param[in] sub      The subscription.
 * @param[in] symbols  An array of symbols to remove.
 * @param[in] count    The count remove symbols.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_subscription_remove_symbols(graal_isolate_thread_t *isolate,
                                           dxf_subscription_t sub,
                                           const char *symbols[],
                                           size_t count);

/**
 * @brief Clears the set of subscribed symbols.
 * @param[in] isolate  The pointer to the runtime data structure for a thread.
 * @param[in] sub      The subscription.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_subscription_clear(graal_isolate_thread_t *isolate, dxf_subscription_t sub);

/**
 * @brief Clears the set of subscribed symbols and closes this subscription.
 * This functions clears lists of all installed event listeners and subscription change
 * listeners and makes sure that no more listeners can be added.
 * Once closed, the dxf_subscription_t descriptor cannot be used in other functions.
 * Removes all internal ref to DXFeedSubscription.
 * @param[in] isolate  The pointer to the runtime data structure for a thread.
 * @param[in] sub      The subscription.
 * @return 0 - if the operation was successful; otherwise, an error code.
 */
ERROR_CODE dxf_subscription_close(graal_isolate_thread_t *isolate, dxf_subscription_t sub);

/** @} */ // end of Subscription

#ifdef __cplusplus
}
#endif

#endif //DXFEED_GRAAL_API_DXF_GRAAL_API_H_
